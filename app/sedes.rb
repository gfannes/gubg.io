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
        def write(language, fo, level)
            raise("Expected write() to be implemented for #{self.class}")
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
        def write(language, fo, level)
            @childs.each do |name, item|
                item.write(language, fo, level)
            end
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
        def write(language, fo, level)
            case language
            when :cpp
                fo.puts("#{Sedes::indent(level)}namespace #{@name} {")
                @childs.each do |name, item|
                    item.write(language, fo, level+2)
                end
                fo.puts("#{Sedes::indent(level)}}")
            else raise("Unsupported language #{language}")
            end
        end
    end

    class Field
        attr_reader(:name, :type)
        def initialize(name, type)
            @name, @type = name, type
        end
    end
    class Single < Field
        def initialize(name, type)
            super(name, type)
        end
        def write(language, fo, level)
            fo.puts("#{Sedes::indent(level)}#{type()} #{name()};")
        end
    end
    class Array < Field
        def initialize(name, type, size)
            super(name, type)
            @size = size
        end
        def write(language, fo, level)
            fo.puts("#{Sedes::indent(level)}#{type()} #{name()}[#{@size}];")
        end
    end

    class Tuple < Item
        def initialize(name, parent:)
            super(name, parent: parent)
        end

        def single(name, type)
            raise("tuple #{hr_name()} already has field #{name}") if @childs.has_key?(name)
            @childs[name] = Single.new(name, type)
        end
        def array(name, type, size)
            raise("tuple #{hr_name()} already has field #{name}") if @childs.has_key?(name)
            @childs[name] = Array.new(name, type, size)
        end

        def write(language, fo, level)
            case language
            when :cpp
                fo.puts("#{Sedes::indent(level)}struct #{@name}{")
                @childs.each do |name, field|
                    field.write(language, fo, level+1)
                end
                fo.puts("#{Sedes::indent(level)}};")
            else raise("Unsupported language #{language}")
            end
        end

        private
        def add_primitive_(name, type)
        end
    end

    class Typedef < Item
        def initialize(name, type, parent:)
            super(name, parent: parent)
            @type = type
        end
        def write(language, fo, level)
            case language
            when :cpp
                fo.puts("#{Sedes::indent(level)}using #{@name} = #{@type};")
            else raise("Unsupported language #{language}")
            end
        end
    end
end

$root = Sedes::Root.new()
$context = $root

def method_missing(method, *args, **kwargs, &block)
    case method
    when :mod, :tuple, :typedef, :single, :array
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
        $root.write(:cpp, fo, 0)
    end
end