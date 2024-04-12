require 'service_manager'

ServiceManager.define_service 'nodes' do |s|
   s.start_cmd = 'cd test/node; ./local-nodes.sh'
   s.loaded_cue = /Local REP/
   s.cwd = Dir.pwd
   s.pid_file = 'nodes.pid'
end

ServiceManager.define_service 'irc' do |s|
   s.start_cmd = 'cd node; ./local-irc.sh'
   s.loaded_cue = /Local REP/
   s.cwd = Dir.pwd
   s.pid_file = 'irc.pid'
end
