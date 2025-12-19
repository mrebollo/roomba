#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
/**
 * @file sim_visual.c
 * @brief Visualización de mapas y trayectorias para el simulador Roomba
 *
 * Proporciona funciones para mostrar el mapa, el recorrido del robot y el estado
 * de la simulación en modo texto (ASCII) y animado en consola.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include "simula_internal.h"

#define RAD_TO_DEG (180.0 / M_PI)           ///< Conversión radianes a grados
#define VISUALIZATION_DELAY_MS 100          ///< Retardo entre frames (ms)
#define COMPASS_BUF_SIZE 4                  ///< Tamaño buffer para puntos cardinales

/**
 * @brief Genera una barra de progreso ASCII
 * @param perc Porcentaje de progreso (0-100)
 * @return Cadena estática con la barra de progreso
 */
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

/**
 * @brief Devuelve la dirección cardinal según el ángulo
 * @param angle Ángulo en radianes
 * @return Cadena con la dirección cardinal (E, SE, S, ...)
 */
static char *compass(float angle){
  static char dir[COMPASS_BUF_SIZE];
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

/**
 * @brief Imprime el mapa en formato ASCII
 * @param view Matriz de caracteres a mostrar
 */
static void print_map_ascii(char view[WORLDSIZE][WORLDSIZE]){
  int max_width = map.ncol > PROGBARLEN ? map.ncol : PROGBARLEN;
  for(int i = 0; i < map.nrow; i++){
    int j = 0;
    for(; j < map.ncol; j++)
      printf("%c", view[i][j]);
    for(; j < max_width; j++)
      printf(" ");
    printf("\n");
  }
}

/**
 * @brief Anota la suciedad en el mapa de visualización
 * @param view Matriz de caracteres a modificar
 */
static void annotate_dirt_to_map(char view[WORLDSIZE][WORLDSIZE]){
  for(int i = 0; i < map.ndirt; i++){
    dirt_t d = map.dirt[i];
    if(d.y >= 0 && d.y < map.nrow && d.x >= 0 && d.x < map.ncol){
      view[d.y][d.x] = (char)(DIGIT_CHAR_BASE + d.depth);
    }
  }
}

/**
 * @brief Dibuja el recorrido del robot sobre el mapa
 * @param view Matriz de caracteres a modificar
 * @param h Historial de posiciones
 * @param len Longitud del historial
 */
static void overlay_path_on_map(char view[WORLDSIZE][WORLDSIZE], sensor_t h[], int len){
  for(int i = 0; i < len; i++){
    if(h[i].y >= 0 && h[i].y < map.nrow && h[i].x >= 0 && h[i].x < map.ncol){
      view[h[i].y][h[i].x] = '.';
    }
  }
}

/**
 * @brief Marca la base y el final del recorrido en el mapa
 * @param view Matriz de caracteres a modificar
 * @param h Historial de posiciones
 * @param len Longitud del historial
 */
static void mark_base_and_end(char view[WORLDSIZE][WORLDSIZE], sensor_t h[], int len){
  if(len > 0){
    if(h[0].y >= 0 && h[0].y < map.nrow && h[0].x >= 0 && h[0].x < map.ncol){
      view[h[0].y][h[0].x] = 'B';
    }
    if(h[len-1].y >= 0 && h[len-1].y < map.nrow && h[len-1].x >= 0 && h[len-1].x < map.ncol){
      view[h[len-1].y][h[len-1].x] = 'o';
    }
  }
}

/**
 * @brief Imprime el estado actual del robot (batería, posición, sensores)
 * @param s Puntero a la estructura de sensores actual
 */
static void print_status_line(sensor_t *s){
  int bat = s->battery / MAXBAT * 100;
  printf("\nBATT: %s %d%%", ascii_progress(bat), bat);
  if (bat < 20 && bat > 0.1)
    printf(" WARNING!");
  printf("\n\nPOS (y:%2d,x:%2d)\t\tHEAD: %s (%d)\n",
    s->y, s->x, compass(s->heading), (int)(s->heading * RAD_TO_DEG));
  printf("\nBUMPER: %c\t\t IFR: %c%s\n",
    s->bumper ? 'D' : ' ',
    s->infrared > 0 ? s->infrared + DIGIT_CHAR_BASE : ' ',
    s->infrared > 0 ? "...cleaning" : " ");
}

static volatile sig_atomic_t g_stop_vis = 0;

/**
 * @brief Manejador de señal SIGINT para detener la visualización
 * @param signo Número de señal (no usado)
 */
static void sigint_vis_handler(int signo){
  (void)signo;
  g_stop_vis = 1;
}

/**
 * @brief Imprime el mapa con el recorrido y estado actual
 * @param view Matriz de caracteres a mostrar
 * @param h Historial de posiciones
 * @param len Longitud del historial
 */
static void print_path(char view[WORLDSIZE][WORLDSIZE], sensor_t h[], int len){
  annotate_dirt_to_map(view);
  overlay_path_on_map(view, h, len);
  mark_base_and_end(view, h, len);
  print_map_ascii(view);
  if(len > 0){
    print_status_line(&h[len-1]);
  }
}

/**
 * @brief Visualiza la simulación paso a paso en consola
 *
 * Muestra el mapa, el recorrido y el estado del robot en modo animado.
 * Requiere que ENABLE_VISUALIZATION esté activado.
 */
void visualize(){
#if ENABLE_VISUALIZATION
  if(!hist){
    printf("No history available for visualization\n");
    return;
  }
  g_stop_vis = 0;
  void (*prev)(int) = signal(SIGINT, sigint_vis_handler);
  // Configurar terminal en modo no canónico
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  printf("\033[?1049h\033[?25l\033[2J");
  fflush(stdout);
  int t = 0;
  int paused = 0;
    printf("\n[Espacio]=pausa, S=siguiente, A=anterior, q=salir\n");
  while (t < timer && !g_stop_vis) {
    if (!paused) {
      printf("\033[2J");
      printf("\033[H");
      char view[WORLDSIZE][WORLDSIZE];
      for(int i = 0; i < map.nrow; i++)
        for(int j = 0; j < map.ncol; j++)
          view[i][j] = map.cells[i][j];
      print_path(view, hist, t);
      printf("\n[Espacio]=pausa, S=siguiente, A=anterior, q=salir\n");
      fflush(stdout);
      const long ms = VISUALIZATION_DELAY_MS;
      struct timespec ts = { ms/1000, (ms%1000)*1000000L };
      nanosleep(&ts, NULL);
      int c = getchar();
      if (c != EOF) {
        if (c == 'q' || c == 'Q') { g_stop_vis = 1; break; }
        if (c == ' ') paused = !paused;
      }
      if (!paused && !g_stop_vis) t++;
    } else {
      int redraw = 1;
      while (paused && !g_stop_vis) {
        if (redraw) {
          printf("\033[2J");
          printf("\033[H");
          char view[WORLDSIZE][WORLDSIZE];
          for(int i = 0; i < map.nrow; i++)
            for(int j = 0; j < map.ncol; j++)
              view[i][j] = map.cells[i][j];
          print_path(view, hist, t);
          printf("\n[Espacio]=pausa, S=siguiente, A=anterior, q=salir\n");
          fflush(stdout);
          redraw = 0;
        }
        int c = getchar();
        if (c != EOF) {
          if (c == 'q' || c == 'Q') { g_stop_vis = 1; break; }
          if (c == ' ') { paused = 0; break; }
            if (c == 's' || c == 'S') { if (t < timer-1) { t++; redraw = 1; } }
            if (c == 'a' || c == 'A') { if (t > 1) { t--; redraw = 1; } }
        }
        struct timespec ts = { 0, 10000000L }; // 10ms
        nanosleep(&ts, NULL);
      }
    }
  }

  // Restaurar terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
  // Antes de restaurar el buffer normal, guardar el último frame
  int last_tick = (timer < 1) ? 0 : (timer-1);
  char view[WORLDSIZE][WORLDSIZE];
  for(int i = 0; i < map.nrow; i++)
    for(int j = 0; j < map.ncol; j++)
      view[i][j] = map.cells[i][j];

  // Restaurar buffer normal y cursor
  printf("\033[?25h\033[?1049l");
  fflush(stdout);
  signal(SIGINT, prev);

  // Imprimir el último frame en el buffer normal
  print_path(view, hist, last_tick);
  printf("\n--- Simulación finalizada ---\n");
  printf("\n[Espacio]=pausa, S=siguiente, A=anterior, q=salir\n");
  fflush(stdout);
  #endif
}
/* --- FIN DEL ARCHIVO ORIGINAL --- */
