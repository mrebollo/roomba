#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simula.h"

typedef struct coor{
  int x, y;
} coor_t;

coor_t base;      // para poder acceder a las coordenadas de la base desde cualquier función
int dirx = 1;  // dirección en x (1 o -1) para poder hacer el giro en el sentido correcto


void on_start();
void on_stop();


float rad2deg(float angle){ 
    return angle * 180.0 / M_PI; 
} 


void on_start(){
  sensor_t rob;
  float angle;
  rmb_awake(&base.x, &base.y);
  rob=rmb_state();
  printf("Pos: %d, %d - Heading: %.2f\n", rob.x, rob.y, rad2deg(rob.head));
  printf("Entering in the main loop\n ");
 
}


void cyclic_behav(){
    sensor_t rob;
    rmb_forward();
    if(rmb_bumper()){
      rob = rmb_state();
      printf("Wall found at: %d, %d - Heading: %.2f\n", rob.x, rob.y, rad2deg(rob.head));
      printf("Turning...(%d) x pi\n", dirx);
      rmb_turn(dirx * M_PI_2);
      rob = rmb_state();
      printf("Turn -> Pos: %d, %d - Heading: %.2f\n", rob.x, rob.y, rad2deg(rob.head));
      rmb_forward();
      rob = rmb_state();
      printf("Fwd -> Pos: %d, %d - Heading: %.2f\n", rob.x, rob.y, rad2deg(rob.head));
      rmb_turn(dirx * M_PI_2);
      rob = rmb_state();
      printf("Turn -> Pos: %d, %d - Heading: %.2f\n", rob.x, rob.y, rad2deg(rob.head));
      dirx *= -1;
    }
 }



void on_stop(){
  char c;
  printf("Simulation complete.\n");
  printf("Press [Enter] to view progress or [Ctrl-C] to exit.\n");
  scanf("%c", &c);
  visualize();
}


int main(int argc, char **argv){
  if(argc > 1){
    load_map(argv[1]);
  }
  configure(on_start, cyclic_behav, on_stop, 1000);
  run();
  return 0;
}
