/**
 * @file sim_visual.c
 * @brief Visualización ASCII de la simulación
 * 
 * Proporciona representación visual en terminal del recorrido del robot,
 * incluyendo mapa, camino, estado de sensores y batería.
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

/* ============================================================================
 * FUNCIONES AUXILIARES DE VISUALIZACIÓN
 * ============================================================================ */

/**
 * @brief Genera una barra de progreso ASCII
 * @param perc Porcentaje (0-100)
 * @return String estático con la barra de progreso [====    ]
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
 * @brief Convierte un ángulo en radianes a punto cardinal
 * @param angle Ángulo en radianes
 * @return String con la dirección (E, SE, S, SW, W, NW, N, NE)
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
 * @param view Matriz de caracteres con el mapa a imprimir
 */
static void print_map_ascii(char view[WORLDSIZE][WORLDSIZE]){
  for(int i = 0; i < map.nrow; i++){
    for(int j = 0; j < map.ncol; j++)
      printf("%c", view[i][j]);
    printf("\n");
  }
}

/**
 * @brief Añade marcadores de suciedad al mapa
 * @param view Matriz donde añadir los marcadores
 * 
 * Marca cada celda sucia con un dígito indicando su nivel de suciedad.
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
 * @brief Superpone el camino recorrido sobre el mapa
 * @param view Matriz donde dibujar el camino
 * @param h Array con el historial de posiciones
 * @param len Número de posiciones en el historial
 * 
 * Marca con '.' cada celda visitada por el robot.
 */
static void overlay_path_on_map(char view[WORLDSIZE][WORLDSIZE], sensor_t h[], int len){
  for(int i = 0; i < len; i++){
    if(h[i].y >= 0 && h[i].y < map.nrow && h[i].x >= 0 && h[i].x < map.ncol){
      view[h[i].y][h[i].x] = '.';
    }
  }
}

/**
 * @brief Marca la base y posición final del robot
 * @param view Matriz donde añadir las marcas
 * @param h Array con el historial de posiciones
 * @param len Número de posiciones en el historial
 * 
 * Marca con 'B' la posición inicial (base) y con 'o' la posición final.
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
 * @brief Imprime la línea de estado del robot
 * @param s Puntero al estado del sensor a mostrar
 * 
 * Muestra batería con barra de progreso, posición, orientación,
 * estado del bumper y nivel de suciedad detectado.
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

static volatile sig_atomic_t g_stop_vis = 0;  ///< Flag para interrumpir visualización

/**
 * @brief Manejador de SIGINT para detener visualización
 * @param signo Número de señal (no usado)
 */
static void sigint_vis_handler(int signo){
  (void)signo;
  g_stop_vis = 1;
}

/**
 * @brief Dibuja el mapa con el camino hasta el tick especificado
 * @param view Buffer del mapa
 * @param h Historial de posiciones
 * @param len Número de ticks a mostrar
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
 * @brief Visualiza la simulación de forma animada
 * 
 * Reproduce el recorrido del robot frame a frame, mostrando el mapa
 * actualizado con el camino recorrido y el estado de los sensores.
 * Solo está activa si ENABLE_VISUALIZATION está definido.
 * Se puede interrumpir con Ctrl-C.
 */
void visualize(){
#if ENABLE_VISUALIZATION
  g_stop_vis = 0;
  void (*prev)(int) = signal(SIGINT, sigint_vis_handler);
  for(int t = 0; t < timer && !g_stop_vis; t++){
    // ANSI clear to avoid spawning subshells
    printf("\033[H\033[J");
    fflush(stdout);
    char view[WORLDSIZE][WORLDSIZE];
    // copy current map to a local view buffer
    for(int i = 0; i < map.nrow; i++)
      for(int j = 0; j < map.ncol; j++)
        view[i][j] = map.cells[i][j];
    print_path(view, hist, t);
    printf("Ctrl-C para salir\n");
    const long ms = VISUALIZATION_DELAY_MS;
    struct timespec ts = { ms/1000, (ms%1000)*1000000L };
    nanosleep(&ts, NULL);
  }
  signal(SIGINT, prev);
#endif
}
