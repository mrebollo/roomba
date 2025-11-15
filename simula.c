#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include "simula.h"
#include "simula_internal.h"
map_t map = {0};
robot_t r;
sensor_t hist[WORLDSIZE*WORLDSIZE];
config_t config;
int timer;
struct sensor *rob = (struct sensor*)&r;
struct _stat stats = {0};
static double _bat_sum = 0.0;
static int _bat_samples = 0;


static void save_state(sensor_t *state){
  state->x = rob->x;
  state->y = rob->y;
  state->heading = rob->heading;
  state->bumper = rob->bumper;
  state->infrarred = rob->infrarred;
  state->battery = rob->battery;
}

static int at_base(){
  return map.patch[rob->y][rob->x] == 'B';
}

// Visual helpers moved to sim_visual.c

static float base_heading(int x, int y){
  float head[4] = {M_PI / 2, 3 * M_PI / 2, 0, M_PI};
  if(x == 1) return head[2];
  else if(x == map.ncol-2) return head[3];
  else if(y == 1) return head[0];
  else return head[1];
}

static float put_base_at(int x, int y){
  map.patch[y][x] = 'B';
  map.bx = x;
  map.by = y;
  return base_heading(x, y);
}

static void set_base_at_origin(int *x, int *y, float *h){
  *x = 1;
  *y = 1;
  *h = 0;
  map.patch[*y][*x] = 'B';
}

static void tick(int action){
  assert(timer < config.exec_time);
  save_state(&hist[timer]);
  _bat_sum += rob->battery;
  _bat_samples++;
  if(rob->battery < 0.1 || (action != -1 && ++timer >= config.exec_time))
    exit(0);
}

static void apply_battery(float amount){
  rob->battery -= amount;
  stats.bat_total += amount;
}

static int is_wall_cell(int y, int x){
  return map.patch[y][x] == WALL;
}

static void update_ifr_at_cell(){
  if(map.patch[rob->y][rob->x] == EMPTY)
    rob->infrarred = 0;
  else if(map.patch[rob->y][rob->x] != 'B')
    rob->infrarred = map.patch[rob->y][rob->x] - '0';
}

static void step_vectors(float heading, int *rx, int *ry, float *dx, float *dy){
  *dy = rounda(sin(heading));
  *dx = rounda(cos(heading));
  *rx = (int)(r.x + *dx);
  *ry = (int)(r.y + *dy);
}

// Status/HUD moved to sim_visual.c

// IO moved to sim_io.c

static void save_map(){
  FILE *fd = fopen("map.pgm","w");
  if(!fd) return;
  fprintf(fd,"P2\n#roomba map\n%d %d\n%d\n", map.ncol, map.nrow, 255);
  for(int i = 0; i < map.nrow; i++){
    for(int j = 0; j < map.ncol; j++){
      switch(map.patch[i][j]){
        case WALL: fprintf(fd,"%d ",128); break;
        case EMPTY: fprintf(fd,"%d ",255); break;
        case 'B': fprintf(fd,"%d ",0); break;
        default: fprintf(fd,"%d ", map.patch[i][j]-'0');
      }
    }
    fprintf(fd,"\n");
  }
  fclose(fd);
}

static void create_vertical_wall(){
  int len = rand()%map.nrow/2 + map.nrow/4;
  int init = rand()%map.nrow/2 + 2;
  int col = rand()%(map.ncol-4)+2;
  for(int i = 0; i < len; i++)
    map.patch[init + i][col] = WALL;
  stats.cell_total -= len;
}

static void create_horiz_wall(){
  int len = rand()%map.ncol/2 + map.ncol/4;
  int init = rand()%map.ncol/2 + 2;
  int row = rand()%(map.nrow-4)+2;
  for(int i = 0; i < len; i++)
    map.patch[row][init + i] = WALL;
  stats.cell_total -= len;
}

static void create_random_obstacles(float prop){
  for(int i = 2; i < WORLDSIZE-2; i++)
    for(int j = 2; j < WORLDSIZE-2; j++)
      if(rand()/(float)RAND_MAX < prop){
        map.patch[i][j] = WALL;
        DEBUG_PRINT("%d, %d\n", i, j);
      }
}

static void init_empty_world(int nrow,int ncol){
  map.nrow = nrow;
  map.ncol = ncol;
  for(int i = 0; i < WORLDSIZE; i++)
    for(int j = 0; j < WORLDSIZE; j++)
      map.patch[i][j] = EMPTY;
}

static void add_border_walls(){
  for(int i = 0; i < map.nrow; i++){
    map.patch[i][0] = WALL;
    map.patch[i][map.ncol-1] = WALL;
  }
  for(int i = 0; i < map.ncol; i++){
    map.patch[0][i] = WALL;
    map.patch[map.nrow-1][i] = WALL;
  }
  stats.cell_total = map.ncol * map.nrow - (map.ncol + map.nrow - 4);
}

static void add_obstacles(float nobs){
  if(nobs >= 1){
    int numobs = (int)nobs;
    while(numobs-- > 0){
      if(rand()%2) create_vertical_wall();
      else create_horiz_wall();
    }
  } else if(nobs > 0) {
    create_random_obstacles(nobs);
  }
}

static void place_dirt(int num_dirty){
  stats.dirt_total = 0;
  map.ndirt = num_dirty;
  for(int i = 0; i < num_dirty; i++){
    int row, col;
    do{
      row = rand()%(map.nrow-2) + 1;
      col = rand()%(map.ncol-2) + 1;
    }while(map.patch[row][col] != EMPTY);
    map.dirt[i].x = col;
    map.dirt[i].y = row;
    map.dirt[i].depth = rand()%MAXDIRT + 1;
    map.patch[row][col] = '0' + map.dirt[i].depth;
    stats.dirt_total += map.dirt[i].depth;
  }
}

int generate_map(int nrow, int ncol, int num_dirty, float nobs){
  if(nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  init_empty_world(nrow, ncol);
  add_border_walls();
  add_obstacles(nobs);
  place_dirt(num_dirty);
  return 0;
}

int load_map(char *filename){
  int i, j, cell, dc = 0;
  int nrow, ncol, aux;
  char line[50];
  DEBUG_PRINT("Loading map %s\n", filename);
  FILE *fd = fopen(filename, "r");
  if(!fd) return -1;
  map.ndirt = 0;
  fgets(line, 50, fd);
  fgets(line, 50, fd);
  fscanf(fd, "%d%d", &ncol, &nrow);
  fscanf(fd, "%d", &aux);
  if(nrow > WORLDSIZE || ncol > WORLDSIZE){ fclose(fd); return -1; }
  map.nrow = nrow;
  map.ncol = ncol;
  for(i = 0; i < nrow; i++){
    for(j = 0; j < ncol; j++){
      fscanf(fd, "%d", &cell);
      switch(cell){
        case 128: map.patch[i][j] = WALL; break;
        case 255: map.patch[i][j] = EMPTY; break;
        case 0: put_base_at(j, i); break;
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
  return 0;
}
// Visualization moved to sim_visual.c

static void _save_log_wrapper(void){ save_log(hist, timer); }
static void _save_stats_wrapper(void){ stats.bat_mean = (_bat_samples>0)? (float)(_bat_sum/_bat_samples) : 0.0f; save_stats(&stats); }

void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time){
  float density;
  srand(time(0));
  config.on_start = start;
  config.exec_beh = beh;
  config.on_stop = stop;
  config.exec_time = exec_time > 0 && exec_time < WORLDSIZE*WORLDSIZE ? exec_time : WORLDSIZE*WORLDSIZE;
#ifndef COMPETITION_MODE
  if(stop != NULL) atexit(stop);
#endif
  atexit(_save_log_wrapper);
  atexit(_save_stats_wrapper);
  density = rand()/(float)RAND_MAX * 0.05f;
  if(map.name == NULL)
    generate_map(WORLDSIZE, WORLDSIZE, 100, density);
}

void run(){
  timer = 0;
  if(config.on_start)
    config.on_start();
  for(;;)
    config.exec_beh();
}

int rmb_awake(int *x, int *y){
  DEBUG_PRINT("Awaking...\n");
  DEBUG_PRINT("Map: %s\n", map.name);
  if(map.name != NULL){
    rob->heading = put_base_at(map.bx, map.by);
    *x = map.bx;
    *y = map.by;
  } else {
    DEBUG_PRINT("No map loaded\n");
    set_base_at_origin(x, y, &rob->heading);
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
  rob->heading += alpha;
  if(rob->heading < 0)
    rob->heading += 2 * M_PI;
  rob->heading = fmod(rob->heading, 2 * M_PI);
  rob->bumper = 0;
  apply_battery(COST_TURN);
  stats.moves[TURN]++;
}

void rmb_forward(){
  float dy, dx;
  int rx, ry;
  step_vectors(rob->heading, &rx, &ry, &dx, &dy);
  if(is_wall_cell(ry, rx)){
    rob->bumper = 1;
    stats.moves[BUMP]++;
    tick(-1);
    apply_battery(COST_BUMP);
    return;
  }
  r.x += dx;
  r.y += dy;
  rob->x = rx;
  rob->y = ry;
  update_ifr_at_cell();
  stats.moves[FWD]++;
  stats.cell_visited++;
  tick(rob->infrarred);
  apply_battery((fabsf(dy*dx) < 0.0001f) ? COST_MOVE : COST_MOVE_DIAG);
}

void rmb_clean(){
  int dirt = map.patch[rob->y][rob->x] - '0';
  if(dirt > 0){
    dirt--;
    map.patch[rob->y][rob->x] = dirt + '0';
    apply_battery(COST_CLEAN);
    rob->infrarred = dirt;
    stats.moves[CLE]++;
    if(dirt == 0) stats.dirt_cleaned++;
  }
  tick(dirt);
}

int rmb_load(){
  if(at_base()){
    rob->battery += 10;
    if(rob->battery > MAXBAT)
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
