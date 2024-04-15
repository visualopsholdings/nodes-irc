require "service_manager"

Before do |scenario|
   ServiceManager.stop
   ServiceManager.start
end
