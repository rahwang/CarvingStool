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

