#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL.h>

#define  PROGLEN  500  // The maximum words the program will accept from a file
#define  TEXTLEN  40 // The maximum characters the text entry point will accept
#define  WORDLEN  10 // The maximum word length
#define  TESTMAX  20 // The number of test files I have accounted for.
// The first half are corrrect while the second half are incorrect
#define  STACKSIZE  100 // The maximum numbers an expression will accept
#define  WINSIZE  700 // Height and width of the window
#define  ERRORDELAY  1500 // The length of time the live error message displays

#ifndef  M_PI
  #define M_PI  3.14159265358979323846
  #endif  
#define  RADIANS(D) {((double)(D/180.0)) * M_PI} // Convert degrees to radians

#define  ALPHA  26 // The number of letters in the alphabet
#define  RGB  3 // The number of colours needed for hexadecimal representation

typedef struct SDLwin {
  SDL_bool exit;
  SDL_Window *win;
  SDL_Renderer *rend;
} SDLwin;

typedef struct stack {
  int exprnums[STACKSIZE];
  int top;
} Stack; 
// Used solely for expression interpreting purposes.

typedef struct turtle {
  int xpos;
  int ypos;
  int angle;
  int pen;
  int colour[RGB];
  int speed;
} Turtle;
// The current state of the turtle.

typedef struct test {
  FILE *record;
  FILE *errors;
} Test;
// Test files

typedef enum mode { normal_mode, testing_mode, live_mode, mid_loop } Mode;
// We use this to know what to reset in reset_program().

typedef enum action { nothing, enter, space, back, quit } Action;
// We use this to know what to do in live() and live_loop().

typedef enum colour { black, red, blue } Colour;
// We use this for drawing the text in the textbox in draw_text().
// We also use it as a flag that there has already been an error in ERROR().

typedef struct program {
  Mode mode; 
  Turtle turt;
  int cw;
  SDLwin sw;
  Stack stack;
  Test test;
  int vars[ALPHA];
  char text[TEXTLEN];
  Colour textcolour;
  char strings[PROGLEN][WORDLEN];
} Program;
// The primary structure that is passed to most functions.

int use_file(Program *prog, char* filename);
// Opens and uses the file given to write to the strings array
int run_turtle(Program *prog);
// Runs the program as normal; reading in the given file and executing it
void finish_turtle(Program *prog, int passed);
// Updates, waits using SDLwait(), then closes the files and quits SDL

/* PARSING */
int parse(Program *prog); // Checks for '{' then continues with instructlist()
int instructlist(Program *prog); // Checks for '}' or calls instruction()
int instruction(Program *prog); // Calls all instr_ functions

/* These functions all return 1 or 0 depending on 
   whether their instruction has been correctly given. */
/* They also increase the prog->cw when they have correctly read a word.
   This means that the interpreting functions must change the prog->cw
   as they need to once control is passed to them. */
int instr_fd(Program *prog);    // "FD"<VARNUM>
int instr_turn(Program *prog);  // ("LT"|"RT)<VARNUM>
int instr_set(Program *prog);   // "SET"<VAR>":="<EXPR> 
int instr_do(Program *prog);    // "DO"<VAR>"FROM"<VARNUM>"TO"<VARNUM>"{"
int instr_pen(Program *prog);   // "PEN"(<VARNUM><VARNUM><VARNUM>|"ON"|"OFF")
int instr_reset(Program *prog); // "RESET"
int instr_clear(Program *prog); // "CLEAR"
int instr_speed(Program *prog); // "SPEED"("OFF"|<VARNUM>)
int varnum(Program *prog);      // <VAR>|number
int var(Program *prog);         // "A"-"Z"
int op(Program *prog);          // "+"|"-"|"*"|"/"
int expr(Program *prog);        // (<VARNUM>|<OP>)<EXPR>

/* INTERPRETING */

/* As noted above, the prog->cw must be changed as needed in these functions.
   If everything has parsed, we can assume that the strings are as expected. */
void interpret_fd(Program *prog);
void interpret_turn(Program *prog, char direction);
void interpret_set(Program *prog);
int interpret_do(Program *prog); // Apart from with loops; they are special live
void make_loop_vars(Program *prog, int *start, int *end, char *var);
int execute_loop(Program *prog, int start, int current, int end, 
                    char varname, int pos);
void interpret_pen(Program *prog);
void reset_turtle(Program *prog);
void interpret_speed(Program *prog);
int interpret_varnum(Program *prog, char varnum[WORDLEN]); 
// Returns the value of the given number or variable
int interpret_expr(Program *prog); 
// Returns the value of the expression

/* STACK */
int pop(Stack *stack);
void push(Stack *stack, int n);

/* OTHER */
int ERROR(Program *prog, char* error); 
// Called if there has been a parsing error. Prints the error and returns 0
void reset_program(Program *prog);
// Resets values depending on what mode the program is in

/* TESTING */
void testing(Program *prog);
int test_correct(Program *prog); // Tests the files that should output correctly
int test_incorrect(Program *prog); // Tests the files with errors in the code
int create_test_file(Program *prog, int i); // Makes the test file name
int check_test(Program *prog, int testnum); 
// Checks that the correct tests have finished as expected

/* LIVE INSTRUCTIONS */
void live(Program *prog);
Action receive_input(Program *prog, int *c);
Action keydown(Program *prog, char key, int *c);
// Takes appropriate action depending on which key was pressed
void draw_text(Program *prog);
// Gives control to letters.c; displays the textbox and displays the text
int live_parse(Program *prog);
// Writes the textbox to the normal array of strings with a couple of exceptions
int live_checks(Program *prog); 
// Checks to see if "STOP", "CLEAR" or nothing at all has been entered.
void read_textbox(Program *prog);
int live_loop(Program *prog);
// Parses and gets ready to receive a loop to execute with the given conditions
void insert_brackets(Program *prog);

/* SDL BACKEND */
void clear_screen(SDLwin *sw);
void update_screen(SDLwin *sw);
void SDLwait(SDLwin *sw);
// Called when output has finished displaying; waits for input to quit.
void check_events(Program *prog); // Called between instructions
void SDLsetup(SDLwin *sw);
void SDLERROR(char* error);
