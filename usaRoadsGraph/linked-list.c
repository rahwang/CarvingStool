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

