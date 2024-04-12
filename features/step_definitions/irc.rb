
@socket = nil

def wait_for(msg)
   while IO.select([@socket], nil, nil, 2) && (line = @socket.recv(50))
      if line.include? msg
         return
      end
   end
end

When('she connects to localhost as {string}') do |name|

   @socket = TCPSocket.new("localhost", 6667)

   @socket << "USER "
   @socket << name
   @socket << " "
   @socket << name
   @socket << " localhost :"
   @socket << name
   @socket << "\r\n"

   @socket << "NICK "
   @socket << name
   @socket << "\r\n"
   
   wait_for("tracy +w")
end

When('she joins {string}') do |channel|
   @socket << "JOIN "
   @socket << channel
   @socket << "\r\n"
   
   wait_for(":No topic")
end

When('she says {string} on {string} and gets {string}') do |msg, channel, waitfor|
   @socket << "PRIVMSG "
   @socket << channel
   @socket << " :"
   @socket << msg
   @socket << "\r\n" 

   wait_for(waitfor)
end
