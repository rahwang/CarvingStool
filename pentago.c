#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 6
#define COLS 6

int win = 0;

// print ascii board
void printBoard(char **b) 
{
  int r, c;
  char r_label[ROWS] = {'A', 'B', 'C', 'D', 'E', 'F'};
  char c_label[COLS] = {'0', '1', '2', '3', '4', '5'};
  char *border = "+ - + - + - + + - + - + - +\n";
  printf("\n\n\n");
  
  printf("   ");
  for (c = 0; c < COLS; c++)
    {
      printf("  %c ", c_label[c]);
      if (c == 2) 
	{
	  printf("  ");
	}
    }
  printf("\n");
  for (r = 0; r < ROWS; r++) 
    {
      if (r == 0 || r == 3)
	{
	  printf("   %s", border);
	}
      printf(" %c |", r_label[r]);
      for (c = 0; c < COLS; c++) 
	{
	  printf(" %c |", b[r][c]);
	  if (c == 2) 
	    {
	      printf(" |");
	    }
	  if (c == 5)
	    {
	      printf("\n");
	    }
	}
      printf("   %s", border);
    }
  printf("\n\n\n");
}

// Deletes newline char when using fgets
char *fg(char *buf)
{
  if (fgets(buf, sizeof(buf), stdin) != 0)
    {
        size_t len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n')
            buf[len-1] = '\0';
        return buf;
    }
    return 0;
}

// Check if move is valid. Update board if valid, return 1 if not.
int checkM(char *m, int player, char **board)
{
  int r, c;
  char symbol = (player == 1) ? 'X' : 'O';

  if (strlen(m) == 2)
    {
      r = m[0] - 'A';
      c = m[1] - '0';
      if (((r >= 0) && (r <= 5)) &&
	  ((r >= 0) && (c <= 5)) &&
	  (board[r][c] == ' ')) 
	{
	  board[r][c] = symbol;
	  return 0;
	}
    }
  printf("\tInvalid move. Try again.\n");
  return 1;
}

void rotate(int quad, int dir, char** board)
{
  int r_start, c_start, i, j, r, c;
  char tmp[3][3];
  // Get quadrant boundaries.
  r_start = ((quad == 1) || (quad == 2)) ? 0 : 3;
  c_start = ((quad == 1) || (quad == 3)) ? 0 : 3;
  
  // Copy quadrant state into tmp.
  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
	{
	  tmp[i][j] = board [i+r_start][j+c_start];
	}
    }
  
  // Update board with rotation.
  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
	{
	  r = j;
	  c = i;
	  if (dir == 2) 
	    {
	      c -=2;
	      c = (c > 0) ? c : 0-c;
	    }
	  else 
	    {
	      r -= 2;
	      r = (r > 0) ? r : 0-r;
	    } 
	  board[r_start+i][c_start+j] = tmp[r][c];
	}
    }
}

// Check if rotation is valid. Update board if valid, return 1 if not.
int checkR(char *rot, int player, char **board)
{
  // Remember: 1 = CW, 2 = CCW. 
  int quad, dir;
  if (strlen(rot) == 3)
    {
      quad = rot[0] - '0';
      dir = rot[2] - '0';
      if (((quad >= 1) && (quad <= 4)) &&
	  ((dir == 1) || (dir == 2))) 
	{
	  rotate(quad, dir, board);
	  return 0;
	}
    }
  else
    {
      printf("\tInvalid move. Try again.\n");
      return 1;
    }
}

void printbits(long long int v) {
   for (; v; v >>= 1) putchar('0' + (v & 1));
   printf("\n");
}

int checkWin(int player, char **board)
{
  int i, j;
  long long int h, v, d1, d2;
  long long int b = 0;
  char symbol = (player == 1) ? 'X' : 'O';

  // Make bitboard
  for (i = 0; i < ROWS; i++)
    {
      for (j = 0; j < COLS; j++)
	{
	  if (board[i][j] == symbol)
	    {
	      b += 1;
	      //printf("1");
	    }
	  else
	    {
	      ;//printf("0");
	    }
	  b = b << 1;
	}
      //printf("\n");
    }

  b = b >> 1;
  //printf("\n%llu\n", b);
  //printbits(b);
  h = b;
  v = b;
  d1 = b;
  d2 = b;

  // Check verticals
  v = ((((b & (b >> 6)) & (b >> 12)) & (b >> 18)) & ( b >> 24));

  // Check horizontals
  h = ((((b & (b >> 1)) & (b >> 2)) & (b >> 3)) & ( b >> 4));
  h = h & 51130563;

  // Check / diagonals
  d1 = ((((b & (b >> 5)) & (b >> 10)) & (b >> 15)) & ( b >> 20));
  d1 = d1 & 3120;

  // Check \ diagonals
  d2 = ((((b & (b >> 7)) & (b >> 14)) & (b >> 21)) & ( b >> 28));
  d2 = d2 & 195;

  if (v || h || d1 || d2)
    {
      printf("VICTORY! Player %i wins!\n\n\n", player);
      exit(0);
    }
  return 0;
}
  


void takeTurn(int player, char **board) 
{
  int r, c;
  char i1[50];
  char i2[50];
  char symbol;
  
  //Do move.
  printf("It's Player %i's turn.\n\n\tPlay where?\n\tAnswer in format 'L#': ", player);
  fg(i1);
  // Repeat prompt if invalid.
  while (checkM(i1, player, board) != 0) 
    {
      memset(i1, '\0', sizeof(i1));
      printf("\n\tPlay where?\n\tAnswer in format 'L#': ");
      fg(i1);
    }
  
  // Do rotation.
  printf("\n\tRotate which quadrant (1-4)? Which way (1) CW or (2) CCW?\n\tAnswer in format '# #': ");
  fg(i2);
  // Repeat prompt if invalid.
  while (checkR(i2, player, board) != 0)
    {
      memset(i2, '\0', sizeof(i2));
      printf("\n\tRotate which quadrant (1-4)? Which way (1) CW or (2) CCW?\n\tAnswer in format '# #': ");
      fg(i2);
    }

  // Print moves made, then board.
  fflush(stdout);
  printf("\nPlayer %i, played at %s. Turned quadrant %c ", player, i1, i2[0]);
  if (i2[2] == '1')
    {
      printf("clockwise.\n");
    }
  else 
    {
      printf("counter-clockwise.\n");
    }
  printBoard(board);
  checkWin(1, board);
  checkWin(2, board);
}
 
int main() 
{
  int r, c;  
  // Allocate board
  char** board = (char **)malloc(ROWS * sizeof(char*));
  for (r = 0; r < ROWS; r++) 
    {
      board[r] = (char*)malloc(COLS * sizeof(char));
    }

  // Initialize board
  for (r = 0; r < ROWS; r++) 
    {
      for (c = 0; c < COLS; c++) 
	{
	  board[r][c] = ' ';
	}
    }

  printBoard(board);

  while(1)
    {
      takeTurn(1, board);
      takeTurn(2, board);
    }
}
