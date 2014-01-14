/* Optimized, vectorized merge sort using bubble sort at the end. 
Takes a single argument, "div", an int specifying the number of 
vectors lists to use. Optimal div value generally around n/16 
compile with gcc -msse4.1 vsort.c -lrt -lm */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <math.h>

#define SMALL 1000         // 1K
#define MEDIUM  10000      // 10K
#define LARGE  100000    // 100k
#define XLARGE  1000000  // 1M
#define BILLION 1E9
#define SIZE 1048576 // 2^20

// Print a list of vectors. For testing.
void print_v(__m128i *a, int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("[%i %i %i %i]\n", _mm_extract_epi32(a[i], 0), _mm_extract_epi32(a[i], 1),
	   _mm_extract_epi32(a[i], 2), _mm_extract_epi32(a[i], 3));
  }
  printf("\n\n");
}

// Given two sorted lists, merge into one sorted list of given len
int *m(int *left, int *right, int len) {
  int i, l, r;
  l = r = i = 0;
  
  int *res = (int *)malloc(sizeof(int)*len);
  while (l < len/2 && r < len-len/2) {
    if (left[l] > right[r])
      res[i++] = right[r++];
    else
      res[i++] = left[l++];
  }
  while (l < len/2)
    res[i++] = left[l++];
  while (r < (len-len/2))
    res[i++] = right[r++];
  return res;
}

int main(int argc, char *argv[])
{
  int n = 65536;
  int *array, i, j, c, d, swap, idx;
  int *res;
  array = (int *)malloc(sizeof(int)*n);
  
  // Fill input array.
  for (c = 0; c < n; c++){
    array[c] = (n-c) % 17; 
    //printf("%i ", array[c]);
  }
  
  //printf("Sorting %d integers\n You should try to time this part. \n", n);
  
  // Begin time
  struct timespec begin, end;
  clock_gettime(CLOCK_REALTIME, &begin);
  
  __m128i *vlist = (__m128i *)malloc(sizeof(__m128i)*n/4);
  
  int divs = atoi(argv[1]);
  int len = n/(4*divs);

  // Allocate space for the lists of sorted vector elements.
  int *vecs[divs*4];
  for (i = 0; i < divs*4; i++) {
      vecs[i] = (int *)malloc(sizeof(int)*len);
  }
  
  // Fill vector list with chunks of 4 elements from input array
  for (i = 0; i < n/4; i++) {
    vlist[i] = _mm_load_si128(array+i*4);
  }
  
  __m128i tmp;
  // Bubble sort divs number of vector lists
  for (i = 0; i < divs; i++) {
    for (c = 0 ; c < ( len - 1 ); c++)  {
      for (d = 0 ; d < len - c - 1; d++) {
	// Swap two vectors for their min and max, in that order
	idx = (len*i) + d;
	tmp = _mm_max_epi32(vlist[idx], vlist[idx+1]);
	vlist[idx]   = _mm_min_epi32(vlist[idx], vlist[idx+1]);
	vlist[idx+1] = tmp;
      }
      // Fill lists
      idx = (len*i) + d;
      vecs[i*4][d] = _mm_extract_epi32(vlist[idx], 0);
      vecs[i*4 + 1][d] = _mm_extract_epi32(vlist[idx], 1);
      vecs[i*4 + 2][d] = _mm_extract_epi32(vlist[idx], 2);
      vecs[i*4 + 3][d] = _mm_extract_epi32(vlist[idx], 3);
    }
    vecs[i*4][0] = _mm_extract_epi32(vlist[i*len], 0);
    vecs[i*4 + 1][0] = _mm_extract_epi32(vlist[i*len], 1);
    vecs[i*4 + 2][0] = _mm_extract_epi32(vlist[i*len], 2);
    vecs[i*4 + 3][0] = _mm_extract_epi32(vlist[i*len], 3);
  }

  // Merge all the sorted lists, two at a time.
  for (i = log2(divs*4), c=2; i > 0; i--, c *= 2) {
    for (j = 0; j < divs*4; j += c) {
      vecs[j] = m(vecs[j], vecs[j+c/2], len*c);
    }
  }
  
  res = vecs[0];
  
  clock_gettime(CLOCK_REALTIME, &end);
  double accum = (end.tv_sec-begin.tv_sec)+(end.tv_nsec-begin.tv_nsec)/BILLION;
  printf("%f\n", accum);
  
  // Check if res is actually sorted.
  for ( c = 0 ; c < n-1 ; c++ ) {
    if (res[c] > res[c+1])
      printf("Whoops: error at position %d\n", c);
    else
      ;//printf("#%d     %d\n", c, res[c]);
  }
  return 0;
}


