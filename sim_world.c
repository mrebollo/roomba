#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simula_internal.h"


static float base_heading(const map_t* m, int x, int y){
  float head[4] = {M_PI / 2, 3 * M_PI / 2, 0, M_PI};
  if(x == 1) return head[2];
  else if(x == m->ncol-2) return head[3];
  else if(y == 1) return head[0];
  else return head[1];
}

float sim_world_put_base(map_t* m, int x, int y){
  m->cells[y][x] = 'B';
  m->base_x = x;
  m->base_y = y;
  return base_heading(m, x, y);
}

void sim_world_set_base_origin(map_t* m, int *x, int *y, float *h){
  *x = 1;
  *y = 1;
  *h = 0;
  m->cells[*y][*x] = 'B';
}

void sim_world_save(const map_t* m){
  FILE *fd = fopen("map.pgm","w");
  if(!fd) return;
  fprintf(fd,"P2\n#roomba map\n%d %d\n%d\n", m->ncol, m->nrow, 255);
  for(int i = 0; i < m->nrow; i++){
    for(int j = 0; j < m->ncol; j++){
      switch(m->cells[i][j]){
        case WALL: fprintf(fd,"%d ",128); break;
        case EMPTY: fprintf(fd,"%d ",255); break;
        case 'B': fprintf(fd,"%d ",0); break;
        default: fprintf(fd,"%d ", m->cells[i][j]-'0');
      }
    }
    fprintf(fd,"\n");
  }
  fclose(fd);
}

int sim_world_is_wall(const map_t* m, int y, int x){
  return m->cells[y][x] == WALL;
}

int sim_world_is_base(const map_t* m, int y, int x){
  return m->cells[y][x] == 'B';
}

int sim_world_cell_dirt(const map_t* m, int y, int x){
  char c = m->cells[y][x];
  if(c == 'B' || c == WALL || c == EMPTY) return 0;
  return c - '0';
}

void sim_world_set_cell_dirt(map_t* m, int y, int x, int dirt){
  if(dirt <= 0) m->cells[y][x] = EMPTY;
  else m->cells[y][x] = (char)('0' + dirt);
}

int sim_world_clean_cell(map_t* m, int y, int x){
  int d = sim_world_cell_dirt(m, y, x);
  if(d > 0){
    --d;
    sim_world_set_cell_dirt(m, y, x, d);
  }
  return d;
}

static void create_vertical_wall(map_t* m){
  int len = rand()%m->nrow/2 + m->nrow/4;
  int init = rand()%m->nrow/2 + 2;
  int col = rand()%(m->ncol-4)+2;
  for(int i = 0; i < len; i++)
    m->cells[init + i][col] = WALL;
}

static void create_horiz_wall(map_t* m){
  int len = rand()%m->ncol/2 + m->ncol/4;
  int init = rand()%m->ncol/2 + 2;
  int row = rand()%(m->nrow-4)+2;
  for(int i = 0; i < len; i++)
    m->cells[row][init + i] = WALL;
}

static void create_random_obstacles(map_t* m, float prop){
  for(int i = 2; i < WORLDSIZE-2; i++)
    for(int j = 2; j < WORLDSIZE-2; j++)
      if(rand()/(float)RAND_MAX < prop){
        m->cells[i][j] = WALL;
        DEBUG_PRINT("%d, %d\n", i, j);
      }
}

static void init_empty_world(map_t* m, int nrow,int ncol){
  m->nrow = nrow;
  m->ncol = ncol;
  for(int i = 0; i < WORLDSIZE; i++)
    for(int j = 0; j < WORLDSIZE; j++)
      m->cells[i][j] = EMPTY;
}

static void add_border_walls(map_t* m){
  for(int i = 0; i < m->nrow; i++){
    m->cells[i][0] = WALL;
    m->cells[i][m->ncol-1] = WALL;
  }
  for(int i = 0; i < m->ncol; i++){
    m->cells[0][i] = WALL;
    m->cells[m->nrow-1][i] = WALL;
  }
}

static void add_obstacles(map_t* m, float nobs){
  if(nobs >= 1){
    int numobs = (int)nobs;
    while(numobs-- > 0){
      if(rand()%2) create_vertical_wall(m);
      else create_horiz_wall(m);
    }
  } else if(nobs > 0) {
    create_random_obstacles(m, nobs);
  }
}

static void place_dirt(map_t* m, int num_dirty){
  m->ndirt = num_dirty;
  for(int i = 0; i < num_dirty; i++){
    int row, col;
    do{
      row = rand()%(m->nrow-2) + 1;
      col = rand()%(m->ncol-2) + 1;
    }while(m->cells[row][col] != EMPTY);
    m->dirt[i].x = col;
    m->dirt[i].y = row;
    m->dirt[i].depth = rand()%MAXDIRT + 1;
    m->cells[row][col] = '0' + m->dirt[i].depth;
  }
}

int sim_world_generate(map_t* m, int nrow, int ncol, int num_dirty, float nobs){
  if(nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  init_empty_world(m, nrow, ncol);
  add_border_walls(m);
  add_obstacles(m, nobs);
  place_dirt(m, num_dirty);
  return 0;
}

int sim_world_load(map_t* m, char *filename){
  int i, j, cell, dc = 0;
  int nrow, ncol, aux;
  char line[50];
  DEBUG_PRINT("Loading map %s\n", filename);
  FILE *fd = fopen(filename, "r");
  if(!fd) return -1;
  m->ndirt = 0;
  fgets(line, 50, fd);
  fgets(line, 50, fd);
  fscanf(fd, "%d%d", &ncol, &nrow);
  fscanf(fd, "%d", &aux);
  if(nrow > WORLDSIZE || ncol > WORLDSIZE){ fclose(fd); return -1; }
  m->nrow = nrow;
  m->ncol = ncol;
  for(i = 0; i < nrow; i++){
    for(j = 0; j < ncol; j++){
      fscanf(fd, "%d", &cell);
      switch(cell){
        case 128: m->cells[i][j] = WALL; break;
        case 255: m->cells[i][j] = EMPTY; break;
        case 0: sim_world_put_base(m, j, i); break;
        default:
          m->dirt[dc].x = i;
          m->dirt[dc].y = j;
          m->dirt[dc++].depth = cell;
          m->cells[i][j] = cell + '0';
          m->ndirt++;
          break;
      }
    }
    fgets(line, 50, fd);
  }
  fclose(fd);
  if(filename)
    snprintf(m->name, sizeof(m->name), "%s", filename);
  else
    m->name[0] = '\0';
  return 0;
}
