#ifndef SIMULA_TYPES_H
#define SIMULA_TYPES_H

#include <stdio.h>

#define WORLDSIZE 50
#define WALL '#'
#define UNKNOWN '.'
#define EMPTY ' '
#define MAXDIRT 5
#define MAXBAT 1000
#define PROGBARLEN 50

// Forward declarations
struct sensor;

typedef struct _dirt{
  int x, y;
  int depth;
} dirt_t;

typedef struct _map{
    char patch[WORLDSIZE][WORLDSIZE];
    int nrow, ncol;
    dirt_t dirt[WORLDSIZE*WORLDSIZE];
    int ndirt;
    char *name;
    int bx, by;
} map_t;

typedef struct _config{
    void (*on_start)();
    void (*exec_beh)();
    void (*on_stop)();
    FILE *output;
    int exec_time;
} config_t;

typedef struct _robot{
  struct sensor *s;
  float x, y;
} robot_t;

#endif // SIMULA_TYPES_H
