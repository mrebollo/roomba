//random walk in a 50x50 matrix

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define SIZE 50

int main(){
  int t;
  float row, col;
  float angle;

  //posicion inicial aleatoria
  row = rand() % SIZE;
  col = rand() % SIZE;
  for(t = 0; t < 100; t++){
    //gira un ángulo al azar, entre 0 y 2pi
    //(M_PI es una cte. definida en math.h)
    angle = rand() / (float)RAND_MAX * 2 * M_PI;
    row += cos(angle);
    col += sin(angle);
    //asegura que la posición esté en el rango
    if(row < 0) row = 0;
    if(row >= SIZE) row = SIZE - 1;
    if(col < 0) col = 0;
    if(col >= SIZE) col = SIZE - 1;
    printf("%f, %f\n", row, col);      
  }
  return 0;
}