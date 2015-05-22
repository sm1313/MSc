#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL.h>
#include <math.h>

#define  SIZEMAX  49  // Change it all you like, 
                      // just make it an odd number please. 
#define  SIZEMIN  5
#define  SWAP(A,B)  {int tmp=B; B=A; A=tmp;}

#define  SDL_8BITCOLOUR  256
#define  PIXELSIZE  10 // Make sure PIXELSIZE is less than the 
                       // max window dimension divided by SIZEMAX.
#define  BORDER  10
#define  DELAY  20

// All Neill's SDL stuff.
struct SDL_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
};
typedef struct SDL_Simplewin SDL_Simplewin;

void Neill_SDL_Init(SDL_Simplewin *sw, int WIDTH, int HEIGHT);
void Neill_SDL_Events(SDL_Simplewin *sw);
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b);
//

//  NOTE: Rows count ascending top to bottom.
//        Columns count ascending left to right.

enum{BLANK, WALL, EXPLORED, EXIT};

// The below functions are used to solve the maze:
int solving(int array[SIZEMAX][SIZEMAX], char map[SIZEMAX][SIZEMAX], 
            int width, int height, 
            int SDL, SDL_Simplewin *sw);
// (Gets us going. Returns 1 if exit found, 0 if not.)
int find_entrance(char map[SIZEMAX][SIZEMAX], int width, int height,
                   int *rowent, int *colent);
int explore(int array[SIZEMAX][SIZEMAX], int rowpos, int colpos, 
            char map[SIZEMAX][SIZEMAX], int width, int height, 
            int SDL, SDL_Simplewin *sw);
// (Recursion happens here.)
//
// The below functions are used with a maze file:
void use_file(char map[SIZEMAX][SIZEMAX], int argc, char **argv,
              int *width, int *height);
void fill_map(FILE *maze, char map[SIZEMAX][SIZEMAX], 
              int width, int height);
// (Translates the maze from the file into the map array.)
//
// The below functions generate a random maze/map:
void generate_maze(char map[SIZEMAX][SIZEMAX], int *width, int *height);
void border_map(char map[SIZEMAX][SIZEMAX], int width, int height);
void make_entrance(char map[SIZEMAX][SIZEMAX], int width, int height);
void make_exit(char map[SIZEMAX][SIZEMAX], int width, int height);
void recursive_diversion(char map[SIZEMAX][SIZEMAX], 
                         int width, int height, 
                         int top_edge, int left_edge);
int wall(char a, char map[SIZEMAX][SIZEMAX], 
         int top_edge, int left_edge, int width, int height);
int gap(char a, int g, char map[SIZEMAX][SIZEMAX], 
        int top_edge, int left_edge, int width, int height, 
        int row_wall, int col_wall);
// (The character a passed to these two functions is used to
// differentiate between dealing with rows and columns.)
int ALL_PURPOSE_ALGORITHM(int lower_bound, int upper_bound, int control);
// Gets a random value between the upper and lower bounds, with extra conditions.
void pickgap(char map[SIZEMAX][SIZEMAX], int row_wall, int col_wall, 
               int left_edge, int top_edge, 
               int *row_wall_gap_1, int *row_wall_gap_2, 
               int *col_wall_gap_1, int *col_wall_gap_2);
// Chooses which gap out of four not to create, as per recursive diversion.
void write_gap(char a, char map[SIZEMAX][SIZEMAX], int wall, int gap);
void write_wall(char a, char map[SIZEMAX][SIZEMAX], int wall, int edge, int length);
//
// The below functions are used both for given and random mazes:

void print_map(char map[SIZEMAX][SIZEMAX], 
               int width, int height);
void write_solution(int array[SIZEMAX][SIZEMAX], 
                    char map[SIZEMAX][SIZEMAX],
                    int width, int height);
void maze_error(int row, int type);
// (Is called when something is unexpected.)
void zero_array(int array[SIZEMAX][SIZEMAX]);
void null_array(char map[SIZEMAX][SIZEMAX]);
//
// This function is only ever called if SDL is specified.
void SDL_Draw_Map(SDL_Simplewin *sw, char map[SIZEMAX][SIZEMAX], 
                 int array[SIZEMAX][SIZEMAX], int width, int height,
                 int rowpos, int colpos, int SDL);
// This one is called if it is not.
void did_you_know_about_SDL();
//

int main(int argc, char **argv)
{
  int width, height, solvable, SDL=0;
  char map[SIZEMAX][SIZEMAX]; // This will be the full maze.
  int array[SIZEMAX][SIZEMAX]; // This will be filled as we explore.
  SDL_Simplewin sw;
  srand(time(NULL));

  zero_array(array); // Sets all values to BLANK (unexplored).
  null_array(map);   // For random generation.

  if(argc<2) {
    fprintf(stderr, "\nPlease type a file from which to read a maze");
    fprintf(stderr, " or type RANDOM for a random maze.\n\n");
    exit(1);
  }
  if(strcmp(argv[1], "RANDOM")==0) {
    generate_maze(map, &width, &height);
  }
  else {
    use_file(map, argc, argv, &width, &height);
  }

  if(argv[2]!=NULL && strcmp(argv[2], "SDL")==0) {
    SDL=1;
    Neill_SDL_Init(&sw, (width*PIXELSIZE) + 2*BORDER, 
                        (height*PIXELSIZE) + 2*BORDER);
  }

  printf("The maze:\n\n");
  print_map(map, width, height);

  solvable = solving(array, map, width, height, SDL, &sw);

  if(SDL) {
    while(!sw.finished) {
      Neill_SDL_Events(&sw);
    }
    atexit(SDL_Quit);
  }
  else {
    did_you_know_about_SDL(); // Lets the user know about SDL.
  }

  if(solvable) {
    return 0; 
  }
  else {
    return 1;
  }
}

int solving(int array[SIZEMAX][SIZEMAX], char map[SIZEMAX][SIZEMAX], 
            int width, int height,  
            int SDL, SDL_Simplewin *sw)
{
  int rowent=0, colent=0;
  int solvable;  // Declares whether we found an exit.

  if(find_entrance(map, width, height, &rowent, &colent)==0) {
    maze_error(-1, 4); // (Error = entrance)
  }
  array[rowent][colent]=EXPLORED;

  SDL_Draw_Map(sw, map, array, width, height, rowent, colent, SDL);

  if(rowent==0) {
    solvable=explore(array, 1, colent, map, width, height, SDL, sw);
  }
  else {
    solvable=explore(array, rowent, 1, map, width, height, SDL, sw);
  }

  if(solvable) {
    array[rowent][colent]=EXIT;
    write_solution(array, map, width, height);
    printf("The solution:\n\n");
    print_map(map, width, height);
    printf("Freedom!\n\n");
    SDL_Draw_Map(sw, map, array, width, height, rowent, colent, SDL);
  }
  else {
    printf("This maze has no exit.\n\n");
  }
  return solvable; // Returns 1 if we are out, 0 if not.
}

int find_entrance(char map[SIZEMAX][SIZEMAX], int width, int height,
                   int *rowent, int *colent)
{ 
  /* Searches for an entrance, starting from row 0, col 1, then 
  1,0 onto 0,2 then 2,0, then 0,2 then 3,0 etc.
  l is used as a marker to indicate that we have reached the end
  of one of the edges, but we need to continue searching the other. */
  int cnt=0, row=0, col=1, l=0;
  while(1) {
    if(l==1 && (row==height-1 || col==width-1)) {
    return 0;
    }
    if(map[row][col]==' ') {
      *rowent = row;
      *colent = col;
      return 1;
    }
    if(row!=height-1 && col!=width-1 && l!=1) {
      SWAP(row, col);
      if(cnt%2==1) {
        col++;
      }
    }
    else {
      if(row>=width-1 && l==1) {
        row++;
      }
      if(col>=height-1 && l==1) {
        col++;
      }
      if(row==height-1 || col==width-1) {
        l=1;
        SWAP(row, col);
      }

    }
    cnt++;
  }
}

int explore(int array[SIZEMAX][SIZEMAX], int rowpos, int colpos, 
            char map[SIZEMAX][SIZEMAX], int width, int height, 
            int SDL, SDL_Simplewin *sw)
{
  if(array[rowpos][colpos]==EXPLORED) {
    return 0; // Been here before, don't explore again.
  }
  if(map[rowpos][colpos]=='#') {
    array[rowpos][colpos]=WALL;
    return 0; // Attempted current position is a wall - go back.
  }
  if(rowpos==height-1 || colpos==width-1 || rowpos==0 || colpos==0) {
    array[rowpos][colpos]=EXIT;
    return 1;  // We are at the boundary - hence an exit.
  }

  SDL_Draw_Map(sw, map, array, width, height, rowpos, colpos, SDL);

  array[rowpos][colpos]=EXPLORED; 
  // If the function got this far, we are in a space. We mark it as
  // EXPLORED so we don't explore it again, and now continue exploring.

  if(explore(array, rowpos+1, colpos, map, width, height, SDL, sw)) {
    array[rowpos][colpos]=EXIT;
    return 1;
  }
  SDL_Draw_Map(sw, map, array, width, height, rowpos, colpos, SDL);
  if(explore(array, rowpos, colpos+1, map, width, height, SDL, sw)) {
    array[rowpos][colpos]=EXIT;
    return 1;
  }
  SDL_Draw_Map(sw, map, array, width, height, rowpos, colpos, SDL);
  if(explore(array, rowpos-1, colpos, map, width, height, SDL, sw)) {
    array[rowpos][colpos]=EXIT;
    return 1;
  }
  SDL_Draw_Map(sw, map, array, width, height, rowpos, colpos, SDL);
  if(explore(array, rowpos, colpos-1, map, width, height, SDL, sw)) {
    array[rowpos][colpos]=EXIT;
    return 1;
  }
  SDL_Draw_Map(sw, map, array, width, height, rowpos, colpos, SDL);

  return 0;
}

void use_file(char map[SIZEMAX][SIZEMAX], int argc, char **argv,
              int *width, int *height)
{
  FILE *maze;

  if(argc<2 || (maze = fopen(argv[1], "r"))==NULL) {
    fprintf(stderr, "\nCannot open %s. Exiting program.\n\n", argv[1]);
    exit(1);
  }
  fscanf(maze, "%d %d", width, height);
  getc(maze);  
  // Eats up the new line after the dimensions in the file.
  fill_map(maze, map, *width, *height);  // Translates file to map.
  fclose(maze); // We don't need anything else.
}

void fill_map(FILE *maze, char map[SIZEMAX][SIZEMAX],
              int width, int height)
{
  char c;
  int row=0, col=0;

  while((c=getc(maze))!=EOF) {
    if(row<height) { // Now any extra lines in the file are ignored.
      if(c!='\n' && c!=' ' && c!= '#' && c!= '.') {
        maze_error(row, 1); // (Error = char)
      }
  
      if(c=='\n') {
        if(col!=width) {
          maze_error(row, 2); // (Error = row size)
        }
        row++;
        col=0;
      }
      else {
        map[row][col]=c;
        col++;
      }
    }
  }
  if(row<height) {
    maze_error(row, 3); // (Error = maze height)
  }
}

void generate_maze(char map[SIZEMAX][SIZEMAX], int *width, int *height)
{
  *width = ALL_PURPOSE_ALGORITHM(SIZEMIN, SIZEMAX, 0);
  *height = ALL_PURPOSE_ALGORITHM(SIZEMIN, SIZEMAX, 0);

  border_map(map, *width, *height);    // Make a border wall.
  make_entrance(map, *width, *height); // Put an entrance in it.
  make_exit(map, *width, *height);     // Put an exit in it.
  recursive_diversion(map, *width, *height, 0, 0); // Generation.
}

void border_map(char map[SIZEMAX][SIZEMAX], int width, int height)
{
  for(int rows=0; rows<height; rows++) {
    map[rows][0]='#';
    map[rows][width-1]='#';
  }
  for(int cols=0; cols<width; cols++) {
    map[0][cols]='#';
    map[height-1][cols]='#';
  }
}

void make_entrance(char map[SIZEMAX][SIZEMAX], int width, int height)
{
  // The entrance will only ever be on the top or the left. 
  // The rand() in the switch picks which it will be. 
  switch (rand()%2) {
    case 0: map[0][ALL_PURPOSE_ALGORITHM(1, width-2, 1)]=' ';  
      break;
    case 1: map[ALL_PURPOSE_ALGORITHM(1, height-2, 1)][0]=' ';
      break;
  }
}

void make_exit(char map[SIZEMAX][SIZEMAX], int width, int height)
{
  // The exit could be created on any of the four sides.
  // The rand()%4 in the switch picks which it will be.
  int r;
  switch (rand()%4) {
    case 0: do {
              r=ALL_PURPOSE_ALGORITHM(1, width-2, 1);
            } while (map[0][r]==' ');
      map[0][r]=' ';
      break;
    case 1: do {
              r=ALL_PURPOSE_ALGORITHM(1, height-2, 1);
            } while (map[r][0]==' ');
      map[r][0]=' ';
      break;
    case 2: do {
              r=ALL_PURPOSE_ALGORITHM(1, width-2, 1);
            } while (map[height-1][r]==' ');
      map[height-1][r]=' ';
      break;
    case 3: do {
              r=ALL_PURPOSE_ALGORITHM(1, height-2, 1);
            } while (map[r][width-1]==' ');
      map[r][width-1]=' ';  
      break;
  }
  /* All the conditions in the while loops make sure that the exit
  is not created on or next to the entrance.
     Note that the exit may be created closer to the top left corner 
  than the entrance. In this case the entrance FOUND by 
  find_entrance will be the exit that was generated here, and
  what was created as the entrance will now become the exit. */

}

void recursive_diversion(char map[SIZEMAX][SIZEMAX], 
                         int width, int height, 
                         int top_edge, int left_edge)
{
  // I will be treating the border of each chamber as its edge.
  // So a chamber with a single space would have a width/height of 3.
  int row_wall, col_wall;
  int row_wall_gap_1=-1, row_wall_gap_2=-1;
  int col_wall_gap_1=-1, col_wall_gap_2=-1;

  if(height<5 || width<5) {
    return;
  }

  row_wall = wall('r', map, top_edge, left_edge, width, height);
  col_wall = wall('c', map, top_edge, left_edge, width, height);
  
  if(row_wall!=-1) {
   row_wall_gap_1=gap('r', 1, map, top_edge, left_edge, width, height,
                      row_wall, col_wall);
   row_wall_gap_2=gap('r', 2, map, top_edge, left_edge, width, height,
                      row_wall, col_wall);
  }

  if(col_wall!=-1) {
    col_wall_gap_1=gap('c', 1, map, top_edge, left_edge, width, height, 
                       row_wall, col_wall);
    col_wall_gap_2=gap('c', 2, map, top_edge, left_edge, width, height,
                       row_wall, col_wall);
  }

  pickgap(map, row_wall, col_wall, left_edge, top_edge,
           &row_wall_gap_1, &row_wall_gap_2, 
           &col_wall_gap_1, &col_wall_gap_2);

  write_gap('r', map, row_wall, row_wall_gap_1);
  write_gap('r', map, row_wall, row_wall_gap_2);
  write_gap('c', map, col_wall, col_wall_gap_1);
  write_gap('c', map, col_wall, col_wall_gap_2);

  recursive_diversion(map, col_wall-left_edge+1, 
                      row_wall-top_edge+1, top_edge, left_edge);
  recursive_diversion(map, col_wall-left_edge+1, 
                      top_edge+height-row_wall, row_wall, left_edge);
  recursive_diversion(map, left_edge+width-col_wall, 
                      row_wall-top_edge+1, top_edge, col_wall);
  recursive_diversion(map, left_edge+width-col_wall, 
                      top_edge+height-row_wall, row_wall, col_wall);
}

int wall(char a, char map[SIZEMAX][SIZEMAX], 
         int top_edge, int left_edge, int width, int height)
{
  // Walls are only ever put on ODD-valued rows and columns. 
  int wall=-1;
  if(a=='r') {
    wall = ALL_PURPOSE_ALGORITHM(top_edge+2, top_edge+height-3, 2);
    write_wall('r', map, wall, left_edge, width);
  }
  if(a=='c') {
    wall = ALL_PURPOSE_ALGORITHM(left_edge+2, left_edge+width-3, 2);
    write_wall('c', map, wall, top_edge, height);
  }
  return wall;
}

int gap(char a, int g, char map[SIZEMAX][SIZEMAX], 
        int top_edge, int left_edge, int width, int height, 
        int row_wall, int col_wall)
{
  // Gaps are only ever put in EVEN-valued cells.
  int gap=-1;
  if(a=='r') {
    if(g==1) {
      if(map[row_wall][left_edge]==' ') { // Keep all of the gaps clear.
        gap = -1;
        write_gap('r', map, row_wall, left_edge+1);
        // We write the gap straight away in case this gap is selected by pickgap.
        // We set it to -1 for pickgap to see it's already been made.
      }
      else {
        do {
          gap = ALL_PURPOSE_ALGORITHM(left_edge+1, col_wall-1, 1);
        } while (gap == col_wall && gap != -1);
      }
    }
    if(g==2) {
      if(map[row_wall][left_edge+width-1]==' ') {
        gap = -1;
        write_gap('r', map, row_wall, left_edge+width-2);
      }
      else {
        do {
          gap = ALL_PURPOSE_ALGORITHM(col_wall+1, left_edge+width-2, 1);
        } while (gap == col_wall && gap != -1);
      }
    }
  }
  if(a=='c') {  // Same as above, modified for dealing with column gaps.
    if(g==1) {
      if(map[top_edge][col_wall]==' ') {
        gap = -1;
        write_gap('c', map, col_wall, top_edge+1);
      }
      else {
        do {
          gap = ALL_PURPOSE_ALGORITHM(top_edge+1, row_wall-1, 1);
        } while (gap == row_wall && gap != -1);
      }
    }
    if(g==2) {
      if(map[top_edge+height-1][col_wall]==' ') {
        gap = -1;
        write_gap('c', map, col_wall, top_edge+height-2);
      }
      else {
        do {
          gap = ALL_PURPOSE_ALGORITHM(row_wall+1, top_edge+height-2, 1);
        } while (gap == row_wall && gap != -1);
      }
    }
  }
  return gap;
}

int ALL_PURPOSE_ALGORITHM(int lower_bound, int upper_bound, int control)
{
  /* This formula is given lower and upper bounds, and uses them to 
  generate a random valid place for gaps and walls. 
  The control is used to handle what type of value it is making. 
  Walls and gaps are only ever made on even- and odd-numbered rows and columns
  respectively, so the algorithm is passed 2 or 1 respectively.
  I also used it for random maze dimensions; here no control is needed. */

  int value;
  int range = upper_bound - lower_bound;
  if(range==0) {
    value = lower_bound;
  }
  else {
    value = (rand()%(range/2))*2 + lower_bound + control;
  }
  return value;

}

void pickgap(char map[SIZEMAX][SIZEMAX], int row_wall, int col_wall, 
               int left_edge, int top_edge, 
               int *row_wall_gap_1, int *row_wall_gap_2, 
               int *col_wall_gap_1, int *col_wall_gap_2)
{
  // Picks a gap to be discarded, i.e. one gap will not be made.
  // If a gap has already been made by gap() then it will be skipped.
  int r=rand()%4, cnt=0;
  while(cnt<=4) {
    if(r==0) {
      if(*row_wall_gap_1 == -1) { 
       r++; 
       cnt++;
      }
      else {
        *row_wall_gap_1 = -1;
        return;
      }
    }
    if(r==1) {
      if(*row_wall_gap_2 == -1) {
        r++;
        cnt++;
      }
      else {
        *row_wall_gap_2 = -1;
        return;
      }
    }
    if(r==2) {
      if(*col_wall_gap_1 == -1) {
        r++;
        cnt++;
        }
      else {
        *col_wall_gap_1 = -1;
        return;
      }
    }
    if(r==3) {
      if(*col_wall_gap_2 == -1) {
        r=0;
        cnt++;
      }
      else {
        *col_wall_gap_2 = -1;
        return;
      }
    }
  }
}

void write_gap(char a, char map[SIZEMAX][SIZEMAX], int wall, int gap)
{
  if(gap!=-1) {
    if(a=='r') {
      map[wall][gap]=' ';
    }
    if(a=='c') {
      map[gap][wall]=' ';
    }
  }
}

void write_wall(char a, char map[SIZEMAX][SIZEMAX], int wall, int edge, int length)
{
  if(wall!=-1) {
    if(a=='r') {
      for(int cols=edge+1; cols<edge+length-1; cols++) {
        map[wall][cols]='#';
      } 
    }
    if(a=='c') {
      for(int rows=edge+1; rows<edge+length-1; rows++) {
        map[rows][wall]='#';
      }
    }
  }
}

void print_map(char map[SIZEMAX][SIZEMAX], 
               int width, int height)
{
  for(int row=0; row<height; row++) {
    for(int col=0; col<width; col++) {
      printf("%c ", map[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

void write_solution(int array[SIZEMAX][SIZEMAX], 
                    char map[SIZEMAX][SIZEMAX],
                    int width, int height)
{
  // Note that this OVERWRITES the original map with the solution.
  for(int row=0; row<height; row++) {
    for(int col=0; col<width; col++) {
      if(array[row][col]==EXIT) {
        map[row][col]='.';
      }
    }
  }
}

void maze_error(int row, int type)
{
  fprintf(stderr, "\nError at maze row %d. ", row+1);
  switch (type) {
    case 1: fprintf(stderr, "(Unexpected character.)\n"); 
      break;
    case 2: fprintf(stderr, "(Row is not the expected length.)\n");
      break;
    case 3: fprintf(stderr, "(Maze is not the expected height.)\n");
      break;
    case 4: fprintf(stderr, "(There is no entrance. Please put the ");
             fprintf(stderr, "entrance on the top or left wall.)\n");
      break;
    default: 
    fprintf(stderr, "Please fix the file.\n\n");
  }
  printf("\n");
  atexit(SDL_Quit);
  exit(1);
}

void zero_array(int array[SIZEMAX][SIZEMAX])
{
  for(int i=0; i<SIZEMAX; i++) {
    for(int j=0; j<SIZEMAX; j++) {
      array[i][j]=BLANK;
    }
  }
}

void null_array(char map[SIZEMAX][SIZEMAX])
{
  for(int i=0; i<SIZEMAX; i++) {
    for(int j=0; j<SIZEMAX; j++) {
      map[i][j]=' ';
    }
  }
}

void SDL_Draw_Map(SDL_Simplewin *sw, char map[SIZEMAX][SIZEMAX], 
                 int array[SIZEMAX][SIZEMAX], int width, int height, 
                 int rowpos, int colpos, int SDL)
{
  if(SDL) {
    SDL_Rect rectangle;

    for(int row=0; row<height; row++) {
      for(int col=0; col<width; col++) {
        rectangle.w = PIXELSIZE;
        rectangle.h = PIXELSIZE;
        rectangle.x = col*PIXELSIZE + BORDER;
        rectangle.y = row*PIXELSIZE + BORDER;
  
        switch (map[row][col]) {
          case ' ': Neill_SDL_SetDrawColour(sw, 255,255,255);
            break;
          case '#': Neill_SDL_SetDrawColour(sw, 0,0,0);
            break;
        }
        switch (array[row][col]) {
          case EXPLORED: Neill_SDL_SetDrawColour(sw, 128,128,128);
            break;
          case EXIT: Neill_SDL_SetDrawColour(sw, 0,192,0);
            break;
        }
        if(row==rowpos && col==colpos) {
          Neill_SDL_SetDrawColour(sw, 255,0,0);
        }
        SDL_RenderFillRect(sw->renderer, &rectangle);
      }
    }

    SDL_Delay(DELAY);
    SDL_RenderPresent(sw->renderer);
    SDL_UpdateWindowSurface(sw->win);
    Neill_SDL_Events(sw);
    if(sw->finished) {
      printf("Exploration terminated. You are lost forever.\n\n");
      exit(0);
    }
  }
}

void did_you_know_about_SDL()
{
  printf("Did you know that you can type SDL at the end of the ");
  printf("command line\nto start up SDL and see all this happen ");
  printf("in GLORIOUS COLOUR?\n\n");
}

// Set up a simple (WIDTH, HEIGHT) window.
// Attempt to hide the renderer etc. from user.
void Neill_SDL_Init(SDL_Simplewin *sw, int WIDTH, int HEIGHT)
{


   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   } 

   sw->finished = 0;
   
   sw->win= SDL_CreateWindow("SDL Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          WIDTH, HEIGHT,
                          SDL_WINDOW_SHOWN);
   if(sw->win == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Window:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   sw->renderer = SDL_CreateRenderer(sw->win, -1, 0);
   if(sw->renderer == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Renderer:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   // Set screen to black
   Neill_SDL_SetDrawColour(sw, 255,255,255);
   SDL_RenderClear(sw->renderer);
   SDL_RenderPresent(sw->renderer);

}

// Gobble all events & ignore most
void Neill_SDL_Events(SDL_Simplewin *sw)
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

// Trivial wrapper to avoid complexities of renderer & alpha channels
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b)
{

   SDL_SetRenderDrawColor(sw->renderer, r, g, b, SDL_ALPHA_OPAQUE);

}
