Feature: Hello

	Scenario: Say hello
	   When she connects to localhost as "tracy"
      When she joins "#my+conversation+1"
      Then she says "hello" on "#my+conversation+1" and gets "world"
 