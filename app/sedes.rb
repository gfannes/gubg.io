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

$context = nil

class Namespace
    attr_reader(:name, :parent)
    def initialize(name, parent: nil)
        @name, @parent = name, parent
        @namespaces = {}
        @types = {}
    end
    def namespace(*names, &block)
        raise("Expected at least on namespace name") if names.empty?()
        name, rest = names[0], names[1..]
        ns = @namespaces[name] = Namespace.new(name, parent: self)
        if rest.empty?()
            block.call(ns) if block_given?()
        else
            ns = ns.namespace(*names, &block)
        end
        ns
    end
    def tuple(name, &block)
        t = @types.fetch(name){|name|@types[name] = Tuple.new(name, namespace: self)}
        block.call(t) if block_given?()
        t
    end
    def all_types()
        ary = @types.values()
        @namespaces.each do |name, namespace|
            ary += namespace.all_types()
        end
        ary
    end
    def to_a()
        ary = []
        p = self
        while p.parent()
            ary << p.name()
            p = p.parent()
        end
        ary.reverse()
    end
end

class Type
    attr_reader(:name, :namespace)
    def initialize(name, namespace: nil)
        @name, @namespace = name, namespace
        @namespace = namespace
    end
    def hr_name()
        @namespace*"::"+"::#{@name}"
    end
end

class Tuple < Type
    def initialize(name, namespace: nil)
        super(name, namespace: namespace)
        @types = {}
    end
    def int(name)
        add_primitive(name, :int)
    end
    def float(name)
        add_primitive(name, :float)
    end
    def write(language, fo)
        case language
        when :cpp
            fo.puts("    struct #{@name}{")
            @types.each do |name, type|
                fo.puts("        #{type} #{name}{};")
            end
            fo.puts("    };")
        else raise("Unsupported language #{language}")
        end
    end

    private
    def add_primitive(name, type)
        raise("tuple #{hr_name()} already has field #{name}") if @types.has_key?(name)
        @types[name] = type
    end
end

$global_namespace = Namespace.new(:"")
$context = $global_namespace

def namespace(*parts, &block)
    orig_context = $context
    ns = $context.namespace(*parts, &block)
    $context = orig_context
    ns
end

options[:inputs].each do |fp|
    fp_resolved = options[:paths].map{|dir|File.join(dir, fp)}.select{|fp|File.exist?(fp)}[0]
    raise("Could not find #{fp} in #{options[:paths]}") unless fp_resolved

    load(fp_resolved)
end

all_types = $global_namespace.all_types()
all_types.each do |type|
    puts()
    pp(type)
end

if fp = options[:output]
    File.open(fp, "w") do |fo|
        all_types.each do |type|
            fo.puts("namespace #{type.namespace().to_a()*'::'} {")
            type.write(:cpp, fo)
            fo.puts("}")
        end
    end
end