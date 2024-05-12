require 'service_manager'

ServiceManager.define_service 'nodes' do |s|
   s.start_cmd = 'build/ZMQCHAT'
   s.loaded_cue = /Local REP/
   s.cwd = Dir.pwd
   s.pid_file = 'nodes.pid'
end

ServiceManager.define_service 'irc' do |s|
   s.start_cmd = 'build/ZMQIRC'
   s.loaded_cue = /accepting.../
   s.cwd = Dir.pwd
   s.pid_file = 'irc.pid'
end
