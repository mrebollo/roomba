#include "simula_robot.h"
#include <math.h>
#include <stdio.h>
#include "simula_types.h"
#include "simula_core.h"

// accessor functions for former global variables
#define map (*get_map())
#define r (*get_robot())
#define hist (get_hist())
#define config (*get_config())
#define timer (*get_timer())
#define stats (*get_stats())
#define rob (*get_rob())

// Robot interface and state
int rmb_awake(int *x, int *y){
  printf("Awaking...\n");
  printf("Map: %s\n", map.name);
  if(map.name != NULL){
    rob->head = put_base_at(map.bx, map.by);
    *x = map.bx;
    *y = map.by;
  }   
  else{
    printf("No map loaded\n");
    set_base_at_origin(x, y, &rob->head);
    save_map();
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
  rob->head += alpha;
  if(rob->head < 0)
    rob->head += 2 * M_PI;
  rob->head = fmod(rob->head, 2 * M_PI);
  rob->bumper = 0;
  rob->battery -= 0.1;
}

void rmb_forward(){
    char patch;
    float dy, dx;
    int rx, ry, batt;
    dy = rounda(sin(rob->head));
    dx = rounda(cos(rob->head));
    rx = (int)(r.x + dx);
    ry = (int)(r.y + dy);
    if(map.patch[ry][rx] == WALL){
      rob->bumper = 1;
      batt = 0.5;
      stats.moves[BUMP]++;
      tick(-1);
    }
    else{
      r.x += dx;
      r.y += dy; 
      rob->x = rx;
      rob->y = ry;
      batt = (dy*dx < 0.0001) ? 1 : 1.4;
      if(map.patch[rob->y][rob->x] == EMPTY)
        rob->infrarred = 0;
      else if(map.patch[rob->y][rob->x] != 'B'){
        rob->infrarred = map.patch[rob->y][rob->x] - '0';
      }  
      stats.moves[FWD]++;
      stats.cell_visited++;
      tick(rob->infrarred); 
    } 
    rob->battery -= batt;
    stats.bat_total += batt; 
}

void rmb_clean(){
  int dirt = map.patch[rob->y][rob->x] - '0';
  if (dirt > 0){
    dirt--;
    map.patch[rob->y][rob->x] = dirt + '0';
    rob->battery -= 0.5;
    stats.bat_total += 0.5;
    rob->infrarred = dirt;
    stats.moves[CLE]++;
    if(dirt == 0) stats.dirt_cleaned++;
  }
  tick(dirt);
}

int rmb_load(){
    if(at_base()){
        rob->battery += 10;
        if (rob->battery > MAXBAT) 
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
