Feature: List

	Scenario: A server will return a list of conversations
	   When she connects to localhost as "tracy"
      Then she LISTS and gets "#my+conversation"
 