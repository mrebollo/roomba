#include "simula_map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simula_types.h"

// Map generation and maintenance
int generate_map(int nrow, int ncol, int num_dirty, float nobs) {
  int i, j, row, col;
  int numobs, len, orient, init; //obstacle
  if( nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  //empty
  map.nrow = nrow;
  map.ncol = ncol;
  for(i = 0; i < WORLDSIZE; i++)
    for(j = 0; j < WORLDSIZE; j++)
      map.patch[i][j] = EMPTY;
  //wall around
  for(i = 0; i < nrow; i++){
    map.patch[i][0] = WALL;
    map.patch[i][ncol-1] = WALL;
  }
  for(i = 0; i < ncol; i++){
    map.patch[0][i] = WALL;
    map.patch[nrow-1][i] = WALL;
  }
  stats.cell_total = ncol * nrow - (ncol + nrow - 4);
  //obstacles
  if(nobs >= 1){
    numobs = (int)nobs;
    while(numobs-- > 0){
      if (rand() % 2)
        create_vertical_wall();
      else 
        create_horiz_wall();
    }
  }
  else if(nobs > 0)
    create_random_obstacles(nobs);
  
  //dirt positions
  stats.dirt_total = 0;
  map.ndirt = num_dirty;
  for(i = 0; i < num_dirty; i++){
    //position avoiding walls
    do{
      row = rand() % (nrow - 2) + 1;
      col = rand() % (ncol - 2) + 1;
    }while(map.patch[row][col] != EMPTY);
    //dirt
    map.dirt[i].x = col;
    map.dirt[i].y = row;
    map.dirt[i].depth = rand() % MAXDIRT + 1;
    map.patch[row][col] = '0' + map.dirt[i].depth;
    stats.dirt_total += map.dirt[i].depth;
  }
  return 0;
}

int load_map(char *filename){
  int i, j, cell, dc = 0; 
  int nrow, ncol, aux;
  int numobs, len, orient, init; //obstacle
  char line[50];
  printf("Loading map %s\n", filename);
  FILE *fd = fopen(filename, "r");
  fgets(line, 50, fd); //P2
  fgets(line, 50, fd); //comment
  fscanf(fd,"%d%d", &ncol, &nrow);
  fscanf(fd,"%d", &aux);
  if( nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  //empty
  map.nrow = nrow;
  map.ncol = ncol;
  for(i = 0; i < nrow; i++){
    for(j = 0; j < ncol; j++){
      fscanf(fd, "%d", &cell);
      switch(cell){
      case 128: 
        map.patch[i][j] = WALL;
        break;
      case 255:
        map.patch[i][j] = EMPTY;
        break;  
      case 0:
        //TODO review this call and the inversion of x and y
        put_base_at(j, i);
        break;
      default:
        map.dirt[dc].x = i;
        map.dirt[dc].y = j;
        map.dirt[dc++].depth = cell;
        map.patch[i][j] = cell + '0';
        map.ndirt++;
        break;    
      }
    }
    fgets(line, 50, fd);
  }
  fclose(fd);
  map.name = filename;
  print_map();
  return 0;
}

void print_map(){
  int i, j;
  for(i = 0; i < map.nrow; i++){
    for(j = 0; j < map.ncol; j++)
      printf("%c", map.patch[i][j]);
     printf("\n");
  }
}

void save_map(){
  FILE *fd = fopen("map.pgm","w");

  fprintf(fd,"P2\n#roomba map\n%d %d\n%d\n", map.ncol, map.nrow, 255);
  for(int i = 0; i < map.nrow; i++){
      for(int j = 0; j < map.ncol; j++){ 
        switch(map.patch[i][j]){
          case WALL: fprintf(fd,"%d ", 128); break;
          case EMPTY: fprintf(fd,"%d ", 255); break;
          case 'B': fprintf(fd,"%d ", 0); break;
          default: fprintf(fd,"%d ", map.patch[i][j]-'0'); break;
        }  
      }
      fprintf(fd,"\n");
  }
  fclose(fd);
}

float put_base_at(int x, int y){
  map.patch[y][x] = 'B';
  map.bx = x;
  map.by = y;
  return base_heading(x, y);
}
