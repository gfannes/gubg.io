require(File.join(ENV['gubg'], 'shared'))
require('gubg/build/Executable')
require('gubg/build/Library')
include GUBG

task :default do
    sh "rake -T"
end

task :clean do
    rm_rf '.cache'
end

desc "Publish all headers"
task :declare do
    publish('src', pattern: '**/*.hpp', dst: 'include')
end

desc "Build library"
task :define => :declare do
    lib = Build::Library.new('gubg.io')
    lib.add_include_path(shared_dir('include'))
    lib.add_sources(FileList.new('src/gubg/**/*.cpp'))
    lib.add_sources(FileList.new(shared('include', '**/*.hpp')))
    lib.build
    publish(lib.lib_filename, dst: 'lib')
end

desc "Run the unit tests"
task :test => :define do
    Rake::Task['ut:test'].invoke
end

namespace :ut do
    ut = nil
    task :setup do
        ut = Build::Executable.new('unit_tests')
        case :debug
        when :debug
            ut.add_option('g')
        else
            ut.add_define('NDEBUG')
        end
        ut.add_include_path(shared_dir('include'))
        # ut.add_sources(FileList.new('src/test/**/*.cpp'))
        ut.add_sources(FileList.new('src/test/parse/**/*.cpp'))
        ut.add_sources(FileList.new(shared('include', '**/*.hpp')))
        ut.add_sources(shared_file('source', 'catch_runner.cpp'))
        ut.add_library_path(shared_dir('lib'))
        ut.add_library('gubg.io')
    end
    task :test => :setup do
        ut.build
        options = %w[-d yes -a]
        options << '[ut]'
        options << '[tree]'
        ut.run(options)
    end
end
