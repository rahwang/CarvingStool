/* Optimized matrix multiplication. Takes a single argument,
   an int which specifies matrix width. Stick to powers of two. 
   Compile with gcc -msse4.1 vmmult.c -lrt -lm */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <smmintrin.h>

#define TINY 10         // 10x10
#define SMALL  100      // 100x100
#define MEDIUM 1000    // 1000x1000
#define LARGE 5000    // 5000x5000
#define YOURSIZE 100    // define your own
#define BILLION 1E9 

int main(int argc, char *argv[])
{
  int i, j, k;

  int n = atoi(argv[1]);

  int *a = (int *)malloc(sizeof(int)*n*n);
  int *res = (int *)malloc(sizeof(int)*n*n);
  int *b = (int *)malloc(sizeof(int)*n*n);

  //printf("Generating the matrix\n");
 
  for (i = 0 ; i < n; i++) {
    for (j = 0 ; j < n; j++) {
      a[i*n + j] = i+j;
      res[i*n+j] = 0;
    }
  }
 
  //printf("multiplying the matrices\n  You should try to time this part.\n");

  struct timespec begin, end;
  clock_gettime(CLOCK_REALTIME, &begin);

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      b[j+i*n] = a[i+j*n];
    }
  }
  /*
  // Pretty print a matrix for testing.
  for (i = 0; i < n; i++) {
    for (j = 0; j <n; j++) {
      printf("%i ", a[i*n + j]);
    }
    printf("\n");
  }
  printf("\n\n\n");
  */
  __m128i a_v, b_v, mult_v, tmp;
  __m128i sum0 = _mm_setzero_si128();
  __m128i sum1 = _mm_setzero_si128();
  __m128i sum2 = _mm_setzero_si128();
  __m128i sum3 = _mm_setzero_si128();
  __m128i sum4 = _mm_setzero_si128();
  __m128i sum5 = _mm_setzero_si128();
  __m128i sum6 = _mm_setzero_si128();
  __m128i sum7 = _mm_setzero_si128();/*
  __m128i sum8 = _mm_setzero_si128();
  __m128i sum9 = _mm_setzero_si128();
  __m128i sum10 = _mm_setzero_si128();
  __m128i sum11 = _mm_setzero_si128();
  __m128i sum12 = _mm_setzero_si128();
  __m128i sum13 = _mm_setzero_si128();
  __m128i sum14 = _mm_setzero_si128();
  __m128i sum15 = _mm_setzero_si128();*/

  for ( i = 0 ; i < n ; i++ )  {
    for ( j = 0 ; j < n ; j+= 8 ) {
      for ( k = 0 ; k < n ; k+= 4 ) {
	// Get a and b, multiply vectors, add to sum.
	tmp = _mm_load_si128(a + i*n + k);
	sum0 = _mm_add_epi32(sum0, _mm_mullo_epi32(tmp, _mm_load_si128(b+j*n+k))); 
	sum1 = _mm_add_epi32(sum1, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+1)*n+k)));
	sum2 = _mm_add_epi32(sum2, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+2)*n + k)));
	sum3 = _mm_add_epi32(sum3, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+3)*n + k)));
	sum4 = _mm_add_epi32(sum4, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+4)*n + k)));
	sum5 = _mm_add_epi32(sum5, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+5)*n + k)));
	sum6 = _mm_add_epi32(sum6, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+6)*n + k))); 
	sum7 = _mm_add_epi32(sum7, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+7)*n + k))); /*
	sum8 = _mm_add_epi32(sum8, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+8)*n + k)));
	sum9 = _mm_add_epi32(sum9, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+9)*n + k)));
	sum10 = _mm_add_epi32(sum10, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+10)*n + k)));
	sum11 = _mm_add_epi32(sum11, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+11)*n + k)));
	sum12 = _mm_add_epi32(sum12, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+12)*n + k)));
	sum13 = _mm_add_epi32(sum13, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+13)*n + k)));
	sum14 = _mm_add_epi32(sum14, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+14)*n + k)));
	sum15 = _mm_add_epi32(sum15, _mm_mullo_epi32(tmp, _mm_load_si128(b + (j+15)*n + k))); */
      }
      // Sum the sums.
      sum0 = _mm_hadd_epi32(sum0, sum0);
      sum0 = _mm_hadd_epi32(sum0, sum0);
      sum1 = _mm_hadd_epi32(sum1, sum1);
      sum1 = _mm_hadd_epi32(sum1, sum1);
      sum2 = _mm_hadd_epi32(sum2, sum2);
      sum2 = _mm_hadd_epi32(sum2, sum2);
      sum3 = _mm_hadd_epi32(sum3, sum3);
      sum3 = _mm_hadd_epi32(sum3, sum3);
      sum4 = _mm_hadd_epi32(sum4, sum4);
      sum4 = _mm_hadd_epi32(sum4, sum4);
      sum5 = _mm_hadd_epi32(sum5, sum5);
      sum5 = _mm_hadd_epi32(sum5, sum5);
      sum6 = _mm_hadd_epi32(sum6, sum6);
      sum6 = _mm_hadd_epi32(sum6, sum6);
      sum7 = _mm_hadd_epi32(sum7, sum7);
      sum7 = _mm_hadd_epi32(sum7, sum7);/*
      sum8 = _mm_hadd_epi32(sum8, sum8);
      sum8 = _mm_hadd_epi32(sum8, sum8);
      sum9 = _mm_hadd_epi32(sum9, sum9);
      sum9 = _mm_hadd_epi32(sum9, sum9);
      sum10 = _mm_hadd_epi32(sum10, sum10);
      sum10 = _mm_hadd_epi32(sum10, sum10);
      sum11 = _mm_hadd_epi32(sum11, sum11);
      sum11 = _mm_hadd_epi32(sum11, sum11);
      sum12 = _mm_hadd_epi32(sum12, sum12);
      sum12 = _mm_hadd_epi32(sum12, sum12);
      sum13 = _mm_hadd_epi32(sum13, sum13);
      sum13 = _mm_hadd_epi32(sum13, sum13);
      sum14 = _mm_hadd_epi32(sum14, sum14);
      sum14 = _mm_hadd_epi32(sum14, sum14);
      sum15 = _mm_hadd_epi32(sum15, sum15);
      sum15 = _mm_hadd_epi32(sum15, sum15); */ 


      // Extract sum and store in res.
      res[i*n + j] = _mm_extract_epi32(sum0, 0); 
      res[i*n + (j+1)] = _mm_extract_epi32(sum1, 0);
      res[i*n + (j+2)] = _mm_extract_epi32(sum2, 0);
      res[i*n + (j+3)] = _mm_extract_epi32(sum3, 0);
      res[i*n + (j+4)] = _mm_extract_epi32(sum4, 0);
      res[i*n + (j+5)] = _mm_extract_epi32(sum5, 0);
      res[i*n + (j+6)] = _mm_extract_epi32(sum6, 0);
      res[i*n + (j+7)] = _mm_extract_epi32(sum7, 0); /*
      res[i*n + (j+8)] = _mm_extract_epi32(sum8, 0); 
      res[i*n + (j+9)] = _mm_extract_epi32(sum9, 0);
      res[i*n + (j+10)] = _mm_extract_epi32(sum10, 0);
      res[i*n + (j+11)] = _mm_extract_epi32(sum11, 0);
      res[i*n + (j+12)] = _mm_extract_epi32(sum12, 0);
      res[i*n + (j+13)] = _mm_extract_epi32(sum13, 0);
      res[i*n + (j+14)] = _mm_extract_epi32(sum14, 0);
      res[i*n + (j+15)] = _mm_extract_epi32(sum15, 0); */

      // Zero sum vector.
      sum0 = _mm_setzero_si128();
      sum1 = _mm_setzero_si128();
      sum2 = _mm_setzero_si128();
      sum3 = _mm_setzero_si128();
      sum4 = _mm_setzero_si128();
      sum5 = _mm_setzero_si128();
      sum6 = _mm_setzero_si128();
      sum7 = _mm_setzero_si128(); /*
      sum8 = _mm_setzero_si128(); 
      sum9 = _mm_setzero_si128();
      sum10 = _mm_setzero_si128();
      sum11 = _mm_setzero_si128();
      sum12 = _mm_setzero_si128();
      sum13 = _mm_setzero_si128();
      sum14 = _mm_setzero_si128();
      sum15 = _mm_setzero_si128(); */
    }
  }

  clock_gettime(CLOCK_REALTIME, &end);
  double accum = (end.tv_sec-begin.tv_sec)+(end.tv_nsec-begin.tv_nsec)/BILLION;
  printf("%f\n", accum);
 
  return 0;
}
