
class IRC

   def wait_for(msg)
      result = ""
      while IO.select([@socket], nil, nil, 2) && (line = @socket.recv(100))
         result += line
         if result.include? msg
            puts "matching " + msg
            return true
         else
#           puts "got " + line
         end
      end
      puts "finished waiting"
      return false
   end

   def wait_for_nothing()
      while IO.select([@socket], nil, nil, 2) && (line = @socket.recv(100))
         return false
      end
      return true
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

   def pass(name)

      write("PASS")
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
