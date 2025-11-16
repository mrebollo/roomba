#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simula_internal.h"

/* battery mean tracking removed; computed at exit by scanning hist[] */

static void save_state(sensor_t *state){
  state->x = rob->x;
  state->y = rob->y;
  state->heading = rob->heading;
  state->bumper = rob->bumper;
  state->infrared = rob->infrared;
  state->battery = rob->battery;
}


static void tick(int action){
  sim_log_tick(action);
}

static void apply_battery(float amount){
  rob->battery -= amount;
  stats_battery_consume(amount);
}

static void update_ifr_at_cell(){
  rob->infrared = sim_world_cell_dirt(&map, rob->y, rob->x);
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
  stats_move(TURN);
}

void rmb_forward(){
  float dy, dx;
  int rx, ry;
  step_vectors(rob->heading, &rx, &ry, &dx, &dy);
  if(sim_world_is_wall(&map, ry, rx)){
    rob->bumper = 1;
    stats_bump();
    tick(-1);
    apply_battery(COST_BUMP);
    return;
  }
  r.x += dx;
  r.y += dy;
  rob->x = rx;
  rob->y = ry;
  update_ifr_at_cell();
  stats_move(FWD);
  stats_visit_cell();
  tick(rob->infrared);
  apply_battery((fabsf(dy*dx) < 0.0001f) ? COST_MOVE : COST_MOVE_DIAG);
}

void rmb_clean(){
  int dirt = sim_world_cell_dirt(&map, rob->y, rob->x);
  if(dirt > 0){
    int before = dirt;
    dirt = sim_world_clean_cell(&map, rob->y, rob->x);
    apply_battery(COST_CLEAN);
    rob->infrared = dirt;
    stats_clean_action(before, dirt);
  }
  tick(dirt);
}

int rmb_load(){
  if(sim_world_is_base(&map, rob->y, rob->x)){
    rob->battery += 10;
    if(rob->battery > MAXBAT)
      rob->battery = MAXBAT;
    tick(0);
    return 1;
  }
  stats_move(LOAD);
  return 0;
}

sensor_t rmb_state(){
  sensor_t s;
  save_state(&s);
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
  return sim_world_is_base(&map, rob->y, rob->x);
}

/* battery mean now computed at exit from hist[], helpers removed */
