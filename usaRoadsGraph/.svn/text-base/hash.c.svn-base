/* Rachel Hwang, rah1 */
/* CMSC152, Winter 2012 */
/* rah1 */

#include "hash.h"

/* hash : char -> unsigned int */
/* compute hash code for given string */
unsigned long hash(char *s) {
  unsigned long res = 17;
  int i;
  char c;
  while((c = s[i]) != '\0') {
    res = res * 37 + c;
    i++;
  }
  //  printf("%lu", res); 
  return res;
}

/* htbl_init : int -> htbl* */
/* allocate space for a new hash table of given size */
/* all buckets must initially contain the empty list */
htbl *htbl_init(int sz) {
  htbl *h = (htbl *) malloc(sizeof(htbl));
  sll **t = (sll **) malloc(sz * sizeof(sll));
  h->n_buckets = sz;
  h->buckets = t;
  int i;
  for (i = 0; i < sz; i++)
      t[i] = NULL;
  return h;
}

/* htbl_add : (htbl*, city) -> int */
/* add city c to hast table t */
/* return the number of cities in c's bucket _after_ inserting it */
int htbl_add(htbl *t, city c) {
  int i;
  char *s = c->name;
  //  printf("%s", s);
  int index = hash(s) % t->n_buckets;
  sll *ss = (t->buckets)[index];
  (t->buckets)[index] = sll_cons(c, ss);
  int num = sll_length((t->buckets)[index]);
  return num + 1;
} 

/* htbl_member : (htbl*, char *) -> int */
/* test membership of given city in given table from given name */
/* the integer returned is a (pseudo) boolean */
int htbl_member(htbl *t, char *name) {
  int i;
  int index;
  sll** bucket = t->buckets;
  for (i = 0; i < t->n_buckets; i++) {
    index = sll_member(bucket[i], name);
    if (index != -1)
      return index;
  }
  return -1;
}

/* htbl_show : htbl* -> <void> */
/* print a represntation of the hash table to stdout */
void htbl_show(htbl *t) {
  int i;
  sll** bucket = t->buckets;
  for (i = 0; i < t->n_buckets; i++) {
    printf("%i: ", i);
    sll_show(bucket[i]);
  }
}

/* for testing
int main() {
  htbl *test = htbl_init(1);
  printf("htbl n_buckets : %i\n", test->n_buckets);
  int x =  htbl_add(test, "0");
  printf("hash value for 0: %d\n", (int)hash("0"));
  printf("member? : %i\n", htbl_member(test, "Hello"));
  htbl_show(test);
  return 1;
}
*/
