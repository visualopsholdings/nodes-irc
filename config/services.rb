require 'service_manager'

# ServiceManager.define_service 'vops' do |s|
#    s.start_cmd = 'cd $VOPS_HOME; MONGO_PORT_27017_TCP_ADDR=127.0.0.1 MONGO_PORT_27017_TCP_PORT=27017 node test-app.js -p 3000 --memcached'
#    s.loaded_cue = /start_tests/
#    s.cwd = Dir.pwd
#    s.pid_file = 'vops.pid'
# end

ServiceManager.define_service 'nodes' do |s|
   s.start_cmd = '$NODES_HOME/build/nodes --test --logLevel=trace'
   s.loaded_cue = /Local REP/
   s.cwd = Dir.pwd
   s.pid_file = 'nodes.pid'
end

ServiceManager.define_service 'irc' do |s|
   s.start_cmd = 'build/nodes-irc --logLevel=trace'
   s.loaded_cue = /accepting.../
   s.cwd = Dir.pwd
   s.pid_file = 'irc.pid'
end
