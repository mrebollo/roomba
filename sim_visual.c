#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include "simula_internal.h"

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

static void print_map_ascii(){
  for(int i = 0; i < map.nrow; i++){
    for(int j = 0; j < map.ncol; j++)
      printf("%c", map.cells[i][j]);
    printf("\n");
  }
}

static void annotate_dirt_to_map(){
  for(int i = 0; i < map.ndirt; i++){
    dirt_t d = map.dirt[i];
    if(map.name != NULL)
      map.cells[d.y][d.x] = d.depth + '0';
    else
      map.cells[d.y][d.x] = d.depth + '0';
  }
}

static void overlay_path_on_map(sensor_t h[], int len){
  for(int i = 0; i < len; i++)
    map.cells[h[i].y][h[i].x] = '.';
}

static void mark_base_and_end(sensor_t h[], int len){
  map.cells[h[0].y][h[0].x] = 'B';
  map.cells[h[len].y][h[len].x] = 'o';
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
    s->infrared > 0 ? s->infrared + '0' : ' ',
    s->infrared > 0 ? "...cleaning" : " ");
}

static volatile sig_atomic_t g_stop_vis = 0;
static void sigint_vis_handler(int signo){
  (void)signo;
  g_stop_vis = 1;
}

static void print_path(sensor_t h[], int len){
  annotate_dirt_to_map();
  overlay_path_on_map(h, len);
  mark_base_and_end(h, len);
  print_map_ascii();
  print_status_line(&h[len]);
}

void visualize(){
#if ENABLE_VISUALIZATION
  g_stop_vis = 0;
  void (*prev)(int) = signal(SIGINT, sigint_vis_handler);
  for(int t = 0; t < timer && !g_stop_vis; t++){
    // ANSI clear to avoid spawning subshells
    printf("\033[H\033[J");
    fflush(stdout);
    print_path(hist, t);
    printf("Ctrl-C para salir\n");
    const long ms = 100;
    struct timespec ts = { ms/1000, (ms%1000)*1000000L };
    nanosleep(&ts, NULL);
  }
  signal(SIGINT, prev);
#endif
}
