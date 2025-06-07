#ifndef SIMULA_CORE_H
#define SIMULA_CORE_H
#include "simula_types.h"

void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time);
void run();
void visualize();

// accessor functions for former global variables
map_t *get_map();
robot_t *get_robot();
sensor_t *get_hist();
config_t *get_config();
int *get_timer();
struct _stat *get_stats();
struct sensor **get_rob();

#endif // SIMULA_CORE_H
