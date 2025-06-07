#include "simula_stats.h"
#include <stdio.h>
#include "simula_types.h"
#include "simula_core.h"

// accessor functions for former global variables
#define map (*get_map())
#define r (*get_robot())
#define hist (get_hist())
#define config (*get_config())
#define timer (*get_timer())
#define stats (*get_stats())
#define rob (*get_rob())

// Statistics and logging
void save_log(){
  int i;
  FILE *fd = fopen("log.csv","w");
  if(!fd)
    return;
  fprintf(fd, "y, x, head, bump, ifr, batt\n");
  for(i = 0; i < timer; i++){
    fprintf(fd, "%d, %d, %.1f, %d, %d, %.1f\n", hist[i].y, hist[i].x, hist[i].head * 180.0 / M_PI, 
        hist[i].bumper, hist[i].infrarred, hist[i].battery);  
  }
  fclose(fd);
}

void save_stats(){
  int i;
  FILE *fd = fopen("stats.csv","w");
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

char* ascii_progress(int perc){
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
