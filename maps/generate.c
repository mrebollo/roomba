/**
 * @brief Genera mapas aleatorios para el roomba
 * 
 * Genera un mapa al azar y lo guarda en format PBM
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simula.h"

#define WALL 128
#define EMPTY 255
#define BASE 0
#define MAXDIRT 5
#define WORLDSIZE 50
enum obstacle {NONE, VERT, HORIZ, RANDOM};


void save(int m[][WORLDSIZE], int rows, int cols, char *filename){
    FILE *fd = fopen(filename,"w");

    fprintf(fd,"P2\n#roomba map\n%d %d\n%d\n", rows, cols, EMPTY);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++)
            fprintf(fd,"%d ", m[i][j]);
        fprintf(fd,"\n");
    }
    fclose(fd);
}



void vertical_wall(int m[][WORLDSIZE], int nrow, int ncol){
  int len, init, col, i;
  len = rand() % nrow / 2 + nrow  / 4;
  init = rand() % nrow / 2 + 2;
  col = ncol / 2 + rand() % 10 - 5;
  printf("*vert: len: %d, init: %d, col: %d\n", len, init, col);
  for(i = 0; i < len; i++)
    m[init + i][col] = WALL;
}


void horiz_wall(int m[][WORLDSIZE], int nrow, int ncol){
  int len, init, row, i;
  len = rand() % ncol / 2 + ncol  / 4;
  init = rand() % ncol / 2 + 2;
  row = nrow / 2 + rand() % 10 - 5;
  for(i = 0; i < len; i++)
    m[row][init + i] = WALL;
}


void random_obs(int m[][WORLDSIZE], int nrow, int ncol, int nobs){
  int i, row, col;
  for(i = 0; i < nobs; i++){
    do{
      row = rand() % (nrow - 4) + 2;
      col = rand() % (ncol - 4) + 2;
    }while(m[row][col] != EMPTY);
    m[row][col] = WALL;
  }
}


int gen_map(int m[][WORLDSIZE], int nrow, int ncol, int num_dirty, 
            int obs, float density){
  int i, j, row, col;
  int numobs, len, orient, init; //obstacle
  if( nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  //empty

  for(i = 0; i < WORLDSIZE; i++)
    for(j = 0; j < WORLDSIZE; j++)
      m[i][j] = EMPTY;
  //wall around
  for(i = 0; i < nrow; i++){
    m[i][0] = WALL;
    m[i][ncol-1] = WALL;
  }
  for(i = 0; i < ncol; i++){
    m[0][i] = WALL;
    m[nrow-1][i] = WALL;
  }
  //obstacles

  switch(obs){
    case VERT: vertical_wall(m, ncol, nrow); break;
    case HORIZ: horiz_wall(m, ncol, nrow); break;
    case RANDOM: random_obs(m, ncol, nrow, WORLDSIZE*WORLDSIZE*density); break;
  }
  //dirt positions
  for(i = 0; i < num_dirty; i++){
    //position avoiding walls
    do{
      row = rand() % (nrow - 2) + 1;
      col = rand() % (ncol - 2) + 1;
    }while(m[row][col] != EMPTY);
    m[row][col] = rand() % MAXDIRT + 1;
  }
  return 0;
}


int main(int argc, char *argv[]){
  int m[WORLDSIZE][WORLDSIZE];

  srand(time(0));
  gen_map(m,WORLDSIZE,WORLDSIZE,50,NONE, 0);
  save(m,WORLDSIZE,WORLDSIZE,"noobs.pgm");

  gen_map(m,WORLDSIZE,WORLDSIZE,50,RANDOM, 0.01);
  save(m,WORLDSIZE,WORLDSIZE,"random1.pgm");

  gen_map(m,WORLDSIZE,WORLDSIZE,50,RANDOM, 0.03);
  save(m,WORLDSIZE,WORLDSIZE,"random3.pgm");

  gen_map(m,WORLDSIZE,WORLDSIZE,50,RANDOM, 0.05);
  save(m,WORLDSIZE,WORLDSIZE,"random5.pgm");
  return 0;
}