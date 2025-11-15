#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include "simula.h"
#include "simula_internal.h"
map_t map = {0};
robot_t r;
sensor_t hist[WORLDSIZE*WORLDSIZE];
config_t config;
int timer;
struct sensor *rob = (struct sensor*)&r;
struct _stat stats = {0};
// robot helpers moved to sim_robot.c

// Visual helpers moved to sim_visual.c

// Base/world helpers now in sim_world.c

// Status/HUD moved to sim_visual.c

// IO moved to sim_io.c

// save_map in sim_world.c

// world helpers in sim_world.c

// generate_map in sim_world.c

// load_map in sim_world.c
// Visualization moved to sim_visual.c

static void _save_log_wrapper(void){ save_log(hist, timer); }
static void _save_stats_wrapper(void){ stats.bat_mean = sim_robot_battery_mean(); save_stats(&stats); }

void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time){
  float density;
  srand(time(0));
  config.on_start = start;
  config.exec_beh = beh;
  config.on_stop = stop;
  config.exec_time = exec_time > 0 && exec_time < WORLDSIZE*WORLDSIZE ? exec_time : WORLDSIZE*WORLDSIZE;
#ifndef COMPETITION_MODE
  if(stop != NULL) atexit(stop);
#endif
  atexit(_save_log_wrapper);
  atexit(_save_stats_wrapper);
  density = rand()/(float)RAND_MAX * 0.05f;
  if(map.name == NULL)
    sim_world_generate(&map, WORLDSIZE, WORLDSIZE, 100, density);
}

void run(){
  timer = 0;
  if(config.on_start)
    config.on_start();
  for(;;)
    config.exec_beh();
}

// robot API moved to sim_robot.c

int load_map(char *filename){
  return sim_world_load(&map, filename);
}
