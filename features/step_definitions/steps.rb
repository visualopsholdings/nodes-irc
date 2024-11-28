
lastResult = nil

When('she CONNECTs to localhost as {string}') do |name|

   @irc = IRC.new()
   @irc.connect("localhost", 6667)
   @irc.pass(name)
   
end

When('she JOINs {string}') do |channel|

   @irc.join(channel)

end

Then('she LISTs') do

   @irc.list()
   
end

Then('she PRIVMSGs {string} to {string}') do |message, channel|

   @irc.privmsg(channel, message)

end

When('she receives {string}') do |msg|
   if !@irc.wait_for(msg)
      fail("Got error")
   end
end

Then('she recieves nothing') do
   if !@irc.wait_for_nothing()
      fail("Got message")
   end
end

When("she waits {int} seconds") do |n|

  sleep(n.to_i)
  
end

When('she sends direct message {string} as {string} to {string}') do |text, user, stream|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: stream).first._id.to_s
   j = JSON.generate({ "type": "addobject", "objtype": "idea", "me": u,  "stream": s, "text": text, "corr": "1" })
   lastResult = JSON.parse(`$NODES_HOME/build/Send --logLevel=debug '#{j}'`)
end




