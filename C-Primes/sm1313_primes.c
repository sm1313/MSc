#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>

#define  MAXNO  65536
#define  MAXFACTORS  16
#define  DELAY  1000
#define  SDL_8BITCOLOUR  256
#ifndef  M_PI
 #define M_PI  3.14159265358979323846
 #endif  
#define  WINSIZE  750

typedef struct SDLwin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
} SDLwin;

enum{CIRCLE, TWOS, SQUARE};

void singlenum(int n, int factors[MAXFACTORS], SDLwin *sw);
void visualise(int factors[MAXFACTORS], int cnt, SDLwin *sw,
               float cx, float cy, float dist);
void go_deeper(int factors[MAXFACTORS], int cnt, SDLwin *sw, 
               float cx, float cy, float dist, int j);
void twos(int factor, int cnt, SDLwin *sw,
          float cx, float cy, float dist, int j);
void squares(int factor, int cnt, SDLwin *sw, 
             float cx, float cy, float dist, int j);
void nextcoordinates(int factor, int newcentre[MAXFACTORS],
                 float cx, float cy, float dist, int i, int j, int type);
// Finding prime factors.
int primefactorisation(int n, int factors[MAXFACTORS]);
int factorise(int n, int factors[MAXFACTORS], int cnt);
int prime(int n);
// SDL stuff.
void set_colour(SDLwin *sw, float angle);
float angle_calc(float cx, float cy);
void SDLcircle(SDLwin *sw, int cx, int cy, int r);
void SDLsetup(SDLwin *sw);
void SDLhold(SDLwin *sw, int n);
void SDLrefresh(SDLwin *sw, int n);
void SDLwintitle(SDLwin *sw, int n);
void SDLevents(SDLwin *sw);
void SDLerror(void);


int main(int argc, char **argv)
{
  int n, cnt, i, factors[MAXFACTORS];
  SDLwin sw;
  SDLsetup(&sw);

  if(argv[1]!=NULL && sscanf(argv[1], "%d", &n)==1) {
    // Just present the visualisation of the number given.
    singlenum(n, factors, &sw);
  }

  else {
    i=1; // We will increase i and do each number in turn.
    while(!sw.finished) {
      cnt = primefactorisation(i, factors);
      visualise(factors, cnt, &sw, 
                WINSIZE/2.0, WINSIZE/2.0, WINSIZE/4.0);
      SDLrefresh(&sw, i);
      i++;
    }
  }
  SDL_Quit();
  return 0;
}

void singlenum(int n, int factors[MAXFACTORS], SDLwin *sw)
// Called if a number to visualise is specified in argv[1].
{
  int cnt;
  if(n>MAXNO) {
    fprintf(stderr, "\nPlease pick a smaller number.\n\n");
    exit(1);
  }
  cnt = primefactorisation(n, factors);
  visualise(factors, cnt, sw, 
            WINSIZE/2.0, WINSIZE/2.0, WINSIZE/4.0);
  SDLhold(sw, n);
}

void visualise(int factors[MAXFACTORS], int cnt, SDLwin *sw,
               float cx, float cy, float dist)
// Does the first visualisation then goes deeper.
/* This is necessary for catching odd powers of two, since the function
   'twos' uses the previous factor (for angling purposes), and if there
   is no previous factor bad things will happen. Hence, we do not call 
   'twos' for the first/largest factor. 
   It will also catch the visualisation of 1. */
{
  int factor = factors[cnt-1];

  if(factor==1) { // DRAW A BIG RED CIRCLE IN THE MIDDLE OF THE SCREEN.
    SDLcircle(sw, WINSIZE/2.0, WINSIZE/2.0, WINSIZE/4.0);
    return;
  }

  if(factor==2 && cnt%2==0) { // i.e. an even power of 2 (multiple of 4).
    squares(2, cnt, sw, cx, cy, dist/2.0, 0);
    return;
  }
  // This is usually done by 'twos', but we are not calling that here.

  int *newcentre;
  newcentre = malloc(2*factor*sizeof(int));  

  for(int i=0; i<factor; i++) {
    nextcoordinates(factor, newcentre, cx, cy, dist, i, -1, CIRCLE);      
    go_deeper(factors, cnt-1, sw, 
      newcentre[2*i], newcentre[(2*i)+1], dist/factor, i);
  }
  free(newcentre);
}
void go_deeper(int factors[MAXFACTORS], int cnt, SDLwin *sw, 
               float cx, float cy, float dist, int j)
{
  if(cnt<=0) { // If there are no more factors, print a circle here.
    SDLcircle(sw, cx, cy, dist);
    return;
  }

  int factor = factors[cnt-1]; // Next factor.

  if(factor==2) {
    twos(factors[cnt], cnt, sw, cx, cy, dist/factor, j);
    return;
  }

  int *newcentre;
  newcentre = malloc(2*factor*sizeof(int));

  for(int i=0; i<factor; i++) {
    nextcoordinates(factor, newcentre, cx, cy, dist, i, -1, CIRCLE);      
    go_deeper(factors, cnt-1, sw, newcentre[2*i], newcentre[(2*i)+1], dist/factor, i);
  }
  free(newcentre);
}

void twos(int factor, int cnt, SDLwin *sw,
          float cx, float cy, float dist, int j)
  // Deals with when remaining factors are 2s.
{
  if(cnt%2==0) {
    squares(factor, cnt, sw, cx, cy, dist, j);
    return;
  } 

  /* else, we need to split the area into two, THEN do some squares,
     since cnt will then be an even number. */

  int *newcentre;
  newcentre = malloc(2*2*sizeof(int)); // 2 centre co-ordinates for 2 squares.

  for(int i=0; i<2; i++) {
    nextcoordinates(factor, newcentre, cx, cy, dist, i, j, TWOS);
    squares(factor, cnt-1, sw, newcentre[2*i], newcentre[(2*i)+1], dist, j);
  }
  // We could call 'twos' recursively at the end here, but there's no point
  // since cnt will be even and squares will immediately be called anyway.

  free(newcentre);
}

void squares(int factor, int cnt, SDLwin *sw, 
             float cx, float cy, float dist, int j)
{
  if(cnt<=0) { // No more factors left; print a circle here.
    SDLcircle(sw, cx, cy, dist);
    return;
  }

  // else, continue recursively making squares.

  int *newcentre;
  newcentre = malloc(2*4*sizeof(int)); // 2 co-ordinates for 4 corners.

  for(int i=0; i<4; i++) {
    nextcoordinates(factor, newcentre, cx, cy, dist*2.0, i, j, SQUARE);
    squares(factor, cnt-2, sw, newcentre[2*i], newcentre[(2*i)+1], dist/2.0, j);
  }
  free(newcentre);
} 

void nextcoordinates(int factor, int newcentre[MAXFACTORS],
                   float cx, float cy, float dist, int i, int j, int type)
{
  float angle, r;

  switch(type) {
    case CIRCLE: angle = (2.0*M_PI/factor)*i;
                 r = dist;
                 break;
    case TWOS:   angle = (M_PI*i)+((2.0*j*M_PI)/factor);
                 r = 2.0*dist;  
                 break;
    case SQUARE: angle = i*(M_PI/2.0) -(M_PI/4.0) + ((2.0*j*M_PI)/factor);
                 r = (dist/(2.0*sin(M_PI/4.0)));
                 break;
    default: fprintf(stderr, "PANIC. Type error in nextcoordinates.\n\n");
             exit(1);
  }

  newcentre[2*i] = cx + (r*sin(angle));
  newcentre[(2*i)+1] = cy - (r*cos(angle));
}

int primefactorisation(int n, int factors[MAXFACTORS])
{
  int cnt=0;

  cnt = factorise(n, factors, cnt);
  for(int i=0; i<cnt; i++) {
  }
  return cnt;
}

int factorise(int n, int factors[MAXFACTORS], int cnt)
{
  if(prime(n)) { // Base case (note: prime(1) returns 1).
    factors[cnt]=n;
    cnt++;
    return cnt;
  }
  else {
    for(int i=2; i<=n/2; i++) {
      if(n%i==0) {
        factors[cnt] = i;
        cnt++;
        cnt = factorise(n/i, factors, cnt);
        return cnt;
      }
    }
  }
  // We should not get this far.
  fprintf(stderr, "PANIC. Factorisation error.\n\n");
  exit(1); 
}

int prime(int n)
{
  for(int i=2; i<=sqrt(n); i++) {
    if(n%i==0) {
      return 0;
    }
  }
  // Note that if 1 or 2 are passed, 1 will also be returned
  // since the for loop will be skipped entirely.
  return 1;
}

void SDLsetcolour(SDLwin *sw, int cx, int cy)
{
  float angle = angle_calc(cx, cy);
  Uint8 r, g, b;
    float rad = sqrt(pow((WINSIZE/2)-cx,2)+pow((WINSIZE/2)-cy,2));
    r = g = b = (int)((255.0/WINSIZE)*rad);
    // Provides slight variation of colour radiating from the centre.
  float sixth = M_PI/3.0, third = 2*sixth;

  if(angle<0) {
    angle = (2.0*M_PI)+angle;
  }

  if(angle<third) {
    if(angle<sixth) {
      r = 255;
      b = 255.0*(angle/sixth);
    }
    else {
      b = 255;
      r = 255.0*(1-((angle-sixth)/sixth));
    }
  }
  if(angle>=third && angle<(2.0*third)) {
    if(angle<(3*sixth)) {
      b = 255;
      g = 255.0*((angle-third)/sixth);
    }
    else {
      g = 255;
      b = 255.0*(1-((angle-(3*sixth))/sixth));

    }
  }
  if(angle>=(2*third)) {
    if(angle<(5*sixth)) {
      g = 255;
      r = 255.0*((angle-2*third)/sixth);
    }
    else {
      r = 255;
      g = 255.0*(1-((angle-(5*sixth))/sixth));
    }
  }
  SDL_SetRenderDrawColor(sw->renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

float angle_calc(float cx, float cy) 
{
  /* Calculates the angle of the current position 
     relative to the centre of the screen */
  float angle=0.0;
  float x=cx-(WINSIZE/2.0), y=(WINSIZE/2.0)-cy;
  if(x==0 && y==0) {
    angle = 0.0;
  }
  if(x>=0 && y>0) {
    angle = atan(x/y);
  }
  if(x>0 && y<=0) {
    angle = atan((-y)/x)+(M_PI/2.0);
  }
  if(x<0 && y>=0) {
    angle = atan(y/(-x))+((3.0/2.0)*M_PI);
  }
  if(x<=0 && y<0) {
    angle = atan(x/y)+M_PI;
  }
  return angle;
}

void SDLcircle(SDLwin *sw, int cx, int cy, int dist)
{
  SDLsetcolour(sw, cx, cy);

  for (double dy = 1; dy <= dist; dy += 1.0) {
    double dx = floor(sqrt((2.0 * dist * dy) - (dy * dy)));
    SDL_RenderDrawLine(sw->renderer, cx-dx, cy+dist-dy, cx+dx, cy+dist-dy);
    SDL_RenderDrawLine(sw->renderer, cx-dx, cy-dist+dy, cx+dx, cy-dist+dy);
  }
}

void SDLsetup(SDLwin *sw)
{
  if(SDL_Init(SDL_INIT_VIDEO)!=0) {
    fprintf(stderr, "\nUnable to initialize SDL:\n");
    SDLerror();
  } 

  sw->finished = 0;
  sw->win=SDL_CreateWindow("Visualisation of Prime Factorisations.",
                           SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                           WINSIZE, WINSIZE, SDL_WINDOW_SHOWN);
  if(sw->win == NULL){
    fprintf(stderr, "\nUnable to initialize SDL Window:\n");
    SDLerror();
  }
  sw->renderer = SDL_CreateRenderer(sw->win, -1, 0);
  if(sw->renderer == NULL){
    fprintf(stderr, "\nUnable to initialize SDL Renderer:\n");
    SDLerror();
  }

  SDL_SetRenderDrawColor(sw->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(sw->renderer);
  SDL_RenderPresent(sw->renderer);
}

void SDLhold(SDLwin *sw, int n)
{
  SDL_RenderPresent(sw->renderer);
  SDL_UpdateWindowSurface(sw->win);
  SDLwintitle(sw, n);
  while(!sw->finished) {
    SDLevents(sw);
  }
}

void SDLrefresh(SDLwin *sw, int n) 
{
  SDL_RenderPresent(sw->renderer);
  SDL_UpdateWindowSurface(sw->win);
  SDLwintitle(sw, n);
  SDL_Delay(DELAY);
  SDLevents(sw);
  // Now reset the screen after the delay, for the next number.
  SDL_SetRenderDrawColor(sw->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(sw->renderer);
  SDL_RenderPresent(sw->renderer);
}

void SDLwintitle(SDLwin *sw, int n)
{
  char *text = "Visualisation of the prime factorisation of: ";
  char str[strlen(text)+5+1]; // 5 is the length of our MAXNO.
  sprintf(str, "%s%d", text, n);
  SDL_SetWindowTitle(sw->win, str);
}

void SDLevents(SDLwin *sw)
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch (event.type){
      case SDL_QUIT:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_KEYDOWN:
        sw->finished = 1;
    }
  }
}
void SDLerror(void)
{
  fprintf(stderr, "%s\n\n", SDL_GetError());
  SDL_Quit();
  exit(1);
}
