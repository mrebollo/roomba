#pragma once

#include <stdio.h>
#include <math.h>
#include "simula.h"

#ifndef COMPETITION_MODE
  #define DEBUG_PRINT(...) printf(__VA_ARGS__)
  #define ENABLE_VISUALIZATION 1
#else
  #define DEBUG_PRINT(...)
  #define ENABLE_VISUALIZATION 0
#endif

#define WORLDSIZE 50
#define WALL '#'
#define EMPTY ' '
#define MAXDIRT 5
#define M_PI_8 ((M_PI) / 8)

#define MAXBAT 1000
#define PROGBARLEN 50
#define rounda(a) (round((a)*100000) / 100000.0)

// Costes de batería
#define COST_TURN 0.1f
#define COST_MOVE 1.0f
#define COST_MOVE_DIAG 1.4f
#define COST_BUMP 0.5f
#define COST_CLEAN 0.5f

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
  sensor_t s;
  float x, y;
} robot_t;

enum movement {FWD, TURN, BUMP, CLE, LOAD};
struct _stat{
  int cell_total;
  int cell_visited;
  int dirt_total;
  int dirt_cleaned;
  float bat_total;
  float bat_mean;
  int moves[5];
};

// Estado global (definido en simula.c)
extern map_t map;
extern robot_t r;
extern sensor_t hist[WORLDSIZE*WORLDSIZE];
extern config_t config;
extern int timer;
extern struct sensor *rob;
extern struct _stat stats;

// IO (implementado en sim_io.c)
void save_log(const sensor_t *hist, int len);
void save_stats(const struct _stat *st);

// World (implementado en sim_world.c)
int sim_world_generate(map_t* m, int nrow, int ncol, int num_dirty, float nobs);
int sim_world_load(map_t* m, char *filename);
void sim_world_save(const map_t* m);
float sim_world_put_base(map_t* m, int x, int y);
void sim_world_set_base_origin(map_t* m, int *x, int *y, float *h);

// Robot (implementado en sim_robot.c)
float sim_robot_battery_mean(void);
