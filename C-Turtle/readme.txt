FORMAL GRAMMAR:

<MAIN>          ::=  "{" <INSTRCTLST>

<INSTRCTLST>    ::=  "}" | 
                     <INSTRUCTION><INSTRCTLST>

<INSTRUCTION>   ::=  <FD> |
                     <LT> |
                     <RT> |
                     <DO> |
                     <SET>

<FD>            ::=  "FD" <VARNUM>
<LT>            ::=  "LT" <VARNUM>
<RT>            ::=  "RT" <VARNUM>

<DO>            ::=  "DO" <VAR> "FROM" <VARNUM> "TO" <VARNUM> "{" <INSTRCTLST>

<SET>           ::=  "SET" <VAR> ":=" <POLISH>

<EXPR>          ::=  <OP> <EXPR> |
                     <VARNUM> <EXPR> |
                     ";"

<VARNUM>        ::=  number |
                     <VAR>

<VAR>           ::=  [A-Z]

<OP>            ::=  "+" |
                     "-" |
                     "*" |
                     "/"

<PEN>           ::=  "PEN" "ON" | 
                     "PEN" "OFF" | 
                     "PEN" <VARNUM> <VARNUM> <VARNUM> 

<RESET>         ::=  "RESET"

<CLEAR>         ::=  "CLEAR"

<SPEED>         ::=  "SPEED" "OFF" |
                     "SPEED" <VARNUM>

EXPRs are reverse Polish expressions. 

PEN X Y Z where X, Y and Z are variables or numbers changes the colour values of R G B respectively for drawing with (mod 256).

RESET puts the turtle back to the start; position is set to the middle of the screen, pen is set to on, and angle, pen colours and all variables are set to 0. Speed is also set to 0 (instant).

CLEAR wipes the screen of any drawing. The turtle's values remain as they are.

SPEED OFF means that you will only see what the turtle has done after it has executed the instructions. This is the default.
SPEED X where X is a variable or a number will mean that the screen will update as the turtle moves, and X is the speed at which this happens (setting the SPEED to 0 has the same effect has setting it to be OFF).

While the turtle is running you can increase or decrease the current speed by pressing space or backspace. Pressing return/enter will set the speed to OFF (so the program will then update immediately with the finished output). 



LIVE TURTLE

Typing 'live' in the command line when executing the program will make a textbox appear at the bottom of the window. If you start typing the characters will appear on the screen. 
Type "STOP" to finish and close the program.

I have avoided using SDL_TTF, instead all the characters have been designed in a 3x5 pixel grid, drawn up by me in a Graphics class when I was 14 and coded in their entirety in the file 'pixeldrawing.c', again by me, so I hope it was worth it.
Backspace functionality is included. 
Upon pressing return/enter, what you have typed will be sent to the parser as a list of instructions (the opening and closing curly brackets are put in for you, as indicated by their presence on the screen), then the turtle will do as you have instructed. Instructions can continually be fed to the turtle in this way. 

I am fairly confident that complete functionality is retained in live mode. 
In particular, the RESET, CLEAR and SPEED functions work in both live mode and when written in a file. So will changing the speed with space and backspace, or pressing enter/return to generate immediately, provided that the program has not finished already (so any key press will exit the program).
Variables you set will be retained until the program finishes or you set them again (or type the instruction RESET).

- LIVE LOOPS -
This may seem like a long explanation, but I think it is quite intuitive once you know what's going on.

Loops can be executed either by typing the full loop out, for example:
    DO A FROM 1 TO 10 { FD 5 }
or by typing the initial string and pressing return/enter. At this point the the text in the textbox will turn blue, indicating that the program is ready to accept the instructions you wish to carry out in the loop. 
Similar to normal instructions, there will be no need to type the curly brackets in this case.
For example, type:
    DO A FROM 1 TO 10
Then press return/enter. The brackets will turn blue, then type:
    FD 5
And the turtle will move forward by 5 pixels 10 times. 
Nested loops are also possible by typing the initial string multiple times, for example typing 'DO B FROM 1 TO 10' in between the two lines above will make the turtle move forward by 100 pixels 100 times (loop of 10 in a loop of 10).
You are able to type an instruction for the loop, then initiate a further loop in this way in the same line. 
If you wish to stop writing a loop and you are already within a loop, you can type a closing bracket yourself in the textbox, then continue with further instructions for the previous loop to carry out. For example, type:
    'DO A FROM 1 TO 10', then press return/enter, then type:
    'FD 5 DO B FROM 1 TO 4', then press return/enter, then type:
    'RT 90 FD 5 } LT 90'
The LT 90 will be carried out after the B loop each time the A loop executes, just as you would expect it to.
