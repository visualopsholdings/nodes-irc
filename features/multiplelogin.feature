Feature: Multiple Login

   Background:
      When there is default security

      And there are streams:
         | name                  | policy |
         | My Conversation 1     | p1     |
         | My Conversation 2     | p1     |

	Scenario: More than one user can connect and login
	   When she connects to localhost as "tracy"
	   When she waits 1 seconds
      When she joins "#my+conversation+1"
      When she connects to localhost as "leanne"
	   When she waits 1 seconds
      Then she joins "#my+conversation+1"
