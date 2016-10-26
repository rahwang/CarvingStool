#ifndef _TYPE_H
#define _TYPE_H

#include <stdlib.h>
#include <stdio.h>

struct city_struct {
  int index;
  char *name;
};

struct road_struct {
  char *name;
  int cost;
};

typedef struct city_struct *city;
typedef struct road_struct *road;

city city_new(char *name, int n);
road road_new(char *name, int cost);

#endif /* _TYPE_H */
