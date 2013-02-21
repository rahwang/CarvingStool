/* Rachel Hwang, rah1 */
/* CS152 Winter 2012 */
/* hw5 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* global variables storing board information */
int  width = 20;
int height = 10;
int pause1 = 0;
int turns = 0;


/* prints ASCII version of given board to stdout */
void printBoard(char** board) {
  int i;
  int j;
  printf("+");
  for (i = 0; i < width; i++)
    printf("-");
  printf("+\n");
  for (j = 0; j < height; j++) {
      printf("|");
      for (i = 0; i < width; i++)
        printf("%c", board[i][j]);
      printf("|\n");
    }
  printf("+");
  for (i = 0; i< width; i++)
    printf("-");
  printf("+\n\n");
}


/* create current board from given live cells from stdin */
void getBoard(char** board)
{
  int i;
  int j;
  int x;
  int y;
  /* initialize cell values to O for dead */
  for (j = 0; j < height; j++) {
      for (i = 0; i < width; i++)
	board[i][j] = '.';
    }
  /* set live cell values to @ */
  while(fscanf(stdin, "%d %d",&x, &y) != EOF) {
      for (j = 0; j < height; j++){
	  for (i = 0; i < width; i++) {
	      if ((i == x) && (j == y))
		board[i][j] = '@';
	    }
	} 
    }
  printBoard(board);
}


/* given cell coordinate x or y, returns appropriate value if out of bounds */
int check(int type, int val) {
  if (type == 0) {
      if (val == width)
	return 0;
      else if (val == -1)
	return width - 1;
      else if (val > width)
	return val % (width - 1);
      else
	return val;
    }
  else {
      if (val == height)
        return 0;
      else if (val == -1)
        return height - 1;
      else if (val > height)
        return val % (height - 1);
      else
        return val;
    }
}


/* given cell, return 1 if live, 0 if dead */
int live(char cell) {
  if (cell == '@')
    return 1;
  else
    return 0;
}


/* given current board, make next board position */
void nextBoard(char** thisB, char** nextB) {
  int i;
  int j;
  int counter; // count of live cells as board updates
  int neighbors; // count of living neighbors for one cell
 
 /* for every cell */
  for (j = 0; j < height; j++) {
      for (i = 0; i < width; i++) {
	  /* reset neighbor count */
	  neighbors = 0;

	  /* count live neighbot cells */
	  neighbors = live(thisB[check(0, i - 1)][check(1, j + 1)]) +
	    live(thisB[check(0, i)][check(1, j + 1)]) +
	    live(thisB[check(0, i + 1)][check(1, j + 1)]) +
	    live(thisB[check(0, i - 1)][check(1, j)]) +
	    live(thisB[check(0, i + 1)][check(1, j)]) +
	    live(thisB[check(0, i - 1)][check(1, j - 1)]) +
	    live(thisB[check(0, i)][check(1, j - 1)]) +
	    live(thisB[check(0, i + 1)][check(1, j - 1)]);

	  /* determines state of the new cell */
	  if ((neighbors == 3) || ((neighbors ==2) && (thisB[i][j] == '@'))) {
	      nextB[i][j] = '@';
	      counter++;
	    }
	  else
	  nextB[i][j] = '.';
	}
    }
  /* if there are live cells, print board */
  if (counter != 0)
    printBoard(nextB);
}


/* gets options and */
int main(int argc, char** argv) {
  int c;
  int i;

  opterr = 0;

  /* gets options */
  while ((c = getopt(argc, argv, "t:h:pw:")) != -1) {
      if (optarg < 0) {
	  fprintf (stderr, "Option -%c requires", optopt);
	  fprintf (stderr, " non-negative argument.\n");
	}
      switch (c) {
        case 't':
	  turns = atoi(optarg);
	  break;
        case 'w':
	  width = atoi(optarg);
          break;
	case 'h':
	  height = atoi(optarg);
	  break;
	case 'p':
	  pause1 = 1;
	  break;
        case '?':
          if (optopt == 'c')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
            fprintf (stderr,
                     "Unknown option character `\\x%x'.\n",optopt);
          return 1;
	}
    }
  /* allocate space for two boards, one for current position, one for next */
  char** board1 = (char**) malloc(width * sizeof(char*));
  for (i = 0; i < width; i++)
      board1[i] = (char*) malloc(height * sizeof(char));
  char** board2 = (char**) malloc(width * sizeof(char*));
  for (i = 0; i < width; i++)
      board2[i]= (char*) malloc(height * sizeof(char));
  /* print initial state */
  getBoard(board1);
  /* print turns */
  for (i = 1; i <= turns; i++) {
      if (pause1 != 0) {
	fflush(stdout);
	sleep(1);
      }
      if ((i % 2) != 0)
	nextBoard(board1, board2);
      else
	nextBoard(board2, board1);
    }
}
