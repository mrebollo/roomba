#ifndef SIMULA_ROBOT_H
#define SIMULA_ROBOT_H
#include "simula_types.h"

int rmb_awake(int *x, int *y);
void rmb_turn(float alpha);
void rmb_forward();
void rmb_clean();
int rmb_load();
sensor_t rmb_state();
int rmb_bumper();
int rmb_ifr();
float rmb_battery();
int rmb_at_base();

#endif // SIMULA_ROBOT_H
