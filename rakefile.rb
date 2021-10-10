require("gubg/shared")

task :run
task :proper
task :clean

task :prepare do
	Dir.chdir(GUBG.mkdir("extern")) do
		GUBG.git_clone('https://github.com/ikalnytskyi', 'termcolor') do
		end
	end
end

