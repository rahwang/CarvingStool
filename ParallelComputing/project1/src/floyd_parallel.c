/* Rachel Hwang            */
/* Winter 2014             */
/* Parallel Computing      */
/* Project 1               */

#include <stdlib.h>
#include <stdio.h>
#include "stopwatch.h"
#include <pthread.h>


// Declare mutex 
pthread_mutex_t c_lock;
// Declare barriers
pthread_barrier_t b1, b2;
// Declare counter
int counter = 0;


// Thread argument struct for thread function
typedef struct thr_data_t {
  int tid;
  int *matrix;
  int n;
} thr_data_t;


// Thread function: complete a row of the Floyd-Warshall algorithm
void *thr_row(void *arg) {
  thr_data_t *data = (thr_data_t *)arg;
  int *a = data->matrix;
  int n = data->n;
  int i, j, k;
  i = j = k = 0;

  // printf("THREAD ID %d\n", data->tid);
  while (1) {
    // get next row from counter
    pthread_mutex_lock(&c_lock);
    i = counter;
    counter++;
    pthread_mutex_unlock(&c_lock);

    // printf("%d   %d\n", data->tid, i);

    if (i < n) {
      if (i != k) {
	for (j = 0; j < n; j++) {
	  if (j != k) {
	    if ((a[i*n+k] + a[k*n+j]) < a[i*n+j])
	      a[i*n + j] = a[i*n+k] + a[k*n+j];
	  }
	}
      }
    } else {
      k++;
      // printf("%d Waiting for k increment\n", data->tid);
      pthread_barrier_wait(&b1);
      pthread_barrier_wait(&b2);
      if (k == n) {
	  pthread_exit(NULL);
      }
    } 
  } 
}


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


// Read in matrix values from specified file
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
  int i, k, n, read, rc;
  StopWatch_t watch;
  
  if (!argv[1])
    printf("Error: no input file name argument given\n");
  if (!argv[2])
    printf("Error: no thread number argument given\n");

  FILE *src = fopen(argv[1], "r");

  // get n value
  read = fscanf(src, "%d\n", &n);
  if (!read)
    printf("Error: no values read\n");

  // Create array of threads
  int num = atoi(argv[2]);
  pthread_t thr[num];
  // Create a thread argument array
  thr_data_t thr_data[num];
  
  // allocate space for the matrix
  int *a = (int *)malloc(sizeof(int)*n*n);
  
  // Read values into adjacency matrix
  readm(a, n, src);
  
  // Pretty print a matrix for testing.
  printf("\nInput values:\n");
  pprint(a, n);

  // Start timing
  startTimer(&watch);

  // Initialize mutex
  pthread_mutex_init(&c_lock, NULL);


  // Initialize barriers
  pthread_barrier_init(&b1, NULL, num+1);
  pthread_barrier_init(&b2, NULL, num+1);

  // Create threads
  for (i = 0; i < num; i++) {
    thr_data[i].tid = i;
    thr_data[i].n = n;
    thr_data[i].matrix = a;
    if ((rc = pthread_create(&thr[i], NULL, thr_row, &thr_data[i]))) {
      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      return EXIT_FAILURE;
    }
  }

  // Floyd-Warshall Algorithm
  k = 0;
  while(k < n) {
    pthread_barrier_wait(&b1);
    k++;
    //pthread_mutex_lock(&c_lock);
    counter = 0;
    //pthread_mutex_unlock(&c_lock);
    pthread_barrier_wait(&b2);
  }

  // Join threads
  for (i = 0; i < num; i++) {
    pthread_join(thr[i], NULL);
  }
  // Destroy mutex
  pthread_mutex_destroy(&c_lock);

  // Stop timing
  stopTimer(&watch);

  // Pretty print a matrix for testing.
  printf("\nOutput values:\n");
  pprint(a, n);

  // Elapsed time
  printf("n = %d\nruntime = %f\n\n", n, getElapsedTime(&watch));

  // Output matrix to file
  writem(a, n, "p_output.txt");

  return 0;
}

