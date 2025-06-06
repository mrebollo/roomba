#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include "simula.h"


#define WORLDSIZE 50
#define WALL '#'
#define UNKNOWN '.'
#define EMPTY ' '
#define MAXDIRT 5

typedef struct _dirt{
  int x, y;
  int depth;
} dirt_t;

typedef struct _map{
    char patch[WORLDSIZE][WORLDSIZE];
    int nrow, ncol;
    dirt_t dirt[WORLDSIZE*WORLDSIZE];
    int ndirt;
    char filename[50];
    int type;
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
  //sensor must be in the first position to allow casting to sensor_t
  sensor_t s;
  float x, y;
} robot_t;


#define MAXBAT 1000
#define EXECTIME 2500
#define PROGBARLEN 50
#define rounda(a) (round(a*100000) / 100000.0)

map_t _map ={0} ;
robot_t r;
sensor_t hist[EXECTIME]; //it's longer
//sensor_t hist[WORLDSIZE*WORLDSIZE];
config_t config;
int timer = 0;
struct sensor *_rob = (struct sensor*)&r;
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

int load_map(char *filename);
int generate_map(int, int, int, float);
//void print_map();
void save_log();
void save_map();
void save_stats();
void save_config();
float put_base_at(int, int);

/**
 * Control functions
 * 
 */

int load_map_from(char *filename){
  FILE *fd = fopen(filename, "r");
  if (fd != NULL){
    strcpy(_map.filename, filename);
    fclose(fd);
    return 0;
  }
  printf("File not found\n");
  return 1;
}

void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time){
  srand(time(0));
  config.on_start = start;
  config.exec_beh = beh;
  config.on_stop =  stop;
  //  config.exec_time = exec_time > 0 && exec_time < WORLDSIZE * WORLDSIZE ? exec_time : WORLDSIZE * WORLDSIZE;
  config.exec_time = EXECTIME; //tiempo fijo para todos los equipos
  /*
   * For the competition, avoid any final user process
   *
  if(stop != NULL)
    atexit(stop);
   */ 
  
  atexit(save_log);
  atexit(save_stats);
  atexit(save_config);
}


void execute(char* filename, int type){
  if(load_map_from(filename) != 0)
    exit(1);
  _map.type = type;
  load_map(filename);
  printf("map loaded: %s\n", filename);
  if(config.on_start != NULL)
    config.on_start();
  //printf("ready to start\n");
  for(;;)
    config.exec_beh();
}

void run(){
  int rep, id;
  char *seq[] = {"../noobs.pgm", "../random1.pgm", "../random3.pgm", "../random5.pgm"};
  char filename[50];
  FILE *fd = fopen("config.txt", "r");
  fscanf(fd, "%d%d", &rep, &id);
  fclose(fd);
  assert(id < 4);
  execute(seq[id], id);
}


/*
* Private actions avialable for the _robot
*/

void update_sensors(){
  if(_map.patch[_rob->y][_rob->x] != EMPTY && 
  _map.patch[_rob->y][_rob->x] != WALL && 
  _map.patch[_rob->y][_rob->x] != 'B')
    _rob->infrarred = _map.patch[_rob->y][_rob->x] - '0';
}

// generates base next to a wall (not for 2024)
/* void set_base(int *x, int *y, float *h){
    // up = 0, dowm, left, right = 3
    switch(rand() % 4){
    case 0: *y = 1; *x = rand() % (_map.ncol-4) + 2; *h = M_PI / 2; break;
    case 1: *y = _map.nrow-2; *x = rand() % (_map.ncol-2) + 2; *h = -M_PI / 2; break;
    case 2: *x = 1; *y = rand() % (_map.nrow-4) + 2; *h = 0; break; 
    case 3: *x = _map.ncol-2; *y = rand() % (_map.nrow-2) + 2; *h = M_PI; break;
    }
    _map.patch[*y][*x] = 'B';
} */

float base_heading(int x, int y){
  // up = 0, down, left, right = 3
  float head[4] = {M_PI / 2, 3 * M_PI / 2, 0, M_PI};
  if(x == 1)
    return head[2];
  else if(x == _map.ncol-2)
    return head[3];
  else if(y == 1)
    return head[0];
  else
    return head[1];
} 



// for 2024 version, begin at (1,1)
void set_base(int *x, int *y, float *h){
    // up = 0, down, left, right = 3
    float head[4] = {M_PI / 2, 3 * M_PI / 2, 0, M_PI};
    int wall = rand() % 4;
    //TODO: cambiarlo para usar put_base_at() y base_heading()
    switch(rand() % 4){
    //left wall
    case 0: *y = 1; *x = rand() % (_map.ncol-4) + 2; *h = M_PI / 2; break;
    //right wall
    case 1: *y = _map.nrow-2; *x = rand() % (_map.ncol-2) + 2; *h = -M_PI / 2; break;
    //bottom wall
    case 2: *x = 1; *y = rand() % (_map.nrow-4) + 2; *h = 0; break; 
    //up wall
    case 3: *x = _map.ncol-2; *y = rand() % (_map.nrow-2) + 2; *h = M_PI; break;
    }
    _map.patch[*y][*x] = 'B';
}

float put_base_at(int x, int y){
  _map.patch[y][x] = 'B';
  _map.bx = x;
  _map.by = y;
  return base_heading(x, y);
}

// set base at (1,1)
void set_base_at_origin(int *x, int *y, float *h){
  *x = 1; *y = 1; *h = 0;
  _map.patch[*y][*x] = 'B';
} 


int at_base(){
    return _map.patch[_rob->y][_rob->x] == 'B';
}


void save_state(sensor_t *state, robot_t r){
  state->x = _rob->x;
  state->y = _rob->y;
  state->head = _rob->head;
  state->bumper = _rob->bumper;
  state->infrarred = _rob->infrarred;
  state->battery = _rob->battery;
}


void tick(int action){
  //printf("timer: %d - exec-time: %d\n", timer, config.exec_time);
  assert(timer < config.exec_time);
  //update_sensors(); 
  save_state(&hist[timer], r); 
  /*
  printf("[RMB] %d, %d, %.1f, %d, %d, %.1f\n", _rob->x, _rob->y, _rob->head * 180.0 / M_PI, 
        _rob->bumper, _rob->infrarred, _rob->battery); 
  */
  if(_rob->battery < 0.1 || (action != -1 && ++timer >= config.exec_time))
    exit(0);
}
 
void save_log(){
  int i;
  //printf("finished. saving log\n ");
  FILE *fd = fopen("log.csv","w");
  if(!fd)
    return;
  fprintf(fd, "y, x, head, bump, ifr, batt\n");
  for(i = 0; i < config.exec_time; i++){
    fprintf(fd, "%d, %d, %.1f, %d, %d, %.1f\n", hist[i].y, hist[i].x, hist[i].head * 180.0 / M_PI, 
        hist[i].bumper, hist[i].infrarred, hist[i].battery);  
  }
  fclose(fd);
}


void save_stats(){
  char cwd[128];
  char *current_folder;
  int i;
  //printf("finished. saving stats\n ");
  FILE *fd = fopen("../stats.csv","a");
  if(!fd)
    return;
  stats.bat_mean = 0;
  int histsize = sizeof(hist) /  sizeof(sensor_t);
  //printf("sizes:  declared %d - real %d\n ", config.exec_time, histsize);
  for(i = 0; i < histsize; i++)
    stats.bat_mean += hist[i].battery;
  //printf("get battery\n"); 
  stats.bat_mean /= histsize;

  getcwd(cwd, 128);
  //printf("folder: %s\n", cwd);
  current_folder = strrchr(cwd, '/') + 1;
  //printf("team id: %s\n", current_folder); 

  //fprintf(fd, "id, type, cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, \
forward, turn, bumps, clean, load\n");
  fprintf(fd, "%s, %d, %d, %d, %d, %d, %.1f, %.1f, %d, %d, %d, %d, %d\n", current_folder, _map.type,
   stats.cell_total, stats.cell_visited,
   stats.dirt_total, stats.dirt_cleaned,
   stats.bat_total, stats.bat_mean,
   stats.moves[FWD], stats.moves[TURN], stats.moves[BUMP], stats.moves[CLE], stats.moves[LOAD]);  
  fclose(fd);
  /* printf("%s, %d, %d, %d, %d, %d, %.1f, %.1f, %d, %d, %d, %d, %d\n", current_folder, _map.type, 
   stats.cell_total, stats.cell_visited,
   stats.dirt_total, stats.dirt_cleaned,
   stats.bat_total, stats.bat_mean,
   stats.moves[FWD], stats.moves[TURN], stats.moves[BUMP], stats.moves[CLE], stats.moves[LOAD]); */
   //printf("stats done\n");
}

void save_config(){
  int rep, id;
  //printf("finished. update config\n ");
  FILE *fd = fopen("config.txt", "r");
  fscanf(fd, "%d%d", &rep, &id);
  fclose(fd);
  fd = fopen("config.txt", "w");
  rep = (++rep) % 5;
  if(rep == 0) id++;
  fprintf(fd, "%d %d", rep, id);
  fclose(fd);
}


/**
 * Map generation and maintenance
 * 
 */

char* ascii_progress(int perc){
    //50 =, delimitators [ and ], and \0
    static char bar[PROGBARLEN+3];
    int i;
    bar[0] = '[';
    bar[PROGBARLEN+1] = ']';
    bar[PROGBARLEN+2] = '\0';
    for(i = 1; i <= perc / 2; i++)
        bar[i] = '=';
    for(; i <= PROGBARLEN; i++)
        bar[i] = ' ';
    return bar;
}

void create_vertical_wall(){
  int len, init, col, i;
  len = rand() % _map.nrow / 2 + _map.nrow  / 4;
  init = rand() % _map.nrow  / 2 + 2;
  col = rand() % (_map.ncol - 4) + 2;
  for(i = 0; i < len; i++)
    _map.patch[init + i][col] = WALL;
  stats.cell_total -= len;
}


void create_horiz_wall(){
  int len, init, row, i;
  len = rand() % _map.ncol / 2 + _map.ncol  / 4;
  init = rand() % _map.ncol  / 2 + 2;
  row = rand() % (_map.ncol - 4) + 2;
  for(i = 0; i < len; i++)
    _map.patch[row][init + i] = WALL;
  stats.cell_total -= len;
}

void create_random_obstacles(float prop){
  int i, j;
  //avoid obstacles next to a wall
  for(i = 2; i < WORLDSIZE-2; i++)
    for(j = 2; j < WORLDSIZE-2; j++) 
      if(rand() / (float)RAND_MAX < prop){ 
        _map.patch[i][j] = WALL;  
        //printf("%d, %d\n", i, j); 
      }
} 


int generate_map(int nrow, int ncol, int num_dirty, float nobs){
  int i, j, row, col;
  int numobs, len, orient, init; //obstacle
  if( nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  //empty
  _map.nrow = nrow;
  _map.ncol = ncol;
  for(i = 0; i < WORLDSIZE; i++)
    for(j = 0; j < WORLDSIZE; j++)
      _map.patch[i][j] = EMPTY;
  //wall around
  for(i = 0; i < nrow; i++){
    _map.patch[i][0] = WALL;
    _map.patch[i][ncol-1] = WALL;
  }
  for(i = 0; i < ncol; i++){
    _map.patch[0][i] = WALL;
    _map.patch[nrow-1][i] = WALL;
  }
  stats.cell_total = ncol * nrow - (ncol + nrow - 4);
  //obstacles
  if(nobs >= 1){
    numobs = (int)nobs;
    while(numobs--){
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
  _map.ndirt = num_dirty;
  for(i = 0; i < num_dirty; i++){
    //position avoiding walls
    do{
      row = rand() % (nrow - 2) + 1;
      col = rand() % (ncol - 2) + 1;
    }while(_map.patch[row][col] != EMPTY);
    //dirt
    _map.dirt[i].x = col;
    _map.dirt[i].y = row;
    _map.dirt[i].depth = rand() % MAXDIRT + 1;
    _map.patch[row][col] = '0' + _map.dirt[i].depth;
    stats.dirt_total += _map.dirt[i].depth;
  }
  return 0;
}


int load_map(char *filename){
  int i, j, cell, dc = 0; 
  int nrow, ncol;
  int numobs, len, orient, init; //obstacle
  char line[50];
  // review to maintain compatibility among versions

  FILE *fd = fopen(_map.filename, "r");
  fgets(line, 50, fd); //P2
  fgets(line, 50, fd); //comment
  fscanf(fd,"%d%d", &nrow, &ncol);
  if( nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  //empty
  _map.nrow = nrow;
  _map.ncol = ncol;
  for(i = 0; i < nrow; i++){
    for(j = 0; j < ncol; j++){
      fscanf(fd, "%d", &cell);
      switch(cell){
      case 128: 
        _map.patch[i][j] = WALL; 
        break;
      case 255:
        _map.patch[i][j] = EMPTY;
        stats.cell_total++;
        break; 
      case 0:
        //TODO review this call and the inversion of x and y
        put_base_at(j, i);
        break; 
      default:
        _map.dirt[dc].x = i;
        _map.dirt[dc].y = j;
        _map.dirt[dc++].depth = cell;
        _map.patch[i][j] = cell + '0';
        _map.ndirt++;
        stats.dirt_total += cell;
        break;    
      }
    }
    fgets(line, 50, fd);
  }
  fclose(fd);
  return 0;
}


void print_map(){
  int i, j;
  for(i = 0; i < _map.nrow; i++){
    for(j = 0; j < _map.ncol; j++)
      printf("%c", _map.patch[i][j]);
     printf("\n");
  }
}


void print_path(sensor_t hist[], int len){
  int i, j, bat;
  dirt_t d;
  //add initial dirt to map
  for(i = 0; i < _map.ndirt; i++){
    d = _map.dirt[i];
    _map.patch[d.y][d.x] = d.depth + '0';
  }
  //add path to map
  for(i = 0; i < len; i++)
    _map.patch[hist[i].y][hist[i].x] = '.';
  //mark the base again (deleted if robot passes over it)
  _map.patch[hist[0].y][hist[0].x] = 'B';
  _map.patch[hist[len].y][hist[len].x] = 'o';

  for(i = 0; i < _map.nrow; i++){
    for(j = 0; j < _map.ncol; j++)
      printf("%c", _map.patch[i][j]);
    printf("\n");
  }
  bat = hist[len].battery / MAXBAT * 100;
  printf("\nBATT: %s %d%%", ascii_progress(bat), bat);
  if (bat < 20 && bat > 0.1)
    printf(" WARNING!");
  printf("\n");
  printf("\nBUMPER: %c\t\t IFR: %c%s\n",
    hist[len].bumper ? 'D' : ' ', 
    hist[len].infrarred > 0 ? hist[len].infrarred + '0' : ' ',
    hist[len].infrarred > 0 ? "...cleaning" : " ");
}


void visualize(){
  int t;
  for(t = 0; t < config.exec_time; t++){
    system("clear");
    print_path(hist, t);
    printf("Ctrl-C para salir\n");
    system("sleep 0.1");

  }
}


/*
* Pubic actions avialable for the robot
*/


int rmb_awake(int *x, int *y){
  set_base(x, y, &_rob->head);
  //robot reset
  _rob->x = *x;
  _rob->y = *y;
  _rob->battery = MAXBAT;
  _rob->bumper = 0;
  _rob->infrarred = 0;
  r.x = *x;
  r.y = *y;
  tick(0);
  return 1;
}

void rmb_turn(float alpha){
  //_rob->head = fmod(_rob->head + alpha, 2 * M_PI);
  //correction in 23-24
  _rob->head += alpha;
  if(_rob->head < 0)
    _rob->head += 2 * M_PI;
  _rob->head = fmod(_rob->head, 2 * M_PI);
  ////////////////////////////////////////
  _rob->bumper = 0;
  _rob->battery -= 0.1;
  stats.bat_total += 0.1;
  stats.moves[TURN]++;
}

void rmb_forward(){
  char patch;
  float dy, dx;
  int rx, ry, batt;
  dy = rounda(sin(_rob->head));
  dx = rounda(cos(_rob->head));
  rx = (int)(r.x + dx);
  ry = (int)(r.y + dy);
  if(_map.patch[ry][rx] == WALL){
    _rob->bumper = 1;
    batt = 0.5;
    tick(-1);
    stats.moves[BUMP]++;
  }
  else{ 
    r.x += dx;
    r.y += dy; 
    _rob->x = rx;
    _rob->y = ry;
    batt = (dy*dx < 0.0001) ? 1 : 1.4;
    if(_map.patch[_rob->y][_rob->x] == EMPTY)
      _rob->infrarred = 0;
    else if(_map.patch[_rob->y][_rob->x] != 'B')
      _rob->infrarred = _map.patch[_rob->y][_rob->x] - '0';
    stats.moves[FWD]++;
    stats.cell_visited++;
    tick(_rob->infrarred); 
  }
  _rob->battery -= batt;
  stats.bat_total += batt;
}

void rmb_clean(){
  int dirt = _map.patch[_rob->y][_rob->x] - '0';
  if (dirt > 0){
    dirt--;
    _map.patch[_rob->y][_rob->x] = dirt + '0';
    _rob->battery -= 0.5;
    stats.bat_total += 0.5;
    _rob->infrarred = dirt;
    stats.moves[CLE]++;
    if(dirt == 0) stats.dirt_cleaned++;

  }
  tick(dirt);
}


int rmb_load(){
    if(at_base()){
        _rob->battery += 10;
        if (_rob->battery >= MAXBAT){
            _rob->battery = MAXBAT;
            stats.moves[LOAD]++;
        }
        tick(0);
        return 1;
    }
    return 0;
}

sensor_t rmb_state(){
    sensor_t s = r.s;
    return s;
}

int rmb_bumper(){
    return _rob->bumper;
}

int rmb_ifr(){
    return _rob->infrarred;
}

float rmb_battery(){
    return _rob->battery;
}

int rmb_at_base(){
  return at_base();
}