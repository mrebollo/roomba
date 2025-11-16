#include <stdio.h>
#include "simula_internal.h"

// Always write logs/stats to current working directory
#define STATS_FILE "stats.csv"
#define LOG_FILE   "log.csv"

void save_log(const sensor_t *hist, int len){
  FILE *file = fopen(LOG_FILE,"w");
  if(!file) return;
  fprintf(file, "y, x, head, bump, ifr, batt\n");
  for(int i = 0; i < len; i++)
    fprintf(file, "%d, %d, %.1f, %d, %d, %.1f\n",
      hist[i].y, hist[i].x,
      hist[i].heading * 180.0 / M_PI,
      hist[i].bumper,
      hist[i].infrared,
      hist[i].battery);
  fclose(file);
}

void save_stats(const struct _stat *st){
  FILE *file = fopen(STATS_FILE,"w");
  if(!file) return;
  fprintf(file, "cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, forward, turn, bumps, clean, load\n");
   fprintf(file, "%d, %d, %d, %d, %.1f, %.1f, %d, %d, %d, %d, %d\n",
     st->cell_total, st->cell_visited, st->dirt_total, st->dirt_cleaned,
     st->bat_total, st->bat_mean,
     st->moves[FWD], st->moves[TURN], st->moves[BUMP], st->moves[CLEAN], st->moves[LOAD]);
  fclose(file);
}
