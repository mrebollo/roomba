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
int sim_should_stop = 0;

void sim_request_stop(void){
  sim_should_stop = 1;
}

void sim_log_tick(int action){
  if(timer < config.exec_time){
    hist[timer].x = rob->x;
    hist[timer].y = rob->y;
    hist[timer].heading = rob->heading;
    hist[timer].bumper = rob->bumper;
    hist[timer].infrared = rob->infrared;
    hist[timer].battery = rob->battery;
    if(action != -1) ++timer;
  }
  if(rob->battery < 0.1f)
    sim_request_stop();
}

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
static void _save_stats_wrapper(void){
  float sum = 0.0f;
  for(int i = 0; i < timer; i++) sum += hist[i].battery;
  stats.bat_mean = (timer > 0) ? sum / (float)timer : 0.0f;
  save_stats(&stats);
}

// Stats helpers
void stats_move(int kind){ stats.moves[kind]++; }
void stats_visit_cell(void){ stats.cell_visited++; }
void stats_bump(void){ stats.moves[BUMP]++; }
void stats_clean_action(int before, int after){
  stats.moves[CLEAN]++;
  if(before > 0 && after == 0) stats.dirt_cleaned++;
}
void stats_battery_consume(float amount){ stats.bat_total += amount; }
void stats_set_cell_total(int total){ stats.cell_total = total; }
void stats_decrease_free_cells(int count){ stats.cell_total -= count; }
void stats_rebuild_from_map(const map_t* m){
  int cells = 0;
  int dirt_sum = 0;
  for(int i = 0; i < m->nrow; i++){
    for(int j = 0; j < m->ncol; j++){
      char c = m->cells[i][j];
      if(c != WALL) cells++;
      if(c != WALL && c != EMPTY && c != 'B') dirt_sum += (c - '0');
    }
  }
  stats.cell_total = cells;
  stats.dirt_total = dirt_sum;
}

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
  if(map.name == NULL)
    stats_rebuild_from_map(&map);
}

void run(){
  timer = 0;
  sim_should_stop = 0;
  if(config.on_start)
    config.on_start();
  while(!sim_should_stop && timer < config.exec_time)
    config.exec_beh();
}

// robot API moved to sim_robot.c

int load_map(char *filename){
  int rc = sim_world_load(&map, filename);
  if(rc == 0) stats_rebuild_from_map(&map);
  return rc;
}
