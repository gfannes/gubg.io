require("gubg/shared")

here = File.dirname(__FILE__)

namespace :io do
	task :prepare do
		Dir.chdir(Gubg.mkdir(here, "extern")) do
			Gubg.git_clone('https://github.com/ikalnytskyi', 'termcolor') do
                sh "git pull --rebase"
            end
			Gubg.git_clone('https://github.com/nlohmann', 'json') do
                sh "git pull --rebase"
            end
			Gubg.git_clone('https://github.com/yhirose', 'cpp-httplib') do
                sh "git pull --rebase"
            end
			Gubg.git_clone('https://github.com/tplgy', 'cppcodec') do
                sh "git pull --rebase"
            end
        end
    end

    task :install do |t, args|
        require("gubg/build/Cooker")
        cooker = Build::Cooker.new().option("c++.std", 20).output("bin")

        recipes = filter_recipes(args, %w[sedes])
        cooker.generate(:ninja, *recipes).ninja()
    end
end

namespace :sedes do
    desc "End to end tests"
    task :ee => :"io:install" do
        test_cases = (0...3).to_a
        test_cases.each do |tc|
            base = "test/ee/sedes"
            Dir.chdir(Gubg.mkdir(here, base, tc)) do
                case tc
                when 0
                    sh "sedes -h"
                when 1
                    sh "sedes" do |ok,res|
                        raise "Should fail without arguments" if ok
                    end
                when 2
                    sh "sedes -i abc.naft -o abc.hpp"
                    sh "cat abc.hpp"
                    cooker = Gubg::Build::Cooker.new
                    cooker.option("c++.std", 20).generate(:ninja, "app").ninja()

                    %w[empty optional array].each do |what|
                        hr_fn = {a: "hr.#{what}.a.txt", b: "hr.#{what}.b.txt"}
                        naft_fn = {a: "naft.#{what}.a.txt", b: "naft.#{what}.b.txt"}
                        cooker.run("create", what, "hr.write", hr_fn[:a], "naft.write", naft_fn[:a])
                        cooker.run("naft.read", naft_fn[:a], "hr.write", hr_fn[:b], "naft.write", naft_fn[:b])
                    end
                end
            end
        end
    end
    task :rb do
        sh "ruby #{here}/app/sedes.rb -i test.rb -I #{here}/test/ee/sedes -o test.hpp"
        sh "cat test.hpp"
        sh "g++ test.cpp -I ./"
    end
end
