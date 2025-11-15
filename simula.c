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
static double _bat_sum = 0.0;
static int _bat_samples = 0;


static void save_state(sensor_t *state){
  state->x = rob->x;
  state->y = rob->y;
  state->heading = rob->heading;
  state->bumper = rob->bumper;
  state->infrarred = rob->infrarred;
  state->battery = rob->battery;
}

static int at_base(){
  return map.patch[rob->y][rob->x] == 'B';
}

// Visual helpers moved to sim_visual.c

// Base/world helpers now in sim_world.c

static void tick(int action){
  assert(timer < config.exec_time);
  save_state(&hist[timer]);
  _bat_sum += rob->battery;
  _bat_samples++;
  if(rob->battery < 0.1 || (action != -1 && ++timer >= config.exec_time))
    exit(0);
}

static void apply_battery(float amount){
  rob->battery -= amount;
  stats.bat_total += amount;
}

static int is_wall_cell(int y, int x){
  return map.patch[y][x] == WALL;
}

static void update_ifr_at_cell(){
  if(map.patch[rob->y][rob->x] == EMPTY)
    rob->infrarred = 0;
  else if(map.patch[rob->y][rob->x] != 'B')
    rob->infrarred = map.patch[rob->y][rob->x] - '0';
}

static void step_vectors(float heading, int *rx, int *ry, float *dx, float *dy){
  *dy = rounda(sin(heading));
  *dx = rounda(cos(heading));
  *rx = (int)(r.x + *dx);
  *ry = (int)(r.y + *dy);
}

// Status/HUD moved to sim_visual.c

// IO moved to sim_io.c

// save_map in sim_world.c

// world helpers in sim_world.c

// generate_map in sim_world.c

// load_map in sim_world.c
// Visualization moved to sim_visual.c

static void _save_log_wrapper(void){ save_log(hist, timer); }
static void _save_stats_wrapper(void){ stats.bat_mean = (_bat_samples>0)? (float)(_bat_sum/_bat_samples) : 0.0f; save_stats(&stats); }

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

int rmb_awake(int *x, int *y){
  DEBUG_PRINT("Awaking...\n");
  DEBUG_PRINT("Map: %s\n", map.name);
  if(map.name != NULL){
    if(map.bx > 0 && map.by > 0){
      rob->heading = sim_world_put_base(&map, map.bx, map.by);
      *x = map.bx;
      *y = map.by;
    } else {
      sim_world_set_base_origin(&map, x, y, &rob->heading);
    }
  } else {
    DEBUG_PRINT("No map loaded\n");
    sim_world_set_base_origin(&map, x, y, &rob->heading);
    sim_world_save(&map);
  }
  rob->x = *x;
  rob->y = *y;
  rob->battery = MAXBAT;
  r.x = *x;
  r.y = *y;
  tick(0);
  return 1;
}

void rmb_turn(float alpha){
  rob->heading += alpha;
  if(rob->heading < 0)
    rob->heading += 2 * M_PI;
  rob->heading = fmod(rob->heading, 2 * M_PI);
  rob->bumper = 0;
  apply_battery(COST_TURN);
  stats.moves[TURN]++;
}

void rmb_forward(){
  float dy, dx;
  int rx, ry;
  step_vectors(rob->heading, &rx, &ry, &dx, &dy);
  if(is_wall_cell(ry, rx)){
    rob->bumper = 1;
    stats.moves[BUMP]++;
    tick(-1);
    apply_battery(COST_BUMP);
    return;
  }
  r.x += dx;
  r.y += dy;
  rob->x = rx;
  rob->y = ry;
  update_ifr_at_cell();
  stats.moves[FWD]++;
  stats.cell_visited++;
  tick(rob->infrarred);
  apply_battery((fabsf(dy*dx) < 0.0001f) ? COST_MOVE : COST_MOVE_DIAG);
}

void rmb_clean(){
  int dirt = map.patch[rob->y][rob->x] - '0';
  if(dirt > 0){
    dirt--;
    map.patch[rob->y][rob->x] = dirt + '0';
    apply_battery(COST_CLEAN);
    rob->infrarred = dirt;
    stats.moves[CLE]++;
    if(dirt == 0) stats.dirt_cleaned++;
  }
  tick(dirt);
}

int rmb_load(){
  if(at_base()){
    rob->battery += 10;
    if(rob->battery > MAXBAT)
      rob->battery = MAXBAT;
    tick(0);
    return 1;
  }
  stats.moves[LOAD]++;
  return 0;
}

sensor_t rmb_state(){
  sensor_t s = r.s;
  return s;
}

int rmb_bumper(){
  return rob->bumper;
}

int rmb_ifr(){
  return rob->infrarred;
}

float rmb_battery(){
  return rob->battery;
}

int rmb_at_base(){
  return at_base();
}

int load_map(char *filename){
  return sim_world_load(&map, filename);
}
