/* Rachel Hwang            */
/* Winter 2014             */
/* Parallel Computing      */
/* Project 1               */

#include <stdlib.h>
#include <stdio.h>
#include "stopwatch.h"
#include <pthread.h>

// Print a matrix
void pprint(int *a, int n) {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j <n; j++) {
      printf("%i ", a[i*n + j]);
    }
    printf("\n");
  }
  printf("\n\n");
}


// Read in matric values from specified file
void readm(int *a, int n, FILE *src) {
  int i;
  int *cur = a;
  int read = fscanf(src, "%d", &i);
  if (!read)
    printf("Error: no data read\n");
  while (!feof(src)) {
    *cur = i;
    cur++;
    read = fscanf(src, "%d", &i);
  }
  fclose(src);
}


// Output matrix to the specified file
void writem(int *a, int n, char *fname) {
  int i, j;
  FILE *dst = fopen(fname, "w");
  fprintf(dst, "Updated adjacency matrix (n = %d)\n", n);
  for (i = 0; i < n; i++) {
    for (j = 0; j <n; j++) {
      fprintf(dst, "%i ", a[i*n + j]);
    }
    fprintf(dst, "\n");
  }
}


int main(int argc, char *argv[])
{
  int i, j, k, n, read;
  StopWatch_t watch;
  
  if (!argv[1])
    printf("Error: no input file name given\n");
  FILE *src = fopen(argv[1], "r");

  // get n value
  read = fscanf(src, "%d\n", &n);
  if (!read)
    printf("Error: no values read\n");
  
  // allocate space for the matrix
  int *a = (int *)malloc(sizeof(int)*n*n);
  
  // Read values into adjacency matrix
  readm(a, n, src);
  
  // Pretty print a matrix for testing.
  printf("\nInput values:\n");
  pprint(a, n);

  // Start timing
  startTimer(&watch);

  // Floyd-Warshall Algorithm
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      if (i != k) {
	for (j = 0; j < n; j++) {
	  if (j != k) {
	    if ((a[i*n+k] + a[k*n+j]) < a[i*n+j])
	      a[i*n + j] = a[i*n+k] + a[k*n+j];
	  }
	}
      }
    }
  } 
  // Stop timing
  stopTimer(&watch);

  // Pretty print a matrix for testing.
  printf("\nOutput values:\n");
  pprint(a, n);

  // Elapsed time
  printf("n = %d\nruntime = %f\n\n", n, getElapsedTime(&watch));

  // Output matrix to file
  writem(a, n, "s_output.txt");

  return 1;
}

