#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "simula_internal.h"

static double _bat_sum = 0.0;
static int _bat_samples = 0;

static void save_state(sensor_t *state){
  state->x = rob->x;
  state->y = rob->y;
  state->heading = rob->heading;
  state->bumper = rob->bumper;
  state->infrared = rob->infrared;
  state->battery = rob->battery;
}

static int at_base(){
  return map.cells[rob->y][rob->x] == 'B';
}

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
  return map.cells[y][x] == WALL;
}

static void update_ifr_at_cell(){
  if(map.cells[rob->y][rob->x] == EMPTY)
    rob->infrared = 0;
  else if(map.cells[rob->y][rob->x] != 'B')
    rob->infrared = map.cells[rob->y][rob->x] - '0';
}

static void step_vectors(float heading, int *rx, int *ry, float *dx, float *dy){
  *dy = rounda(sin(heading));
  *dx = rounda(cos(heading));
  *rx = (int)(r.x + *dx);
  *ry = (int)(r.y + *dy);
}

// Public robot API
int rmb_awake(int *x, int *y){
  DEBUG_PRINT("Awaking...\n");
  DEBUG_PRINT("Map: %s\n", map.name);
  if(map.name != NULL){
    if(map.base_x > 0 && map.base_y > 0){
      rob->heading = sim_world_put_base(&map, map.base_x, map.base_y);
      *x = map.base_x;
      *y = map.base_y;
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
  tick(rob->infrared);
  apply_battery((fabsf(dy*dx) < 0.0001f) ? COST_MOVE : COST_MOVE_DIAG);
}

void rmb_clean(){
  int dirt = map.cells[rob->y][rob->x] - '0';
  if(dirt > 0){
    dirt--;
    map.cells[rob->y][rob->x] = dirt + '0';
    apply_battery(COST_CLEAN);
    rob->infrared = dirt;
    stats.moves[CLEAN]++;
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
  return rob->infrared;
}

float rmb_battery(){
  return rob->battery;
}

int rmb_at_base(){
  return at_base();
}

float sim_robot_battery_mean(void){
  return (_bat_samples > 0) ? (float)(_bat_sum / _bat_samples) : 0.0f;
}
