#include <stdlib.h>
#include <stdio.h>

// Returns minimum of args
int min(int a,int b){
if (a < b)
  return a;
 else
   return b;
}

int main(int argc, char *argv[])
{
  int i, j, k, n;
  
  FILE *src = fopen(argv[1], "r");

  fscanf(src, "%d", &n);
  
  // allocate space for the matrix
  int *a = (int *)malloc(sizeof(int)*n*n);
  
  // Read values into adjacency matrix
  int *cur = a;
  fscanf(src, "%d", &i);
  while (!feof(src)) {
    *cur = i;
    cur++;
    fscanf(src, "%d", &i);
  }
  fclose(src);
  
  // Pretty print a matrix for testing.
  for (i = 0; i < n; i++) {
    for (j = 0; j <n; j++) {
      printf("%i ", a[i*n + j]);
    }
    printf("\n");
  }
  printf("\n\n\n");

  // Floyd-Warshall Algorithm
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      if (i != k) {
	for (j = 0; j < n; j++) {
	  if (j != k) 
	    a[i*n + j] = min(a[i*n+j], a[i*n+k] + a[k*n+j]);
	}
      }
    }
  } 

  // Output to file
  FILE *dst = fopen("ouput.txt", "w");
  fprintf(dst, "Updated adjacency matrix (n = %d\n", n);
  for (i = 0; i < n; i++) {
    for (j = 0; j <n; j++) {
      fprintf(dst, "%i ", a[i*n + j]);
    }
    fprintf(dst, "\n");
  }
  fprintf(dst, "\n\n\n");

  return 1;
}

