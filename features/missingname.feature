Feature: Missing name

   Background:
      When there are users:
         | fullname  |
         | Chuck     |

	Scenario: A wont allow login without name
	   When she CONNECTs to localhost as "Chuck"
      Then she receives "401"
 