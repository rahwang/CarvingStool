#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include "type.h"

/* linked lists of cities */

struct string_list_node {
  city c;
  struct string_list_node *next;
};

typedef struct string_list_node sll; /* "sll" for "string linked list" */

/* By convention, the empty list is NULL. */

/* sll_cons : (city, sll*) -> sll* */
/* build new list with given city at the head */
sll *sll_cons(city c, sll *ss);

/* sll_length : sll* -> int */
/* return the number of cities in the given list */
int sll_length(sll *ss);

/* sll_member : (sll*, char *name) -> int */
/* test membership of given city in given list */
/* use strcmp to compare names */
int sll_member(sll *ss, char *name);

/* sll_show : sll* -> <void> */
/* print a representation of the linked list to stdout */
void sll_show(sll *ss);

#endif /* _LINKED_LIST_H */
