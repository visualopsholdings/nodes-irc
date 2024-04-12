
class IRC

   def wait_for(msg)
      while IO.select([@socket], nil, nil, 2) && (line = @socket.recv(50))
         if line.include? msg
            puts "matching " + msg
            return true
         else
            if line.start_with? ":localhost 433"
               puts "user already on channel"
               return false
            else
               puts line
            end
         end
      end
      puts "finished waiting"
      return false
   end

   def connect(server, port)
      @socket = TCPSocket.new(server, port)
   end

   def write(text)
      @socket << text
   end

   def endline
      write("\r\n")
   end

   def arg(text)
      write(" ")
      write(text)
   end

   def user(server, name)

      write("USER")
      arg(name)
      arg(name)
      arg(server)
      arg(":" + name)
      endline

   end

   def nick(name)

      write("NICK")
      arg(name)
      endline

   end

   def join(channel)

      write("JOIN")
      arg(channel)
      endline

   end

   def privmsg(channel, msg)

      write("PRIVMSG")
      arg(channel)
      arg(":" + msg)
      endline 

   end

   def list()

      write("LIST")
      endline 

   end
   
end
