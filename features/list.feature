Feature: List

   Background:
      When there is default security

      And there are streams:
         | name                  | policy |
         | My Conversation 1     | p1     |

	Scenario: A server will return a list of conversations
	   When she connects to localhost as "tracy"
      Then she receives "tracy +w"
	   Then she waits 1 seconds
      Then she LISTS
      Then she receives "#my+conversation+1"
      Then she receives ":End of /LIST"