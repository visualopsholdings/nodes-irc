Feature: Multiple Login

   Background:
      When there is default security

      And there are streams:
         | name                  | policy |
         | My Conversation 1     | p1     |
         | My Conversation 2     | p1     |

	Scenario: More than one user can connect and login
	   When she CONNECTs to localhost as "tracy"
      Then she receives "tracy +w"
	   Then she waits 1 seconds
      Then she JOINs "#my+conversation+1"
      Then she receives ":No topic"
      Then she CONNECTs to localhost as "leanne"
      Then she receives "leanne +w"
	   Then she waits 1 seconds
      Then she JOINs "#my+conversation+1"
      Then she receives ":No topic"
