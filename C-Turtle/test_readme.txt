White-box testing: The program will always write to the file 'testrecord.txt' as it does things. This will happen in all of normal, testing and live mode.

Black-box testing: Type 'test' in the command line when executing the program and it will run through the tests that I have written for it.
I have written 20 files (this number is defined in turtle.h), half of them are positive tests and the other half are negative.

Test files 0-9 should output a square with side length of 100 pixels, and the top right corner at the centre of the screen (where the turtle starts).
The turtle should finish with the following conditions, which are checked:

  The turtle's position will be at the centre of the screen
  The turtle's angle will be 0 / facing up
  The turtle's pen will be ON
  The turtle's colour will be 0 0 0 / black

test 0 has only the opening and closing braces
test 1 has FD LT and RT
test 2 has DO loops with numbers
test 3 has SET function and FD LT and RT using VARs
test 4 has DO loops with VARs
test 5 has POLISH expressions 
test 6 has nested DO loops also using each other's VARs and the loop start smaller than the loop end
test 7 has POLISH expressions using VARs
test 8 has the PEN ON and PEN OFF functions
test 9 has the COLOUR function and VARs using themselves
(output square for Test 9 will also have red, blue, yellow and green sides, though the only way to check this is to run the test individually yourself)

Test files 10-19 should fail

test 10 has a missing opening brace
test 11 has a missing closing brace
test 12 has an incorrect instruction
test 13 has an incorrect DO loop
test 14 has an incorrect SET function
test 15 has a missing space between a VAR and the FROM in a DO loop
test 16 has an expression with too many operations
test 17 has an expression with too many VARs and numbers
test 18 has an expression which attempts to divide by 0
test 19 has an incorrect PEN function


If all testing does not pass (if any of 0-9 fail or any of 10-19 pass) then this will be fprinted to stderr and testerrors.txt will contain details of what went wrong.
