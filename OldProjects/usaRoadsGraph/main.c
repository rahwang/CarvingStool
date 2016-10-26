/* Rachel Hwang, rah1 */
/* CMSC 152, Winter 2012 */
/* hw6 */

#include <string.h>
#include <ctype.h>

#include "hash.h"

#define NAME_LEN 50 /* default max name length */
#define ARRAY_DEFAULT_SIZE 400 /* default size array */
#define HASH_DEFAULT_SIZE 400  /* default hash table size */
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

