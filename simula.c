#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <signal.h>
#include "simula.h"

#ifndef COMPETITION_MODE
  #define DEBUG_PRINT(...) printf(__VA_ARGS__)
  #define ENABLE_VISUALIZATION 1
  #define STATS_FILE "stats.csv"
  #define LOG_FILE "log.csv"
#else
  #define DEBUG_PRINT(...) 
  #define ENABLE_VISUALIZATION 0
  #define STATS_FILE "../stats.csv"
  #define LOG_FILE "log.csv"
#endif

#define WORLDSIZE 50
#define WALL '#'
#define EMPTY ' '
#define MAXDIRT 5
#define M_PI_8 ((M_PI) / 8)

// Costes de batería (constantes)
#define COST_TURN 0.1f
#define COST_MOVE 1.0f
#define COST_MOVE_DIAG 1.4f
#define COST_BUMP 0.5f
#define COST_CLEAN 0.5f

#define MAXBAT 1000
#define PROGBARLEN 50
#define rounda(a) (round((a)*100000) / 100000.0)

typedef struct _dirt{
  int x, y;
  int depth;
} dirt_t;

typedef struct _map{
  char patch[WORLDSIZE][WORLDSIZE];
  int nrow, ncol;
  dirt_t dirt[WORLDSIZE*WORLDSIZE];
  int ndirt;
  char *name;
  int bx, by;
} map_t;

typedef struct _config{
  void (*on_start)();
  void (*exec_beh)();
  void (*on_stop)();
  FILE *output;
  int exec_time;
} config_t;

typedef struct _robot{
  sensor_t s;
  float x, y;
} robot_t;

static map_t map;
static robot_t r;
static sensor_t hist[WORLDSIZE*WORLDSIZE];
static config_t config;
static int timer;
static struct sensor *rob = (struct sensor*)&r;

enum movement {FWD, TURN, BUMP, CLE, LOAD};
struct _stat{
  int cell_total;
  int cell_visited;
  int dirt_total;
  int dirt_cleaned;
  float bat_total;
  float bat_mean;
  int moves[5];
} stats;

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

static char* ascii_progress(int perc){
  static char bar[PROGBARLEN+3];
  int i;
  bar[0] = '[';
  bar[PROGBARLEN+1] = ']';
  bar[PROGBARLEN+2] = '\0';
  for(i = 1; i <= perc / 2; i++) bar[i] = '=';
  for(; i <= PROGBARLEN; i++) bar[i] = ' ';
  return bar;
}

static char *compass(float angle){
  static char dir[3];
  if(angle >= 15*M_PI_8 || angle < M_PI_8) strcpy(dir, "E");
  else if(angle < 3*M_PI_8) strcpy(dir, "SE");
  else if(angle < 5*M_PI_8) strcpy(dir, "S");
  else if(angle < 7*M_PI_8) strcpy(dir, "SW");
  else if(angle < 9*M_PI_8) strcpy(dir, "W");
  else if(angle < 11*M_PI_8) strcpy(dir, "NW");
  else if(angle < 13*M_PI_8) strcpy(dir, "N");
  else strcpy(dir, "NE");
  return dir;
}

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

static void print_map_ascii(){
  for(int i = 0; i < map.nrow; i++){
    for(int j = 0; j < map.ncol; j++)
      printf("%c", map.patch[i][j]);
    printf("\n");
  }
}

static void annotate_dirt_to_map(){
  for(int i = 0; i < map.ndirt; i++){
    dirt_t d = map.dirt[i];
    if(map.name != NULL)
      map.patch[d.x][d.y] = d.depth + '0';
    else
      map.patch[d.y][d.x] = d.depth + '0';
  }
}

static void overlay_path_on_map(sensor_t h[], int len){
  for(int i = 0; i < len; i++)
    map.patch[h[i].y][h[i].x] = '.';
}

static void mark_base_and_end(sensor_t h[], int len){
  map.patch[h[0].y][h[0].x] = 'B';
  map.patch[h[len].y][h[len].x] = 'o';
}

static void print_status_line(sensor_t *s){
  int bat = s->battery / MAXBAT * 100;
  printf("\nBATT: %s %d%%", ascii_progress(bat), bat);
  if (bat < 20 && bat > 0.1)
    printf(" WARNING!");
  printf("\n\nPOS (y:%2d,x:%2d)\t\tHEAD: %s (%d)\n",
    s->y, s->x, compass(s->heading), (int)(s->heading * 180.0 / M_PI));
  printf("\nBUMPER: %c\t\t IFR: %c%s\n",
    s->bumper ? 'D' : ' ',
    s->infrarred > 0 ? s->infrarred + '0' : ' ',
    s->infrarred > 0 ? "...cleaning" : " ");
}

static void save_log(){
  FILE *fd = fopen(LOG_FILE,"w");
  if(!fd) return;
  fprintf(fd, "y, x, head, bump, ifr, batt\n");
  for(int i = 0; i < timer; i++)
    fprintf(fd, "%d, %d, %.1f, %d, %d, %.1f\n",
      hist[i].y, hist[i].x,
      hist[i].heading * 180.0 / M_PI,
      hist[i].bumper,
      hist[i].infrarred,
      hist[i].battery);
  fclose(fd);
}

static void save_stats(){
  FILE *fd = fopen(STATS_FILE,"w");
  if(!fd) return;
  stats.bat_mean = 0;
  for(int i = 0; i < timer; i++)
    stats.bat_mean += hist[i].battery;
  stats.bat_mean /= (timer > 0 ? timer : 1);
  fprintf(fd, "cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, forward, turn, bumps, clean, load\n");
  fprintf(fd, "%d, %d, %d, %d, %.1f, %.1f, %d, %d, %d, %d, %d\n",
    stats.cell_total, stats.cell_visited, stats.dirt_total, stats.dirt_cleaned,
    stats.bat_total, stats.bat_mean,
    stats.moves[FWD], stats.moves[TURN], stats.moves[BUMP], stats.moves[CLE], stats.moves[LOAD]);
  fclose(fd);
}

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
  print_map_ascii();
  return 0;
}

void print_map(){
  print_map_ascii();
}

void print_path(sensor_t h[], int len){
  annotate_dirt_to_map();
  overlay_path_on_map(h, len);
  mark_base_and_end(h, len);
  print_map_ascii();
  print_status_line(&h[len]);
}

// Visualization interrupt handling
static volatile sig_atomic_t g_stop_vis = 0;
static void sigint_vis_handler(int signo){
  (void)signo;
  g_stop_vis = 1;
}

void visualize(){
#if ENABLE_VISUALIZATION
  g_stop_vis = 0;
  void (*prev)(int) = signal(SIGINT, sigint_vis_handler);
  for(int t = 0; t < timer && !g_stop_vis; t++){
    system("clear");
    print_path(hist, t);
    printf("Ctrl-C para salir\n");
    struct timespec ts = {0, 100000000};
    nanosleep(&ts, NULL);
  }
  signal(SIGINT, prev);
#endif
}

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
  atexit(save_log);
  atexit(save_stats);
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
