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


typedef struct _sensor{
    int x, y;       // posición del robot en horizontal (x) y vertical (y)
    float head;     // dirección en la que apunta actualmente el robot
    int bumper;     // bumper presionado o libre (detección de obstáculos)
    int infrarred;  // valor del sensor de infrarrojos (detección de suciedad)
    float battery;  // capacidad de batería disponible
} sensor_t;

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
  //sensor must be in the first position to allow casting to sensor_t
  sensor_t s;
  float x, y;
} robot_t;

#endif // SIMULA_TYPES_H
