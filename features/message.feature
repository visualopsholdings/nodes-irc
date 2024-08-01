Feature: Message

   Background:
      When there is default security

      And there are streams:
         | name               | policy |
         | My Conversation 1  | p1     |

	Scenario: send a messsage to a user
	   When she connects to localhost as "tracy"
      Then she receives "tracy +w"
	   And she waits 1 seconds
      And she joins "#my+conversation+1"
	   And she waits 1 seconds
 	   And she sends direct message "xxxxx" as "tracy" to "My Conversation 1"
      And she receives "xxxxx"
 