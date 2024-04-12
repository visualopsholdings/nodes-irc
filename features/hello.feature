Feature: Hello

	Scenario: Say hello
	   When she connects to localhost as "tracy"
      When she joins "#my+conversation"
      Then she says "hello" on "#my+conversation" and gets "world"
 