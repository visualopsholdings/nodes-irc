
When('she connects to localhost as {string}') do |name|

   @irc = IRC.new()
   @irc.connect("localhost", 6667)
   @irc.nick(name)   
   @irc.user("localhost", name)
   if !@irc.wait_for(name + " +w")
      fail("Got error")
   end
   
end

When('she joins {string}') do |channel|

   @irc.join(channel)
   if !@irc.wait_for(":No topic")
      fail("Got error")
   end
   
end

When('she says {string} on {string} and gets {string}') do |msg, channel, waitfor|

   @irc.privmsg(channel, msg)
   if !@irc.wait_for(waitfor)
      fail("Got error")
   end
   
end

Then('she LISTS and gets {string}') do |match|

   @irc.list()
   if !@irc.wait_for(match)
      fail("Got error")
   end
   if !@irc.wait_for(":End of /LIST")
      fail("Got error")
   end
   
end

