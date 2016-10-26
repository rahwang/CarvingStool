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
/* Rachel Hwang, rah1 */
/* CMSC152, Winter 2012 */
/* lab 6 */

#include "linked-list.h"

/* By convention, the empty list is NULL. */

/* sll_cons : (city, sll*) -> sll* */
/* build new list with given city at the head */
sll *sll_cons(city c, sll *ss) {
  sll *list = (sll *) malloc(sizeof(sll));
  list->c = c;
  list->next = ss;
  return list;
}

/* sll_length : sll* -> int */
/* return the number of cities in the given list */
int sll_length(sll *ss) {
  int counter = 0;
  sll *tmp = ss;
  while(tmp) {
    counter++;
    tmp = tmp->next;
  }
  return counter; 
}

/* sll_member : (sll*, char *) -> int */
/* test membership of given city in given list, if present return index */
/* use strcmp to compare names */
/* -1 indicated failure */
int sll_member(sll *ss, char *name) {
  sll *tmp = ss;
  while(tmp) {
    if (!strcmp(tmp->c->name, name))
      return tmp->c->index;
    else
      tmp = tmp->next;
  }
  return -1;
}

/* sll_show : sll* -> <void> */
/* print a representation of the linked list to stdout */
void sll_show(sll *ss) {
  sll *tmp = ss;
  int i;
  int sz = sll_length(ss);
  char c;
  while (tmp) {
    printf("\"");
    for (i = 0; (c = tmp->c->name[i]) != '\0'; i++)
      printf("%c", c);
    printf("\" -> ");
    tmp = tmp->next;
  }
  printf(".\n"); 
}

/* tests
int main() {
  sll *test =  sll_cons("Hello", NULL);
  test = sll_cons("World", test);
  printf("length = %i\nmember = %i\n", sll_length(test), sll_member(test, "Hello"));
  sll_show(test);
  return 1;
}
*/

/* Rachel Hwang, rah1 */
/* CMSC 152, Winter 2012 */
/* hw6 */

#include <string.h>
#include <ctype.h>

#include "hash.h"

#define NAME_LEN 20 /* default max name length */
#define ARRAY_DEFAULT_SIZE 5 /* default size array */
#define HASH_DEFAULT_SIZE 5  /* default hash table size */
#define MAX_LINE_LEN 256 /* we don't expect very long lines */

/* Dear grader, */
/* Sorry this is so long it is 4:55 and I have no time left to condense :( */

void err_table_size(int sz)
{
  fprintf(stderr,"illegal table sz %d; table sz must be positive\n", sz);
  exit(1);
}

/* Allocate a string on the heap and fill it with '\0' chars. */
char *alloc_str(int sz)
{
  int i;
  char *s = (char *)malloc(sz*sizeof(char));
  /* Note: it's good practice, when allocating space for a string,          
   * to zero out the string's characters.                                   
   */
  for (i=0; i<sz; i++)
    s[i] = '\0';
  return s;
}

/* trim_newline replaces the newline at the end of the string (if there     
 *   is one) with '\0', and then calls strdup on the result,                
 *   returning a pointer to the duplicated string.                          
 * The resulting string has no extra characters at the end. */
char *trim_newline(char *s)
{
  int n = strlen(s);
  if (s[n-1]=='\n')
    s[n-1]='\0';
  return strdup(s);
}

/* builds graph of cities and roads (vertices and edges) from given file 
   prints all direct connections, and neightbor information for all cities */
int main() {
  int i;
  int j;
  char *c = (char *) malloc(sizeof(char) * 2);
  c[0] = 'X';
  c[1] = '\0';
  /* counters for num of cities and roads */
  int c_counter = 0;
  int r_counter = 0;

  /* storage variables */
  char *from;
  int from_index;
  char *to;
  int to_index;
  char *rname;
  char *cost;
  char * direction;
  int comma_counter = 0;
  int from_counter = 0;
  int to_counter = 0;
  int direction_counter = 0;
  int cost_counter = 0;
  int rname_counter = 0;

  /* allocate space for strings that store edge data */
  from = (char *) malloc(sizeof(char) * NAME_LEN);
  to = (char *) malloc(sizeof(char) * NAME_LEN);
  rname = (char *) malloc(sizeof(char) * NAME_LEN);
  cost = (char *) malloc(sizeof(char) * NAME_LEN);
  direction = (char *) malloc(sizeof(char) * 4);

  /* stores whether we are in the cites (1) or roads (0) portion of file */
  int read_cities = 1;
  int sz = HASH_DEFAULT_SIZE;
  int sz2 = ARRAY_DEFAULT_SIZE;

  /* build a new empty hash table to store cities */
  htbl *tbl = htbl_init(sz);

  /* build a new array to store cities, initialize to NULL */
  char * arr[sz2];
  for (i = 0; i < sz2; i++)
    arr[i] = NULL;
 
 /* build adjacency matrix, intialize to empty road */
  road  matrix[sz2][sz2];
  for (i = 0; i < sz2; i++) {
    for (j = 0; j < sz2; j++)
      matrix[i][j] =  road_new("name", -1);
  } 

  /* read a line at a time from stdin, create structs to store data */
  while (!feof(stdin)) {
    char *s = alloc_str(MAX_LINE_LEN);
    char *t;
    fgets(s,MAX_LINE_LEN,stdin);
    t = trim_newline(s);
    free(s);

    /* read in data, build data structs */
    if (strcmp("", t)) {
      if (!strcmp("#edges", t))
	read_cities = 0;
      else if (read_cities == 1) {
	if (strcmp("#vertices", t)) {
	  arr[c_counter] = t;
	  int waa = htbl_add(tbl, city_new(t, c_counter));
	  c_counter++;
	}
      }
      /* get road data adn to/from cities */
      else if (read_cities == 0) {
	for (i = 0; i < strlen(t); i++) {
	  c[0] = t[i];
	  if ((c[0] == '=') || (c[0] == '<') || (c[0] == '>')) {
	    direction[direction_counter] = c[0];
	    direction_counter++;
	    if (c[0] == '=')
	      comma_counter++;
	  }
	  else if (c[0] == ',')
	    comma_counter++;
	  else if (comma_counter == 0) {
	    from[from_counter] = c[0];
	    from_counter++;
	  }
	  else if (comma_counter == 1) { 
	    to[to_counter] = c[0];
	    to_counter++;
	  }
	  else if (comma_counter == 2) {
	    rname[rname_counter] = c[0];
	    rname_counter++;
	  }
	  else { 
	    cost[cost_counter] = c[0];
	    cost_counter++;
	  }
	}

	to[to_counter] = '\0';
	rname[rname_counter] = '\0';
	cost[cost_counter] = '\0';
	direction[direction_counter] = '\0';
	from[from_counter] = '\0';

	from_index = htbl_member(tbl, from);
	to_index = htbl_member(tbl, to);
	if ((from_index == -1) || (to_index == -1)) {
	  printf("error: invalid city entry");
	  exit(-1);
	}
	else {
	  /* parse direction, and add road info to matrix */
	  road nroad = road_new(rname, atoi(cost));
	  if (!strcmp(direction, "=>")) {
	    matrix[from_index][to_index] = nroad;
	    r_counter++;
	  }
	  else if (!strcmp(direction, "<=")) {
	    matrix[to_index][from_index] = nroad;
	    r_counter++;
	  }
	  else if (!strcmp(direction, "<=>")) {
	    matrix[from_index][to_index] = nroad;
	    matrix[to_index][from_index] = nroad;
	    r_counter++;
	  }
	}
      }
    }

    /* flush values */
    comma_counter = 0;
    rname_counter = 0;
    direction_counter = 0;
    from_counter = 0;
    to_counter = 0;
    cost_counter = 0;
  }

  /* and FINALLY, print values */
  printf("Built graph with %i vertices and %i edges.\n", c_counter, r_counter);
  printf("***\nconnected?\n");
  for (i = 0; i < sz2; i++) {
    for (j = 0; j < sz2; j++) {
      if ((arr[i] != NULL) && (arr[j] != NULL) && (i != j)) {
	printf("%s to %s: ", arr[i], arr[j]);
	if ((matrix[i][j])->cost != -1)
	  printf("yes (%s, %i)\n", (matrix[i][j])->name, (matrix[i][j])->cost);
	else
	  printf("no\n");
      }
    }
  }
  printf("***\nneighbors:\n");
  for (i = 0; i < sz2; i++) {
    if (arr[i] != NULL) {
      printf("%s: ", arr[i]);
      for (j = 0; j < sz2; j++) {
	if (((matrix[i][j])->cost != -1) || ((matrix[j][i])->cost != -1))
	  printf("%s ", arr[j]);
      } 
      printf("\n");
    }
  }
  return 0; 
}

#include <stdio.h>
#include <stdlib.h>

#include "type.h"

/* given name and index, build new city */ 
city city_new(char *cname, int n) {
  city c = (city) malloc(sizeof(struct city_struct));
  c->index = n;
  c->name = cname;
  return c;
}

/* given road name and cost, build new road */ 
road road_new(char *rname, int rcost) {
  road r = (road) malloc(sizeof(struct road_struct));
  r->cost = rcost;
  r->name = rname;
  return r;
}


/* #grader  */
/* data structure design    4/ 4 */
/* parsing                  4/ 4 */
/* functionality (it works) 4/ 4 */
/* Makefile                 1/ 1 */
/* svn                      1/ 1 */
/* style                    5/ 6 */
/* Have a more concise main; have a separate parsing function. */

/* #total                  19/20 */
