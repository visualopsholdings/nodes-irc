
lastResult = nil

When('she connects to localhost as {string}') do |name|

   @irc = IRC.new()
   @irc.connect("localhost", 6667)
   @irc.pass(name)
   
end

When('she joins {string}') do |channel|

   @irc.join(channel)

end

Then('she LISTS') do

   @irc.list()
   
end

When('she receives {string}') do |msg|
   if !@irc.wait_for(msg)
      fail("Got error")
   end
end

When("she waits {int} seconds") do |n|

  sleep(n.to_i)
  
end

When('she sends direct message {string} as {string} to {string}') do |text, user, stream|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: stream).first._id.to_s
   lastResult = JSON.parse(`$NODES_HOME/build/Send --logLevel=trace --cmd=message --args="#{u},#{s},#{text}"`)
   expect(lastResult["type"]).to eq("ack")
end




