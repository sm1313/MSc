#include "turtle.h"

int main(int argc, char** argv) 
{
  Program prog;
  prog.mode = normal_mode; // For now, at least.
  int passed = 0;

  // The two reset functions act as initialising functions for everything.
  reset_program(&prog);
  reset_turtle(&prog);

  if(argc < 2) { // No argument has been given.
    return ERROR(&prog, "Please enter an argument or file to read from.");
  } 
 
  SDLsetup(&prog.sw);

  if(strcmp(argv[1], "test")==0) {
    prog.mode = testing_mode;
    testing(&prog);
  }

  else if(strcmp(argv[1], "live")==0) {
    prog.mode = live_mode;
    live(&prog);
  }

  else if(use_file(&prog, argv[1])) {
    passed = run_turtle(&prog);
  }

  // if: passing failed || we tested || we were live || the file was not valid
  // then the value of 'passed' that we pass to finish_turtle() will be 0 
  finish_turtle(&prog, passed);
  return 0;
}

int use_file(Program *prog, char* filename)
{
  // Read in strings from the (potentially) given file, write them to the array
  int cw = 0; // Local so we don't have to worry about changing it 
  FILE *file;
  if((file = fopen(filename, "r"))==NULL) {
    fprintf(stderr, "\n'%s' is not a valid argument or file.\n", filename);
    return 0;
  }
  while(cw < PROGLEN && fscanf(file, "%s", prog->strings[cw])!=0) {
    fprintf(prog->test.record, "%s ", prog->strings[cw]);
    cw += 1;
  }
  fclose(file);

  for(int i = 0; i < PROGLEN; i++) {
    prog->strings[i][WORDLEN-1] = '\0';
  }

  return 1;
}

int run_turtle(Program *prog) 
{
  prog->cw = 0;

  if(prog->strings[PROGLEN-1][0]!='\0') { // We may have missed some strings.
    return ERROR(prog, "Program is too long. Maybe use more loops?");
  }

  // Just for the record: 
    fprintf(prog->test.record, "Given file read in as follows:\n");
    while(prog->strings[prog->cw][0]!='\0') {
      fprintf(prog->test.record, "%s ", prog->strings[prog->cw++]);
    }
    prog->cw = 0;
  // 

  fprintf(prog->test.record, "\nBegin parsing.\n\n");
  if(parse(prog)) {
    fprintf(prog->test.record, "\nThe file makes sense!\n\n"); 
    return 1;
  }
  else {
    return 0;
  }
}

void finish_turtle(Program *prog, int passed) 
{
  fprintf(stderr, "Finished.\n");
  // If the value of 'passed' is equal to zero (see main() for details)
  // the window is not updated and we do not wait for further input before quit
  if(passed) {
    update_screen(&prog->sw);
    SDLwait(&prog->sw); 
  }

  SDL_Quit();
  fclose(prog->test.record);
  fclose(prog->test.errors);
}

/* LANGUAGE AND INTERPRETATIONS */

int parse(Program *prog)
{
  if(strcmp(prog->strings[prog->cw], "{")==0) {
    fprintf(prog->test.record,  "{\n");
    prog->cw += 1;
    return instructlist(prog);
  }

  return ERROR(prog, "Program must begin with a '{'");
}

int instructlist(Program *prog)
{
  check_events(prog);

  if(strcmp(prog->strings[prog->cw], "}")==0) {
    fprintf(prog->test.record,  "}\n");
    prog->cw += 1;
    return 1;
  }

  if(instruction(prog)) {
    return instructlist(prog);
  }
  return 0;
}

int instruction(Program *prog)
{
  fprintf(prog->test.record, "string %s\n", prog->strings[prog->cw]);
  if(instr_fd(prog) || instr_turn(prog) || instr_set(prog) || instr_do(prog) 
       || instr_pen(prog) || instr_reset(prog) || instr_clear(prog) 
         || instr_speed(prog)) {
    return 1;
  }
  // If an instruction was not given at all, presume that '}' was missed
  if(prog->strings[prog->cw][0]=='\0') {
    return ERROR(prog, "Program must end with a '}'");
  }
  // If there is something in the string presume it was an attempted instruction
  else {
    char error[TEXTLEN];
    strcpy(error, "Invalid instruction: ");
    strcat(error, prog->strings[prog->cw]);
    return ERROR(prog, error);
  }
}

int instr_fd(Program *prog)
{
  if(strcmp(prog->strings[prog->cw], "FD")!=0) {
    return 0;
  }
  fprintf(prog->test.record,  "FD\n");
  prog->cw += 1;

  varnum(prog);

  interpret_fd(prog);

  return 1;
}

int instr_turn(Program *prog) 
{
  char direction;
  if(strcmp(prog->strings[prog->cw], "LT")==0) {
    direction = 'l';
  }
  else if(strcmp(prog->strings[prog->cw], "RT")==0) {
    direction = 'r';
  }
  else {
    return 0;
  }
  fprintf(prog->test.record, "%cT\n", toupper(direction));
  prog->cw += 1;

  varnum(prog);

  interpret_turn(prog, direction);

  return 1;    
}

int instr_pen(Program *prog)
{
  if(strcmp(prog->strings[prog->cw], "PEN")!=0) {
    return 0;
  }
  fprintf(prog->test.record, "PEN\n");
  prog->cw += 1;
  
  if(strcmp(prog->strings[prog->cw], "ON")==0) {
    fprintf(prog->test.record, "ON\n");
    prog->cw += 1;
  }
  else if(strcmp(prog->strings[prog->cw], "OFF")==0) {
    fprintf(prog->test.record, "OFF\n");
    prog->cw += 1;
  }
  else {
    for(int i=0; i<RGB; i++) {
      if(!varnum(prog)) {
        return 0;
      }
    }
  }

  interpret_pen(prog);

  return 1;
}

int instr_do(Program *prog)
{
  if(strcmp(prog->strings[prog->cw], "DO")!=0) {
    return 0;
  }
  fprintf(prog->test.record,  "LOOP |DO|\n");
  prog->cw += 1;

  if(!var(prog)) {
    return ERROR(prog, "Expecting a variable after 'DO'.");
  }

  if(strcmp(prog->strings[prog->cw], "FROM")!=0) {
    return ERROR(prog, "Expecting 'FROM' after 'DO'.");
  }
  fprintf(prog->test.record,  "LOOP |FROM|\n");
  prog->cw += 1;

  if(!varnum(prog)) {
    return ERROR(prog, "Expecting a var or num after 'FROM'.");
  }

  if(strcmp(prog->strings[prog->cw], "TO")!=0) {
    return ERROR(prog, "Expecting 'TO' after 'FROM'.");
  }
  fprintf(prog->test.record,  "LOOP |TO|\n");
  prog->cw += 1;

  if(!varnum(prog)) {
    return ERROR(prog, "Expecting a var or num after 'TO'.");
  }

  if(prog->mode != live_mode && prog->mode != mid_loop) {
    if(strcmp(prog->strings[prog->cw], "{")!=0) {
      return ERROR(prog, "Expecting '{' after 'TO'.");
    }
    fprintf(prog->test.record,  "LOOP |{|\n");
    prog->cw += 1;
  }

  // Either way, the prog->cw is now on the first instruction after '{'
  return interpret_do(prog); 
}

int instr_set(Program *prog)
{
  if(strcmp(prog->strings[prog->cw], "SET")!=0) {
    return 0;
  }
  fprintf(prog->test.record,  "SET\n");
  prog->cw += 1;

  if(!var(prog)) {
    return ERROR(prog, "Expecting a variable after 'SET'.");
  }

  if(strcmp(prog->strings[prog->cw], ":=")!=0) {
    return ERROR(prog, "Expecting ':=' after 'SET'.");
  }
  fprintf(prog->test.record, "SET |:=|\n");
  prog->cw += 1;

  if(!expr(prog)) {
    return ERROR(prog, "Expecting expression after ':='.");
  }

  interpret_set(prog);

   return 1;
}

int instr_reset(Program *prog) 
{
  if(strcmp(prog->strings[prog->cw], "RESET")!=0) {
    return 0;
  }
  fprintf(prog->test.record, "RESET.\n");
  prog->cw += 1;

  reset_turtle(prog);

  return 1;
}

int instr_clear(Program *prog)
{
  if(strcmp(prog->text, "CLEAR")!=0) {
    return 0;
  }
  fprintf(prog->test.record, "CLEAR.\n");
  prog->cw += 1;

  clear_screen(&prog->sw);

  return 1;
}

int instr_speed(Program *prog)
{
  if(strcmp(prog->strings[prog->cw], "SPEED")!=0) {
    return 0;
  }
  fprintf(prog->test.record, "SPEED: \n");
  prog->cw += 1;

  if(strcmp(prog->strings[prog->cw], "OFF")==0) {
    fprintf(prog->test.record, "OFF.\n");
    prog->cw += 1;
  }
  else {
    if(!varnum(prog)) {
      return 0;
    }
  }

  interpret_speed(prog);

  return 1;
}

int varnum(Program *prog)
{
  int num;

  if(sscanf(prog->strings[prog->cw], "%d", &num)!=0) {
    fprintf(prog->test.record, "VARNUM: num %d\n", num);
    prog->cw += 1;
    return 1;
  }

  else if(var(prog)) { // var() will increase prog->cw if it reads a variable
    return 1;
  }

  return 0;
}

int var(Program *prog)
{
  if(prog->strings[prog->cw][1]!='\0') { // The string is longer than one char
    return 0;
  }

  char varname = prog->strings[prog->cw][0];

  if(varname >= 'A' && varname <= 'Z') {
    fprintf(prog->test.record, "VAR: %c\n", varname);
    prog->cw += 1; 

    return 1;
  }

  return 0;
}

int op(Program *prog)
{
  if(strcmp(prog->strings[prog->cw], "+")==0
      || strcmp(prog->strings[prog->cw], "-")==0 
      || strcmp(prog->strings[prog->cw], "*")==0 
      || strcmp(prog->strings[prog->cw], "/")==0) {
    fprintf(prog->test.record, "OP: %s\n", prog->strings[prog->cw]);
    prog->cw += 1;

    return 1;
  }

  return 0;
}

int expr(Program *prog)
{
  fprintf(prog->test.record, "EXPR\n");
  int count = 0; // This will be used to count the number of var/nums and ops.

  // A valid expression will have one more var/num than op, and as we go,
  // at any point it will always have more var/nums than ops.
  // Hence, we add one if we read a var/num, and take one if we read an op, 
  // all the while making sure that the count does not drop below 1.

  do {
    if(varnum(prog)) {
      count += 1;
    }
    else if(op(prog)) {
      count -= 1;
    }
    else {
      return ERROR(prog, "Expression invalid.");
    }
  } while(strcmp(prog->strings[prog->cw], ";")!=0 && count > 0);

  // A valid expresion will end with count equalling 1. If it is not, 
  // we have either read a ';', or we have read more operators than var/nums.
 
  if(count > 1) { // We have read a ';' and we have var/nums left over.
    return ERROR(prog, "Expression invalid; too many vars/nums.");
  }
  if(count < 1) { // We have read too many ops and have run out of var/nums
    return ERROR(prog, "Expression invalid; too many ops.");
  } 

  // If count is 1, then it is not > 0, 
  // hence from the while loop we have read a ';' and the expression is valid.
  fprintf(prog->test.record,  "|;|");
  prog->cw += 1;

  return 1;
}

/* INTERPRETING */

void interpret_fd(Program *prog)
{
  int dist = interpret_varnum(prog, prog->strings[prog->cw-1]);
  double rads = RADIANS(prog->turt.angle); // The angle is stored in degrees
  int newx = prog->turt.xpos + (int)(dist*sin(rads)); // Where the turtle will 
  int newy = prog->turt.ypos + (int)(-dist*cos(rads)); // end up afterwards

  if(prog->turt.pen) {
    SDL_SetRenderDrawColor(prog->sw.rend, 
               prog->turt.colour[0], prog->turt.colour[1], prog->turt.colour[2], 
                 SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(prog->sw.rend, 
                         prog->turt.xpos, prog->turt.ypos, newx, newy);
  }

  // Just for the record: 
    fprintf(prog->test.record, "FD from %d, %d to %d, %d ; ", 
              prog->turt.xpos, prog->turt.ypos, newx, newy);
    if(prog->turt.pen) {
      fprintf(prog->test.record, "pen was ON with colour %d %d %d.\n", 
            prog->turt.colour[0], prog->turt.colour[1], prog->turt.colour[2]);
    }
    else {
      fprintf(prog->test.record, "pen was OFF.\n");
    }
  // 

  // Move the turtle to its new position
  prog->turt.xpos = newx; 
  prog->turt.ypos = newy;

  if(prog->turt.speed != 0) {
    SDL_Delay(1000/prog->turt.speed);
    update_screen(&prog->sw);
  }
}

void interpret_turn(Program *prog, char direction)
{
  int deg_turn = interpret_varnum(prog, prog->strings[prog->cw-1]);

  switch(direction) {

    case 'l': 
      prog->turt.angle -= deg_turn;
      break;

    case 'r': 
      prog->turt.angle += deg_turn;
      break;

  }

  prog->turt.angle = prog->turt.angle % 360;
  fprintf(prog->test.record, "%cT: ", toupper(direction));
  fprintf(prog->test.record, "Angle changed to %d.\n", prog->turt.angle);
}

void interpret_set(Program *prog)
{
  char varname; // The variable we will set
  int value;    // The value we will set the variable to

  do {
    prog->cw -= 1; // In case another SET follows directly
  } while(strcmp(prog->strings[prog->cw], "SET")!=0);

  prog->cw += 1; // Now we are on the VAR to be set
  varname = prog->strings[prog->cw][0];

  prog->cw += 2; // Now we are at the beginning of the expression
  value = interpret_expr(prog);

  prog->vars[varname - 'A'] = value;

  fprintf(prog->test.record, "SET var %c to \n", varname);
}

int interpret_do(Program *prog)
{
  if(prog->mode == live_mode || prog->mode == mid_loop) {
    if(live_loop(prog)) {
      return 1;
    }
  }
  fprintf(prog->test.record, "Not a live loop.\n");
  char varname; // The variable the loop is looping through
  int loopstart, loopend; // The start and end points of the loop
  // These are needed by both of the below functions, hence being declared here

  make_loop_vars(prog, &loopstart, &loopend, &varname);

  int pos = prog->cw; 
  if(loopstart <= loopend) {
    for(int loop = loopstart; loop <= loopend; loop++) {
      if(!execute_loop(prog, loopstart, loop, loopend, varname, pos)) {
        return 0;
      }
    }
  }
  else {
    for(int loop = loopstart; loop >= loopend; loop--) {
      if(!execute_loop(prog, loopend, loop, loopstart, varname, pos)) {
        return 0;
      }
    }
  }
  return 1;
}

void make_loop_vars(Program *prog, int *start, int *end, char *var)
{
  do {
    prog->cw -= 1;
  } while(strcmp(prog->strings[prog->cw], "DO")!=0);

  prog->cw += 1; // Now we are on the variable
  *var = prog->strings[prog->cw][0];

  prog->cw += 2; // Now we are on the loop's starting point
  *start = interpret_varnum(prog, prog->strings[prog->cw]);

  prog->cw += 2; // Now we are on the loop's ending point
  *end = interpret_varnum(prog, prog->strings[prog->cw]);

  prog->cw += 1; // Now we are on the opening bracket of the loop

  fprintf(prog->test.record, "LOOP %d to %d for %c\n", *start, *end, *var);
}

int execute_loop(Program *prog, int start, int current, int end, 
                    char varname, int pos) 
{
  prog->vars[varname - 'A'] = current;
  fprintf(prog->test.record, "LOOP: %c is on %d\n", varname, current);

  prog->cw = pos;

  if(prog->mode == mid_loop) { // This is used by live_mode
    fprintf(prog->test.record, "We are parsing a LIVE loop.\n");
    return live_parse(prog);
  }
  else {
    return parse(prog);
  }
}

void interpret_pen(Program *prog)
{
  do {
    prog->cw -= 1;
  } while(strcmp(prog->strings[prog->cw], "PEN")!=0);

  prog->cw += 1; // Now we are on the first string after PEN
  if(strcmp(prog->strings[prog->cw], "ON")==0) {
    fprintf(prog->test.record, "PEN has been set ON.\n");
    prog->turt.pen = 1;
    prog->cw += 1;
  }
  else if(strcmp(prog->strings[prog->cw], "OFF")==0) {
    fprintf(prog->test.record, "PEN has been set OFF.\n");
    prog->turt.pen = 0; 
    prog->cw += 1;
  }
  else {
    for(int i=0; i<RGB; i++) {
      prog->turt.colour[i] = 
                           interpret_varnum(prog, prog->strings[prog->cw])%256;
      prog->cw += 1;
    }
  fprintf(prog->test.record, "PEN has been set to %d %d %d\n", 
            prog->turt.colour[0], prog->turt.colour[1], prog->turt.colour[2]);
  }
}

void reset_turtle(Program *prog)
{
  prog->turt.xpos = prog->turt.ypos = WINSIZE/2;
  prog->turt.angle = 0;
  prog->turt.pen = 1;
  prog->turt.speed = 0;

  for(int i=0; i<RGB; i++) {
    prog->turt.colour[i] = 0; // Sets the pen colour to black
  }

  for(int i=0; i<ALPHA; i++) {
    prog->vars[i] = 0; // Sets all variables to zero
  }
}

void interpret_speed(Program *prog)
{
  if(strcmp(prog->strings[prog->cw-1], "OFF")==0) {
    prog->turt.speed = 0;
    fprintf(prog->test.record, "SPEED SET OFF.\n");
    return;
  }
  else {
    prog->turt.speed = interpret_varnum(prog, prog->strings[prog->cw-1]);
    fprintf(prog->test.record, "SPEED SET.\n");
  }
}
int interpret_varnum(Program *prog, char varnum[WORDLEN])
{
  fprintf(prog->test.record, "INTERPRETING VARNUM %s\n", varnum);

  if(varnum[0] - 'A' < ALPHA && varnum[0] - 'A' >= 0) {
    fprintf(prog->test.record, "returning var: %c ", varnum[0]);
    fprintf(prog->test.record, "with value %d\n", prog->vars[varnum[0]-'A']);
    return prog->vars[varnum[0] - 'A'];
  }
  else {
    int num;
    if(sscanf(varnum, "%d", &num)==0) { 
      // The var/num was checked before interpreting
      fprintf(stderr, "SOMETHING HAS GONE HORRIBLY WRONG WITH MY EXPRESSIONS.");
    }
    fprintf(prog->test.record, " returning num: %d\n", num);
    return num;
  }
}

int interpret_expr(Program *prog) 
{
  int result, tmpcalc, tmpnum; // tmp numbers are needed for order of operation
  prog->stack.top = 0;

  while(strcmp(prog->strings[prog->cw], ";")!=0) {
    if(op(prog)) {
      prog->cw -= 1; // op() advances the cw; we just use op() for convenience
      tmpnum = pop(&prog->stack); 
      if(strcmp(prog->strings[prog->cw], "+")==0) {
        tmpcalc = pop(&prog->stack) + tmpnum;
        push(&prog->stack, tmpcalc);
      }
      else if(strcmp(prog->strings[prog->cw], "-")==0) {
        tmpcalc = pop(&prog->stack) - tmpnum;
        push(&prog->stack, tmpcalc);
      }
      else if(strcmp(prog->strings[prog->cw], "*")==0) {
        tmpcalc = pop(&prog->stack) * tmpnum;
        push(&prog->stack, tmpcalc);
      }
      else if(strcmp(prog->strings[prog->cw], "/")==0) {
        if(tmpnum == 0) {
          return ERROR(prog, "Attempting to divide by zero.");
          // Returns 0, but will then also get 'invalid instruction' message.
        }
        tmpcalc = pop(&prog->stack) / tmpnum;
        push(&prog->stack, tmpcalc);
      }
    }
    else {
      push(&prog->stack, interpret_varnum(prog, prog->strings[prog->cw]));
    }
  prog->cw += 1;
  }

  result = pop(&prog->stack);

  prog->cw += 1; // We came out after reading ';', so now we are back after it

  if(prog->stack.top != 0) { // The expression was checked before interpreted.
    fprintf(stderr, "SOMETHING HAS GONE HORRIBLY WRONG WITH MY EXPRESSIONS.");
    exit(1);
  }

  fprintf(prog->test.record, "EXPR returning %d\n", result);
  return result;
}

/* STACK */

void push(Stack *stack, int n)
{
  if(stack->top >= STACKSIZE) {
    fprintf(stderr, "Expression too large.");
    return;
  }

  stack->exprnums[stack->top] = n;
  stack->top += 1;
}

int pop(Stack *stack)
{
  if(stack->top <= 0) { // The expression was checked before interpreted.
    fprintf(stderr, "SOMETHING HAS GONE HORRIBLY WRONG WITH MY EXPRESSIONS.");
    exit(1);
  }
  stack->top -= 1;
  return stack->exprnums[stack->top];
}

/* OTHER */

int ERROR(Program *prog, char* error)
{
  // If we are testing the errors should not be visible outside of the files
  if(prog->mode != testing_mode) {
    fprintf(stderr, "%s\n", error);
  }
  // We should keep a record of them regardless
  fprintf(prog->test.record, "%s\n", error);
  
  // The following is for the textbox displaying the error.
  if(prog->textcolour != red) { // If there has not already been an error
    fprintf(prog->test.record, "new error.\n");
    int c = 0; 
    for(int i=0; i<TEXTLEN; i++) {
      prog->text[i] = '\0';
    }
    while(c < TEXTLEN && error[c] != '\0') {
      prog->text[c] = error[c];
      c++;
    }
  }
  else {
    fprintf(prog->test.record, "already red.\n");
  }
  prog->textcolour = red;

  return 0;
}

void reset_program(Program *prog)
{
  // When testing and instructing live, we read things many times, 
  // but keep writing to the files, hence have not closed them.
  if(prog->mode == normal_mode) { 
    prog->test.record = fopen("testrecord.txt", "w");
    prog->test.errors = fopen("testerrors.txt", "w");
  }

  // If we are in a live loop we do not want to reset these.
  if(prog->mode != mid_loop) {
    prog->cw = 0;

    for(int i=0; i<PROGLEN; i++) {
      for(int j=0; j<WORDLEN; j++) {
        prog->strings[i][j] = '\0';
      }
    }
  }

  // Regardless, we can reset the textbox and the textbox colour
  for(int i=0; i<TEXTLEN; i++) {
    prog->text[i] = '\0';
  }
  prog->textcolour = black;
}

/* TESTING */

void testing(Program *prog) 
{
  if(!test_correct(prog) || !test_incorrect(prog)) {
    fprintf(stderr, "Test failed; check 'testerrors.txt' for details.\n\n");
  }
  else {
    fprintf(stderr, "All tests passed! \n\n");
  }
}

int test_correct(Program *prog) 
{
  int pass = 1;
  // The following tests should all pass.
  for(int i=0; i<TESTMAX/2; i++) {
    reset_program(prog);
    if(create_test_file(prog, i)) { // If test exists, run it.
      if(!run_turtle(prog) || !check_test(prog, i)) { 
          // If any of them fail, this is wrong.
          fprintf(prog->test.record, "Test %d failed, incorrectly.\n", i);
          pass = 0;
      }
    }
  }
  return pass;
}

int test_incorrect(Program *prog) 
{
  int pass = 1;
  // The following tests should all fail.
  for(int i=TESTMAX/2; i<TESTMAX; i++) {
    reset_program(prog);
    if(create_test_file(prog, i)) { // If test exists, run it.
      if(run_turtle(prog)) { 
          // If any of them pass, this is wrong.
          fprintf(prog->test.record, "Test %d passed, incorrectly.\n", i);
          pass = 0;
      }
    }
  }
  return pass;
}

int create_test_file(Program *prog, int i)
{
  int length = strlen("test") + strlen("xx.txt") + 1; // +1 for \0
  char testname[length];
  char num[strlen("xx")+1];

  sprintf(num, "%d", i);
  strcpy(testname, "test"); 
  strcat(testname, num);
  strcat(testname, ".txt");
  if(!use_file(prog, testname)) {
    return 0;
  }
  return 1;  
}

int check_test(Program *prog, int testnum) 
{
  int passed = 1;
  if(prog->turt.xpos != WINSIZE/2 || prog->turt.ypos != WINSIZE/2) {
    fprintf(prog->test.errors, "Turtle position not at centre of screen.\n");
    fprintf(prog->test.errors, "Turtle position is (%d, %d).\n", 
              prog->turt.xpos, prog->turt.ypos);
    passed = 0;
  }
  if(prog->turt.angle%360 != 0) {
    fprintf(prog->test.errors, "Turtle angle is not facing up.\n");
    fprintf(prog->test.errors, "Turtle angle is %d.\n", prog->turt.angle);
    passed = 0;
  }
  if(prog->turt.pen != 1) {
    fprintf(prog->test.errors, "Turtle pen is not ON.\n");
    passed = 0;
  }
  if(prog->turt.colour[0] != 0 || prog->turt.colour[1] != 0 
       || prog->turt.colour[2] != 0) {
    fprintf(prog->test.errors, "Turtle pen colour is not black.\n");
    fprintf(prog->test.errors, "Turtle pen colour is %d %d %d.\n", 
              prog->turt.colour[0], prog->turt.colour[1], prog->turt.colour[2]);
  }

  if(!passed) {
    fprintf(prog->test.errors, "\nTest %d failed.\n\n", testnum);
    return 0;
  }
  return 1;
}

/* LIVE INSTRUCTIONS */

void live(Program *prog)
{
  int c = 0; // Used as a counter when writing to the textbox
  draw_text(prog); // Will just draw the textbox and brackets
  while(!prog->sw.exit) {
    switch(receive_input(prog, &c)) {

      case enter :
        read_textbox(prog);
        if(live_parse(prog)) {
          reset_program(prog);
          draw_text(prog);
        }
        c = 0; 
        break;

      case quit :
        prog->sw.exit = 1;
        break;

      default :
        break;

    }
  }
}

Action receive_input(Program *prog, int *c)
{
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {

      case SDL_TEXTINPUT : 
        if(event.text.text[0] == ' ' && *c == 0) {
            break;
        }
        if(*c < TEXTLEN) { // We do not want to write over the end of the box
          prog->text[*c] = toupper(event.text.text[0]);
          if(event.text.text[0] == '}') { // User is terminating a loop
            prog->textcolour = black;
          }
          *c += 1; 
          draw_text(prog); // Update the textbox on the screen
        }
        break;

      case SDL_KEYDOWN :
        return keydown(prog, event.key.keysym.sym, c); // Decides what to do

      case SDL_QUIT : 
        return quit;

    }
  }
  return nothing;
}

Action keydown(Program *prog, char key, int *c)
{
  switch(key) {

    case SDLK_RETURN : // User wishes to execute what they have typed
        return enter;

    case SDLK_BACKSPACE : // Overwrite previous char with null character
      if(c != NULL) {
        if(*c != 0) {
          *c -= 1;
          prog->text[*c] = '\0'; 
        }
        draw_text(prog); // Update the textbox
      }
      return back;

    case SDLK_SPACE :
      return space;

  }
  return nothing;
}

int live_parse(Program *prog) 
{
  if(live_checks(prog)) {
    return 0;
  }

  int current = prog->cw; // A record of the current opening bracket

  prog->cw = current; // On the opening bracket
  fprintf(prog->test.record, "Parsing instruction.\n");
  if(!parse(prog)) {
    draw_text(prog);
    SDL_Delay(ERRORDELAY);
    reset_program(prog);
    return 0;    
  }

  return 1;
}

int live_checks(Program *prog)
{
  if(prog->text[0] == '\0') { // There is nothing in the textbox to execute
    return 1;
  }

  if(strcmp(prog->text, "STOP")==0) { // The user wants to exit the program
    prog->sw.exit = 1;
    return 1; 
    // The switch statement in live() will stop and the program will finish
  }

  return 0;
}

void read_textbox(Program *prog)
{
  int current = prog->cw; // A record of the point we should be parsing from
  int count = 0, c = 0;
  prog->strings[prog->cw][0] = '{'; // Write this in for the parser
  fprintf(prog->test.record, "Inserting {\n");
  prog->cw += 1;
  while(count < TEXTLEN && prog->text[count]!='\0') {
    if(prog->text[count]==' ') { // New word
      if(c!=0) { // Otherwise the next word would be a space and make no sense
        prog->cw += 1;
        c = 0;
      }
    }
    else {
      if(c<WORDLEN) { // Since no instruction is this long, we cut it at this 
                      // point for our array, because it is already invalid
        prog->strings[prog->cw][c] = prog->text[count];
        c += 1;
      }
    }
    count += 1;
  }

  // If the user has typed a space at the end of their text, we will have moved
  // on to the next word in strings[]. If they have not, we will still be on
  // their word, and we want to be on the next one, which will be empty.
  if(prog->strings[prog->cw][0] != '\0') { 
    prog->cw += 1;
  }
  // Then put closing bracket there, for the parser.
  prog->strings[prog->cw][0] = '}'; 
  fprintf(prog->test.record, "Inserting }\n");

  // Also put in null characters at the end of each string so we don't overrun
  for(int i = 0; i < PROGLEN; i++) {
    prog->strings[i][WORDLEN-1] = '\0';
  }

  // Just for the record: 
    prog->cw = 0;
    for(int i=0; i<TEXTLEN; i++) {
      fprintf(prog->test.record, "%s ", prog->strings[prog->cw++]);
    }
    fprintf(prog->test.record, "\n\n");
  //

  prog->cw = current; // On the opening bracket we put in
}

int live_loop(Program *prog)
{
  fprintf(prog->test.record, "Live loop check.\n");
  char varname; // The variable the loop is looping through
  int loopstart, loopend; // The start and end points of the loop
  Action action;
  int c = 0;
  make_loop_vars(prog, &loopstart, &loopend, &varname);

  // This check is for if the user has tried to type a whole loop in one line
  // If so, we return out of this function and parse normally
  fprintf(prog->test.record, "c: %s, +1: %s\n", prog->strings[prog->cw], prog->strings[prog->cw+1]); 
  if(prog->strings[prog->cw][0] == '{' || prog->strings[prog->cw+1][0] != '\0') { 
    return 0;
  }
  fprintf(prog->test.record, "Live loop confirmed.\n");

  prog->mode = mid_loop;
  reset_program(prog);
  prog->textcolour = blue;
  draw_text(prog);
  do {
    action = receive_input(prog, &c);
  } while(action != enter);

  read_textbox(prog);

  insert_brackets(prog);

  int pos = prog->cw;
  if(loopstart <= loopend) {
    for(int loop = loopstart; loop <= loopend; loop++) {
      if(!execute_loop(prog, loopstart, loop, loopend, varname, pos)) {
        return 0;
      }
    }
  }
  else {
    for(int loop = loopstart; loop >= loopend; loop--) {
      if(!execute_loop(prog, loopend, loop, loopstart, varname, pos)) {
        return 0;
      }
    }
  }

  prog->mode = live_mode;
  return 1;
}

void insert_brackets(Program *prog)
{
  int brackets = 0; // We will count the number of unmatched brackets we have
  int cw = 0; // A local one so we don't have to reset it back in live_loop()
  while(prog->strings[cw][0] != '\0') { // Go to the end of the list
    if(prog->strings[cw][0] == '{') {
      brackets += 1;
    }
    if(prog->strings[cw][0] == '}') {
      brackets -= 1; // They may have fully entered their own loop
      // read_text() has also put one in,
    }
    cw += 1;
  }
  for(int i = 0; i < brackets; i++) {
    prog->strings[cw][0] = '}'; // Put another bracket there
    cw += 1;
    fprintf(prog->test.record, "Inserting }\n");
  }
}

/* SDL BACKEND */

void clear_screen(SDLwin *sw) 
{
  SDL_SetRenderDrawColor(sw->rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(sw->rend);
  SDL_RenderPresent(sw->rend);
}

void update_screen(SDLwin *sw)
{
  SDL_RenderPresent(sw->rend);
  SDL_UpdateWindowSurface(sw->win);
}

void SDLwait(SDLwin *sw)
{
  SDL_Event event;
  while(!sw->exit) {
    while(SDL_PollEvent(&event)) {
      if(event.type==SDL_QUIT || event.type==SDL_KEYDOWN) {
        sw->exit = 1;
      }
    }
  }
}

void check_events(Program *prog) 
{
  SDL_Event event;
  if(SDL_PollEvent(&event)) {
    if(event.type == SDL_QUIT) {
      finish_turtle(prog, ERROR(prog, "Program terminated by user."));
      exit(1);
    }
    if(event.type == SDL_KEYDOWN) {
      fprintf(prog->test.record, "keydown\n");
      switch(keydown(prog, event.key.keysym.sym, NULL)) {

        case enter :
          prog->turt.speed = 0;
          break;

        case back : 
          if(prog->turt.speed < 2) {
            prog->turt.speed = 1;
          }
          else {
            prog->turt.speed /= 2;
          }
          break;

        case space :
          prog->turt.speed *= 2;
          break;

        default : 
          break;

      }
    }
  }
}

void SDLsetup(SDLwin *sw)
{
  if(SDL_Init(SDL_INIT_VIDEO)!=0) {
    SDLERROR("Unable to initialize SDL");
  } 

  sw->exit = 0;

  sw->win=SDL_CreateWindow("sm1313's Turtle",
                     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                     WINSIZE, WINSIZE, SDL_WINDOW_SHOWN);
  if(sw->win == NULL){
    SDLERROR("Unable to initialize SDL Window");
  }

  sw->rend = SDL_CreateRenderer(sw->win, -1, 0);
  if(sw->rend == NULL){
    SDLERROR("Unable to initialize SDL Renderer");
  }

  clear_screen(sw);
  SDL_SetRenderDrawColor(sw->rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

void SDLERROR(char* error)
{
  fprintf(stderr, "\n%s:\n%s\n\n", error, SDL_GetError());
  SDL_Quit();
  exit(1);
}
