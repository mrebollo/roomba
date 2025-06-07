#include "simula_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simula_types.h"
#include "simula_stats.h"
#include "simula_map.h"

// Provide accessors for global variables
map_t *get_map() { return &map; }
robot_t *get_robot() { return &r; }
sensor_t *get_hist() { return hist; }
config_t *get_config() { return &config; }
int *get_timer() { return &timer; }
struct _stat *get_stats() { return &stats; }
struct sensor **get_rob() { return &rob; }

// Forward declaration for sensor_t
typedef struct sensor sensor_t;

// Core simulation control
void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time) {
    float density;
    srand(time(0));
    config.on_start = start;
    config.exec_beh = beh;
    config.on_stop = stop;
    config.exec_time = exec_time > 0 && exec_time < WORLDSIZE * WORLDSIZE ? exec_time : WORLDSIZE * WORLDSIZE;
    if(stop != NULL)
        atexit(stop);
    atexit(save_log);
    atexit(save_stats);

    //map generation (obstacle density until 5%)
    density = rand() / (float)RAND_MAX * 0.05;
    if(map.name == NULL)
        generate_map(WORLDSIZE, WORLDSIZE, 100, density);
}

void run() {
    timer = 0;
    if(config.on_start != NULL)
        config.on_start();
    for(;;)
        config.exec_beh();
}

void visualize() {
    int t;
    for(t = 0; t < config.exec_time; t++){
        system("clear");
        print_path(hist, t);
        printf("Ctrl-C para salir\n");
        system("sleep 0.1");
    }
}

// Global variables
map_t map = {0};
robot_t r;
sensor_t hist[WORLDSIZE*WORLDSIZE];
config_t config;
int timer;
struct sensor *rob = (struct sensor*)&r;
enum movement {FWD, TURN, BUMP, CLE, LOAD};
struct _stat{
  int cell_total;
  int cell_visited;
  int dirt_total;
  int dirt_cleaned;
  float bat_total;
  float bat_mean;
  int moves[5]; // forward, turn, bump, clean, load
} stats = {0};


