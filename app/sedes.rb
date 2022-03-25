require("optparse")

options = {
    verbose: 0,
    inputs: [],
    paths: [],
    output: nil,
}
OptionParser.new() do |opts|
    opts.banner = "Usage: blabla"

    opts.on("-v", "--verbose=LEVEL", "Verbosity level"){|level|options[:verbose] = level.to_i()}
    opts.on("-i", "--input=FILE", "Input file(s)"){|fp|options[:inputs] << fp}
    opts.on("-I", "--path=PATH", "Search path(s)"){|dir|options[:paths] << File.absolute_path(dir)}
    opts.on("-o", "--output=FILE", "Output file"){|fp|options[:output] = fp}
end.parse!()

log = ->(level, msg){puts(msg) if options[:verbose] >= level}
log.(1, options)

module Sedes
    def self.indent(level)
        '  '*level
    end

    class Item
        attr_reader(:name, :parent)
        def initialize(name, parent:)
            @name, @parent = name, parent
            @childs = {}
        end
        def path()
            ary = []
            p = self
            while p.parent()
                ary << p.name()
                p = p.parent()
            end
            ary.reverse()
        end
        def visit(guest)
            guest.open(self)
            @childs.each{|name, item|item.visit(guest)}
            guest.close(self)
        end

        private
        def context_call(new_context, &block)
            if block_given?()
                orig_context = $context
                $context = new_context
                block.call($context)
                $context = orig_context
            end
        end
    end

    class Root < Item
        def initialize()
            super(:"", parent: nil)
        end
        def mod(*names, &block)
            raise("Expected at least on mod name") if names.empty?()
            name, rest = names[0], names[1..]
            ns = @childs[name] = Mod.new(name, parent: self)
            if rest.empty?()
                context_call(ns, &block)
            else
                ns = ns.mod(*rest, &block)
            end
            ns
        end
    end

    class Mod < Item
        def initialize(name, parent:)
            super(name, parent: parent)
        end
        def mod(*names, &block)
            raise("Expected at least on mod name") if names.empty?()
            name, rest = names[0], names[1..]
            ns = @childs[name] = Mod.new(name, parent: self)
            if rest.empty?()
                context_call(ns, &block)
            else
                ns = ns.mod(*rest, &block)
            end
            ns
        end
        def tuple(name, &block)
            t = @childs.fetch(name){|name|@childs[name] = Tuple.new(name, parent: self)}
            context_call(t, &block)
            t
        end
        def typedef(name, type)
            @childs[name] = Typedef.new(name, type, parent: self)
        end
    end

    class Field < Item
        attr_reader(:type)
        def initialize(name, type, parent:)
            super(name, parent: parent)
            @type = type
        end
    end
    class Single < Field
        def initialize(name, type, parent:)
            super(name, type, parent: parent)
        end
    end
    class Array < Field
        attr_reader(:size)
        def initialize(name, type, size, parent:)
            super(name, type, parent: parent)
            @size = size
        end
    end
    class Vector < Field
        def initialize(name, type, parent:)
            super(name, type, parent: parent)
        end
    end

    class Tuple < Item
        def initialize(name, parent:)
            super(name, parent: parent)
        end

        def single(name, type)
            raise("tuple #{hr_name()} already has field #{name}") if @childs.has_key?(name)
            @childs[name] = Single.new(name, type, parent: self)
        end
        def array(name, type, size)
            raise("tuple #{hr_name()} already has field #{name}") if @childs.has_key?(name)
            @childs[name] = Array.new(name, type, size, parent: self)
        end
        def vector(name, type)
            raise("tuple #{hr_name()} already has field #{name}") if @childs.has_key?(name)
            @childs[name] = Vector.new(name, type, parent: self)
        end

        private
        def add_primitive_(name, type)
        end
    end

    class Typedef < Item
        attr_reader(:type)
        def initialize(name, type, parent:)
            super(name, parent: parent)
            @type = type
        end
    end

    class CppWriter
        def initialize(fo)
            @fo = fo
            @level = 0
        end
        def indent()
            '  '*@level
        end
        def write(msg)
            @fo.puts(indent()+msg)
        end

        def open(item)
            case item
            when Root    then write("#pragma once"); write("#include <array>"); write("#include <vector>")
            when Mod     then write("namespace #{item.name()} {"); @level += 1
            when Tuple   then write("struct #{item.name()} {"); @level += 1
            when Typedef then write("using #{item.name()} = #{item.type()};")
            when Single  then write("#{item.type()} #{item.name()};")
            when Array   then write("std::array<#{item.type()}, #{item.size()}> #{item.name()};")
            when Vector  then write("std::vector<#{item.type()}> #{item.name()};")
            else puts("open #{item.class}")
            end
        end
        def close(item)
            case item
            when Root
            when Mod     then @level -= 1; write("}")
            when Tuple   then @level -= 1; write("};")
            when Typedef
            when Single
            when Array
            when Vector
            else puts("close #{item.class}")
            end
        end
    end
end

$root = Sedes::Root.new()
$context = $root

def method_missing(method, *args, **kwargs, &block)
    case method
    when :mod, :tuple, :typedef, :single, :array, :vector
        $context.send(method, *args, **kwargs, &block)
    end
end

options[:inputs].each do |fp|
    fp_resolved = options[:paths].map{|dir|File.join(dir, fp)}.select{|fp|File.exist?(fp)}[0]
    raise("Could not find #{fp} in #{options[:paths]}") unless fp_resolved

    load(fp_resolved)
end

if fp = options[:output]
    File.open(fp, "w") do |fo|
        $root.visit(Sedes::CppWriter.new(fo))
    end
end