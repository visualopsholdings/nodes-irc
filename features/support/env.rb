require 'mongoid'

Mongoid.load!("config/mongoid.yml", :test)
Mongoid.logger.level = Logger::INFO
Mongo::Logger.logger.level = Logger::INFO

Time.zone = 'UTC'
