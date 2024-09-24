Feature: Message

   Background:
      When there is default security

      And there are streams:
         | name               | policy |
         | My Conversation 1  | p1     |

	Scenario: send a messsage to a user
	   When she CONNECTs to localhost as "tracy"
      Then she receives "tracy +w"
	   And she waits 1 seconds
      And she JOINs "#my+conversation+1"
	   And she waits 1 seconds
 	   And she sends direct message "What's up?" as "leanne" to "My Conversation 1"
      And she receives "What's up?"
      And she PRIVMSGs "Nothing much" to "#my+conversation+1"
      And she recieves nothing
 	   And she sends direct message "That's good" as "leanne" to "My Conversation 1"
      And she receives "That's good"
