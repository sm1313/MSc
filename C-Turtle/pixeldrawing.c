#include "turtle.h"

// I have designed each symbol in the language with a 3x5 pixel grid.
#define  LETTERW  3
#define  LETTERH  5

#define  PIXELSIZE  5
#define  TEXTPOS  WINSIZE-(PIXELSIZE*LETTERH)-2 // The y position of the textbox

void draw_character(Program *prog, int x, int y, char letter);
// draw_character picks which draw function to give its array of pixels 
void zero_pixels(int array[LETTERH][LETTERW]);
// zero_pixels sets all the values of the pixel array to zero (blank)
void draw_array(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
// draw_array draws the 2D array of pixels, if a 1 then draw, if a 0 then skip
void draw_pixel(Program* prog, int x, int y);
// draw_pixel draws the actual pixel at (x, y), with size PIXELSIZE

void draw_a(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_b(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_c(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_d(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_e(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_f(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_g(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_h(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_i(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_j(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_k(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_l(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_m(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_n(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_o(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_p(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_q(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_r(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_s(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_t(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_u(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_v(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_w(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_x(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_y(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_z(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_0(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_1(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_2(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_3(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_4(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_5(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_6(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_7(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_8(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_9(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_plus(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_minus(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_multiply(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_divide(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_point(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_colon(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_equals(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_semicolon(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_opening(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_closing(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_quote(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_exclaim(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);
void draw_question(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]);

// draw_text is called from turtle.c, so its declaration is in turtle.h
void draw_text(Program *prog) 
{
  int c = 0;
  // First draw the text box again.
  SDL_SetRenderDrawColor(prog->sw.rend, 223, 223, 223, SDL_ALPHA_OPAQUE);
  for(int i=0; i<=(LETTERH*PIXELSIZE); i++) {
    SDL_RenderDrawLine(prog->sw.rend, 0, TEXTPOS+i, WINSIZE, TEXTPOS+i);
  }
  switch(prog->textcolour) {
    case black :
      SDL_SetRenderDrawColor(prog->sw.rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
      break;
    case red :
      SDL_SetRenderDrawColor(prog->sw.rend, 255, 0, 0, SDL_ALPHA_OPAQUE);
      break;
    case blue :
      SDL_SetRenderDrawColor(prog->sw.rend, 64, 64, 255, SDL_ALPHA_OPAQUE);
      break;
  }
  // Now draw the two brackets at either side. 
  draw_character(prog, 0, TEXTPOS+1, '{');
  draw_character(prog, WINSIZE-(LETTERW*PIXELSIZE), TEXTPOS+1, '}');

  // Now draw the contents of the prog->text array, as entered by the user.
  while(prog->text[c]!='\0' && c < TEXTLEN) {
    draw_character(prog, (c+2)*((LETTERW*PIXELSIZE)+1), 
                  WINSIZE-(LETTERH*PIXELSIZE)-1, toupper(prog->text[c]));
    c += 1;
  }
  update_screen(&prog->sw);
}

void draw_character(Program* prog, int x, int y, char letter) 
{
  int pixels[LETTERH][LETTERW];
  zero_pixels(pixels);

  switch(letter) {
    case 'A' : draw_a(prog, x, y, pixels); break;
    case 'B' : draw_b(prog, x, y, pixels); break;
    case 'C' : draw_c(prog, x, y, pixels); break;
    case 'D' : draw_d(prog, x, y, pixels); break;
    case 'E' : draw_e(prog, x, y, pixels); break;
    case 'F' : draw_f(prog, x, y, pixels); break;
    case 'G' : draw_g(prog, x, y, pixels); break;
    case 'H' : draw_h(prog, x, y, pixels); break;
    case 'I' : draw_i(prog, x, y, pixels); break;
    case 'J' : draw_j(prog, x, y, pixels); break;
    case 'K' : draw_k(prog, x, y, pixels); break;
    case 'L' : draw_l(prog, x, y, pixels); break;
    case 'M' : draw_m(prog, x, y, pixels); break;
    case 'N' : draw_n(prog, x, y, pixels); break;
    case 'O' : draw_o(prog, x, y, pixels); break;
    case 'P' : draw_p(prog, x, y, pixels); break;
    case 'Q' : draw_q(prog, x, y, pixels); break;
    case 'R' : draw_r(prog, x, y, pixels); break;
    case 'S' : draw_s(prog, x, y, pixels); break;
    case 'T' : draw_t(prog, x, y, pixels); break;
    case 'U' : draw_u(prog, x, y, pixels); break;
    case 'V' : draw_v(prog, x, y, pixels); break;
    case 'W' : draw_w(prog, x, y, pixels); break;
    case 'X' : draw_x(prog, x, y, pixels); break;
    case 'Y' : draw_y(prog, x, y, pixels); break;
    case 'Z' : draw_z(prog, x, y, pixels); break;
    case '0' : draw_0(prog, x, y, pixels); break;
    case '1' : draw_1(prog, x, y, pixels); break;
    case '2' : draw_2(prog, x, y, pixels); break;
    case '3' : draw_3(prog, x, y, pixels); break;
    case '4' : draw_4(prog, x, y, pixels); break;
    case '5' : draw_5(prog, x, y, pixels); break;
    case '6' : draw_6(prog, x, y, pixels); break;
    case '7' : draw_7(prog, x, y, pixels); break;
    case '8' : draw_8(prog, x, y, pixels); break;
    case '9' : draw_9(prog, x, y, pixels); break;
    case '+' : draw_plus(prog, x, y, pixels); break;
    case '-' : draw_minus(prog, x, y, pixels); break;
    case '*' : draw_multiply(prog, x, y, pixels); break;
    case '/' : draw_divide(prog, x, y, pixels); break;
    case '.' : draw_point(prog, x, y, pixels); break;
    case ':' : draw_colon(prog, x, y, pixels); break;
    case '=' : draw_equals(prog, x, y, pixels); break;
    case ';' : draw_semicolon(prog, x, y, pixels); break;
    case '{' : draw_opening(prog, x, y, pixels); break;
    case '}' : draw_closing(prog, x, y, pixels); break;
    case '\'' : draw_quote(prog, x, y, pixels); break;
    case '!' : draw_exclaim(prog, x, y, pixels); break;
    case ' ' : break;
    // If none of the above were called, the character is not in the language,
    // hence there is no point in drawing it / creating a function for it.
    default :  draw_question(prog, x, y, pixels); break;
  }
  draw_array(prog, x, y, pixels);
}

void zero_pixels(int array[LETTERH][LETTERW]) 
{
  for(int row=0; row<LETTERH; row++) {
    for(int col=0; col<LETTERW; col++) {
      array[row][col] = 0; 
    }
  }
}

void draw_array(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int row=0; row<LETTERH; row++) {
    for(int col=0; col<LETTERW; col++) {
      if(pixels[row][col]) {
        draw_pixel(prog, x + (col*PIXELSIZE), y + (row*PIXELSIZE));
      }
    }
  }
}

void draw_pixel(Program* prog, int x, int y) 
{
  for(int i=0; i<PIXELSIZE; i++) {
    SDL_RenderDrawLine(prog->sw.rend, x+i, y, x+i, y+PIXELSIZE-1);
  }
}

void draw_a(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=1; i<5; i++) {
    pixels[i][0] = pixels[i][2] = 1;
  }
  pixels[0][1] = pixels[2][1] = 1;
}

void draw_b(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<5; i++) {
    pixels[i][0] = 1;
  }
  for(int i=0; i<5; i+=2) {
    pixels[i][1] = 1;
  }
  pixels[1][2] = pixels[3][2] = 1;
}

void draw_c(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=1; i<4; i++) {
    pixels[i][0] = 1;
  }
  pixels[0][1] = pixels[0][2] = pixels[4][1] = pixels[4][2] = 1;
}

void draw_d(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<5; i++) {
    pixels[i][0] = 1;
  }
  for(int i=1; i<4; i++) {
    pixels[i][2] = 1;
  }
  pixels[0][1] = pixels[4][1] = 1;
}

void draw_e(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<3; i++) {
    pixels[0][i] = pixels[4][i] = pixels[i+1][0] = 1;
  }
  pixels[2][1] = 1;
}

void draw_f(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_e(prog, x, y, pixels);
  pixels[4][1] = pixels[4][2] = 0;
}

void draw_g(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<3; i++) {
    pixels[i+1][0] = pixels[i+2][2] = 1;
  }
  pixels[0][1] = pixels[0][2] = pixels[4][1] = 1;
}

void draw_h(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<5; i++) {
    pixels[i][0] = pixels[i][2] = 1;
  }
  pixels[2][1] = 1;
}

void draw_i(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<3; i++) {
    pixels[0][i] = pixels[4][i] = pixels[i+1][1] = 1;
  }
}

void draw_j(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<5; i++) {
    pixels[i][2] = 1;
  }
  pixels[4][1] = pixels[4][0] = pixels[3][0] = 1;
}

void draw_k(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_h(prog, x, y, pixels);
  pixels[2][2] = 0;
}

void draw_l(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<5; i++) {
    pixels[i][0] = 1;
  }
  pixels[4][1] = pixels[4][2] = 1;
}

void draw_m(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_h(prog, x, y, pixels);
  pixels[1][1] = 1;
}

void draw_n(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_a(prog, x, y, pixels);
  pixels[0][0] = 1;
  pixels[2][1] = 0;
}

void draw_o(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_d(prog, x, y, pixels);
  pixels[0][2] = pixels[4][2] = 1;
}

void draw_p(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_f(prog, x, y, pixels);
  pixels[1][2] = pixels[2][2] = 1;
}

void draw_q(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_g(prog, x, y, pixels);
  pixels[0][0] = pixels[3][1] = pixels[1][2] = 1;
  pixels[3][2] = 0;
}

void draw_r(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_b(prog, x, y, pixels);
  pixels[2][2] = pixels[3][1] = pixels[4][2] = 1;
  pixels[3][2] = pixels[4][1] = 0;
}

void draw_s(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_b(prog, x, y, pixels);
  pixels[0][2] = 1;
  pixels[0][0] = pixels[1][2] = pixels[2][0] = pixels[3][0] = 0;
}

void draw_t(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_i(prog, x, y, pixels);
  pixels[4][0] = pixels[4][2] = 0;
}

void draw_u(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_o(prog, x, y, pixels);
  pixels[0][1] = pixels[4][0] = pixels[4][2] = 0;
}

void draw_v(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_u(prog, x, y, pixels);
  pixels[3][1] = 1;
  pixels[3][0] = pixels[3][2] = 0;
}

void draw_w(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_h(prog, x, y, pixels);
  pixels[3][1] = 1;
}

void draw_x(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_h(prog, x, y, pixels);
  pixels[2][0] = pixels[2][2] = 0;
}

void draw_y(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_v(prog, x, y, pixels);
  pixels[2][1] = 1;
  pixels[2][0] = pixels[2][2] = 0;
}

void draw_z(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_e(prog, x, y, pixels);
  pixels[1][2] = 1;
  pixels[1][0] = pixels[2][0] = 0;
}

void draw_0(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=1; i<4; i++) {
    pixels[i][0] = pixels[i][2] = 1;
  }
  pixels[0][1] = pixels[4][1] = 1;
}

void draw_1(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_i(prog, x, y, pixels);
  pixels[0][2] = 0;
}

void draw_2(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_z(prog, x, y, pixels);
  pixels[2][0] = pixels[2][2] = 1;
}

void draw_3(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_2(prog, x, y, pixels);
  pixels[3][2] = 1;
  pixels[3][0] = 0;
}

void draw_4(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_h(prog, x, y, pixels);
  pixels[3][0] = pixels[4][0] = 0;
}

void draw_5(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_3(prog, x, y, pixels);
  pixels[1][0] = 1;
  pixels[1][2] = 0;
}

void draw_6(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_5(prog, x, y, pixels);
  pixels[3][0] = 1;
}

void draw_7(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<5; i++) {
    pixels[i][2] = 1;
  }
  pixels[0][0] = pixels[0][1] = 1;
}

void draw_8(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_6(prog, x, y, pixels);
  pixels[1][2] = 1;
}

void draw_9(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_8(prog, x, y, pixels);
  pixels[3][0] = 0;
}

void draw_plus(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<3; i++) {
    pixels[2][i] = pixels[i+1][1] = 1;
  }
}

void draw_minus(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<3; i++) {
    pixels[2][i] = 1;
  }
}

void draw_multiply(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<3; i++) {
    pixels[i][i] = pixels[i][2-i] = 1;
  }
}

void draw_divide(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  pixels[0][2] = pixels[1][2] = pixels[2][1] = pixels[3][0] = pixels[4][0] = 1;
}

void draw_point(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  pixels[4][1] = 1;
}

void draw_colon(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  pixels[1][1] = pixels[3][1] = 1;
}

void draw_equals(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  for(int i=0; i<3; i++) {
    pixels[1][i] = pixels[3][i] = 1;
  }
}

void draw_semicolon(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  pixels[1][1] = pixels[3][1] = pixels[4][0] = 1;
}

void draw_opening(Program* prog, int x, int y, int pixels[LETTERH][LETTERW])
{
  draw_c(prog, x, y, pixels);
  pixels[1][1] = pixels[3][1] = 1;
  pixels[1][0] = pixels[3][0] = 0;
}
void draw_closing(Program* prog, int x, int y, int pixels[LETTERH][LETTERW])
{

  draw_opening(prog, x, y, pixels);
  pixels[0][0] = pixels[4][0] = pixels[2][2] = 1;
  pixels[0][2] = pixels[4][2] = pixels[2][0] = 0;
}

void draw_quote(Program* prog, int x, int y, int pixels[LETTERH][LETTERW])
{
  pixels[0][1] = pixels[1][1] = 1;
}

void draw_exclaim(Program* prog, int x, int y, int pixels[LETTERH][LETTERW])
{
  pixels[0][1] = pixels[1][1] = pixels[2][1] = pixels[4][1] = 1;
}
void draw_question(Program* prog, int x, int y, int pixels[LETTERH][LETTERW]) 
{
  draw_z(prog, x, y, pixels);
  pixels[3][0] = pixels[4][0] = pixels[4][2] = 0;
}
