Feature: Multiple Login

	Scenario: More than one user can connect and login
	   When she connects to localhost as "tracy"
      When she joins "#my+conversation"
      When she connects to localhost as "leanne"
      Then she joins "#my+conversation"
