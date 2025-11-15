#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <math.h>
#include <assert.h>
#include <time.h>
#include "simula.h"

// Compilation flags for student vs competition mode
#ifndef COMPETITION_MODE
  #define DEBUG_PRINT(...) printf(__VA_ARGS__)
  #define ENABLE_VISUALIZATION 1
  #define STATS_FILE "stats.csv"
  #define LOG_FILE "log.csv"
#else
  #define DEBUG_PRINT(...) // No output in competition mode
  #define ENABLE_VISUALIZATION 0
  #define STATS_FILE "../stats.csv"
  #define LOG_FILE "log.csv"
#endif

#define WORLDSIZE 50
#define WALL '#'
#define UNKNOWN '.'
#define EMPTY ' '
#define MAXDIRT 5
#define M_PI_8 M_PI / 8

/**
 * @brief Data structure to store dirt positions in the map
 */
typedef struct _dirt{
  int x, y;   /**< Position in the map (x: col, y: row)*/
  int depth;  /**< Dirt amount < 9 */
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
  //sensor must be in the first position to allow casting to sensor_t
  sensor_t s;
  float x, y;
} robot_t;


#define MAXBAT 1000
#define PROGBARLEN 50
#define rounda(a) (round(a*100000) / 100000.0)


map_t map = {0};
robot_t r;
sensor_t hist[WORLDSIZE*WORLDSIZE];
config_t config;
int timer;
struct sensor *rob = (struct sensor*)&r;
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

int generate_map(int, int, int, float);
int load_map(char *);
void print_map();
void save_log();
void save_map();
void save_stats();
float put_base_at(int, int);


/**
 * Control functions
 * 
 */



void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time){
  float density;
  srand(time(0));
  config.on_start = start;
  config.exec_beh = beh;
  config.on_stop = stop;
  config.exec_time = exec_time > 0 && exec_time < WORLDSIZE * WORLDSIZE ? exec_time : WORLDSIZE * WORLDSIZE;
  #ifndef COMPETITION_MODE
  if(stop != NULL)
    atexit(stop);
  #endif
  atexit(save_log);
  atexit(save_stats);

  //map generation (obstacle density until 5%)
  density = rand() / (float)RAND_MAX * 0.05;
  if(map.name == NULL)
    generate_map(WORLDSIZE, WORLDSIZE, 100, density);
}


void run(){
  timer = 0;
  if(config.on_start != NULL)
    config.on_start();
  for(;;)
    config.exec_beh();
}


/*
* Private actions avialable for the robot
*/

void update_sensors(){
  if(map.patch[rob->y][rob->x] != EMPTY && map.patch[rob->y][rob->x] != WALL && map.patch[rob->y][rob->x] != 'B')
    rob->infrarred = map.patch[rob->y][rob->x] - '0';
}

float base_heading(int x, int y){
  // up = 0, down, left, right = 3
  float head[4] = {M_PI / 2, 3 * M_PI / 2, 0, M_PI};
  if(x == 1)
    return head[2];
  else if(x == map.ncol-2)
    return head[3];
  else if(y == 1)
    return head[0];
  else
    return head[1];
} 


// set base next to a wall
void set_base(int *x, int *y, float *h){
    // TODO: cambiarlo para usar put_base_at() y base_heading()
    switch(rand() % 4){
    //left wall
    case 0: *y = 1; *x = rand() % (map.ncol-4) + 2; *h = M_PI / 2; break;
    //right wall
    case 1: *y = map.nrow-2; *x = rand() % (map.ncol-2) + 2; *h = -M_PI / 2; break;
    //bottom wall
    case 2: *x = 1; *y = rand() % (map.nrow-4) + 2; *h = 0; break; 
    //up wall
    case 3: *x = map.ncol-2; *y = rand() % (map.nrow-2) + 2; *h = M_PI; break;
    }
    map.patch[*y][*x] = 'B';
}

float put_base_at(int x, int y){
  map.patch[y][x] = 'B';
  map.bx = x;
  map.by = y;
  return base_heading(x, y);
}


// set base at (1,1)
void set_base_at_origin(int *x, int *y, float *h){
  *x = 1; *y = 1; *h = 0;
  map.patch[*y][*x] = 'B';
} 


int at_base(){
    return map.patch[rob->y][rob->x] == 'B';
}


void save_state(sensor_t *state){
  state->x = rob->x;
  state->y = rob->y;
  state->heading = rob->heading;
  state->bumper = rob->bumper;
  state->infrarred = rob->infrarred;
  state->battery = rob->battery;
}


void tick(int action){
  assert(timer < config.exec_time);
  //update_sensors(); 
  save_state(&hist[timer]);
  /*
  printf("%d, %d, %.1f, %d, %d, %.1f\n", rob->x, rob->y, rob->heading * 180.0 / M_PI, 
        rob->bumper, rob->infrarred, rob->battery);  
        */
  if(rob->battery < 0.1 || (action != -1 && ++timer >= config.exec_time))
    exit(0);
}


void save_log(){
  int i;
  FILE *fd = fopen(LOG_FILE,"w");
  if(!fd)
    return;
  fprintf(fd, "y, x, head, bump, ifr, batt\n");
  for(i = 0; i < timer; i++){ //config.exec_time; i++){
    fprintf(fd, "%d, %d, %.1f, %d, %d, %.1f\n", hist[i].y, hist[i].x, hist[i].heading * 180.0 / M_PI, 
        hist[i].bumper, hist[i].infrarred, hist[i].battery);  
  }
  fclose(fd);
}


void save_stats(){
  int i;
  FILE *fd = fopen(STATS_FILE,"w");
  if(!fd)
    return;
  stats.bat_mean = 0;
  for(i = 0; i < config.exec_time; i++)
    stats.bat_mean += hist[i].battery;
  stats.bat_mean /= config.exec_time;
  fprintf(fd, "cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, \
forward, turn, bumps, clean, load\n");
  fprintf(fd, "%d, %d, %d, %d, %.1f, %.1f, %d, %d, %d, %d, %d\n", 
   stats.cell_total, stats.cell_visited,
   stats.dirt_total, stats.dirt_cleaned,
   stats.bat_total, stats.bat_mean,
   stats.moves[FWD], stats.moves[TURN], stats.moves[BUMP], stats.moves[CLE], stats.moves[LOAD]);  
  fclose(fd);
}


/**
 * Map generation and maintenance
 * 
 */

void save_map(){
  FILE *fd = fopen("map.pgm","w");
  if(!fd) return;

  fprintf(fd,"P2\n#roomba map\n%d %d\n%d\n", map.ncol, map.nrow, 255);
  for(int i = 0; i < map.nrow; i++){
      for(int j = 0; j < map.ncol; j++){ 
        switch(map.patch[i][j]){
          case WALL: fprintf(fd,"%d ", 128); break;
          case EMPTY: fprintf(fd,"%d ", 255); break;
          case 'B': fprintf(fd,"%d ", 0); break;
          default: fprintf(fd,"%d ", map.patch[i][j]-'0'); break;
        }  
        //printf("%c", map.patch[i][j]);
      }
      //printf("\n");  
      fprintf(fd,"\n");
  }
  fclose(fd);
}

int load_map(char *filename){
  int i, j, cell, dc = 0; 
  int nrow, ncol, aux;
  char line[50];
  DEBUG_PRINT("Loading map %s\n", filename);
  FILE *fd = fopen(filename, "r");
  fgets(line, 50, fd); //P2
  fgets(line, 50, fd); //comment
  fscanf(fd,"%d%d", &ncol, &nrow);
  fscanf(fd,"%d", &aux);
  if( nrow > WORLDSIZE || ncol > WORLDSIZE){
    fclose(fd);
    return -1;
  }
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
  len = rand() % map.nrow / 2 + map.nrow  / 4;
  init = rand() % map.nrow  / 2 + 2;
  col = rand() % (map.ncol - 4) + 2;
  for(i = 0; i < len; i++)
    map.patch[init + i][col] = WALL;
  stats.cell_total -= len;
}


void create_horiz_wall(){
  int len, init, row, i;
  len = rand() % map.ncol / 2 + map.ncol  / 4;
  init = rand() % map.ncol  / 2 + 2;
  row = rand() % (map.ncol - 4) + 2;
  for(i = 0; i < len; i++)
    map.patch[row][init + i] = WALL;
  stats.cell_total -= len;
}


void create_random_obstacles(float prop){
  int i, j;
  //avoid obstacles next to a wall
  for(i = 2; i < WORLDSIZE-2; i++)
    for(j = 2; j < WORLDSIZE-2; j++) 
      if(rand() / (float)RAND_MAX < prop){ 
       map.patch[i][j] = WALL;  
             printf("%d, %d\n", i, j); 
      }
} 


int generate_map(int nrow, int ncol, int num_dirty, float nobs){
  int i, j, row, col;
  int numobs;
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


void print_map(){
  int i, j;
  for(i = 0; i < map.nrow; i++){
    for(j = 0; j < map.ncol; j++)
      printf("%c", map.patch[i][j]);
     printf("\n");
  }
}


char *compass(float angle){
  static char dir[3];
  if(angle >= 15 * M_PI_8 || angle < M_PI_8)
    strcpy(dir, "E");
  else if(angle >= M_PI_8 && angle < 3 * M_PI_8)
    strcpy(dir, "SE");
  else if(angle >= 3 * M_PI_8 && angle < 5 * M_PI_8)
    strcpy(dir, "S");
  else if(angle >= 5 * M_PI_8 && angle < 7 * M_PI_8)
    strcpy(dir, "SW");
  else if(angle >= 7 * M_PI_8 && angle < 9 * M_PI_8)
    strcpy(dir, "W");
  else if(angle >= 9 * M_PI_8 && angle < 11 * M_PI_8)
    strcpy(dir, "NW");
  else if(angle >= 11 * M_PI_8 && angle < 13 * M_PI_8)
    strcpy(dir, "N");
  else if(angle >= 13 * M_PI_8 && angle < 15 * M_PI_8)
    strcpy(dir, "NE");
  return dir;
}


void print_path(sensor_t hist[], int len){
  int i, j, bat;
  dirt_t d;
  //add initial dirt to map
  for(i = 0; i < map.ndirt; i++){
    d = map.dirt[i];
    if(map.name != NULL)
      map.patch[d.x][d.y] = d.depth + '0';
    else
      map.patch[d.y][d.x] = d.depth + '0';
  }
  //add path to map
  for(i = 0; i < len; i++)
    map.patch[hist[i].y][hist[i].x] = '.';
  //mark the base again (deleted if robot passes over it)
  map.patch[hist[0].y][hist[0].x] = 'B';
  map.patch[hist[len].y][hist[len].x] = 'o';

  for(i = 0; i < map.nrow; i++){
    for(j = 0; j < map.ncol; j++)
      printf("%c", map.patch[i][j]);
    printf("\n");
  }
  bat = hist[len].battery / MAXBAT * 100;
  printf("\nBATT: %s %d%%", ascii_progress(bat), bat);
  if (bat < 20 && bat > 0.1)
    printf(" WARNING!");
  printf("\n");
  printf("\nPOS (y:%2d,x:%2d)\t\tHEAD: %s (%d)\n",
    hist[len].y, hist[len].x, 
    compass(hist[len].heading),
    (int)(hist[len].heading * 180.0 / M_PI));
  printf("\nBUMPER: %c\t\t IFR: %c%s\n",
    hist[len].bumper ? 'D' : ' ', 
    hist[len].infrarred > 0 ? hist[len].infrarred + '0' : ' ',
    hist[len].infrarred > 0 ? "...cleaning" : " ");
}


void visualize(){
  #if ENABLE_VISUALIZATION
  int t;
  for(t = 0; t < config.exec_time; t++){
    system("clear");
    print_path(hist, t);
    printf("Ctrl-C para salir\n");
    system("sleep 0.1");
  }
  #else
  // Visualization disabled in competition mode
  #endif
}


/*
* Pubic actions avialable for the robot
*/


int rmb_awake(int *x, int *y){
  DEBUG_PRINT("Awaking...\n");
  DEBUG_PRINT("Map: %s\n", map.name);
  if(map.name != NULL){
    rob->heading = put_base_at(map.bx, map.by);
    *x = map.bx;
    *y = map.by;
  }   
  else{
    DEBUG_PRINT("No map loaded\n");
    set_base_at_origin(x, y, &rob->heading);
    save_map();
  } 
  //robot reset
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
  rob->battery -= 0.1;
}


void rmb_forward(){
    float dy, dx;
    int rx, ry;
    float batt;
    dy = rounda(sin(rob->heading));
    dx = rounda(cos(rob->heading));
    //rx = r.x; ry = r.y;
    //r.x += dx;
    //r.y += dy; 
    //rob->x = (int)(r.x + dx);
    //rob->y = (int)(r.y + dy);
    rx = (int)(r.x + dx);
    ry = (int)(r.y + dy);
    //diagonal moves consume 1.4, otherwise 1
    //stats.moves[FWD]++;
    //stats.cell_visited++;
    //batt = rob->battery;
    //rob->battery -= (dy*dx == 0) ? 1 : 1.4;
    // detects a wall
    if(map.patch[ry][rx] == WALL){
      rob->bumper = 1;
      //r.x = rx;
      //r.y = ry;
      //rob->x = (int)r.x;
      //rob->y = (int)r.y;
      //battery consumption if obstacle is just 0.5;
      batt = 0.5;
      //stats.moves[FWD]--;
      //stats.cell_visited--;
      stats.moves[BUMP]++;
      tick(-1);
    }
    else{
      r.x += dx;
      r.y += dy; 
      rob->x = rx;
      rob->y = ry;
      batt = (dy*dx < 0.0001) ? 1 : 1.4;
      //moves to the next position
      if(map.patch[rob->y][rob->x] == EMPTY)
        rob->infrarred = 0;
      else if(map.patch[rob->y][rob->x] != 'B'){
        rob->infrarred = map.patch[rob->y][rob->x] - '0';
      }  
      stats.moves[FWD]++;
      stats.cell_visited++;
      tick(rob->infrarred); 
    } 
    rob->battery -= batt;
    stats.bat_total += batt; 
}

void rmb_clean(){
  int dirt = map.patch[rob->y][rob->x] - '0';
  if (dirt > 0){
    dirt--;
    map.patch[rob->y][rob->x] = dirt + '0';
    rob->battery -= 0.5;
    stats.bat_total += 0.5;
    rob->infrarred = dirt;
    stats.moves[CLE]++;
    if(dirt == 0) stats.dirt_cleaned++;
  }
  tick(dirt);
}


int rmb_load(){
    if(at_base()){
        rob->battery += 10;
        if (rob->battery > MAXBAT) 
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