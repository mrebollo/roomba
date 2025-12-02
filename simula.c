/**
 * @file simula.c
 * @brief Núcleo del simulador de Roomba
 * 
 * Contiene la lógica principal de control del simulador, incluyendo
 * configuración, ciclo de ejecución y gestión del historial.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include "simula.h"
#include "simula_internal.h"

#define MIN_BATTERY_THRESHOLD 0.1f    ///< Umbral mínimo de batería para detener simulación
#define MAX_OBSTACLE_DENSITY 0.05f    ///< Densidad máxima de obstáculos aleatorios
#define DEFAULT_DIRT_CELLS 100        ///< Número por defecto de celdas sucias
#define MAX_EXEC_TIME 100000          ///< Tiempo máximo de ejecución (ticks)

/* ============================================================================
 * ESTADO GLOBAL
 * ============================================================================ */

map_t map = {0};                      ///< Mapa del mundo
robot_t r;                            ///< Estado interno del robot
sensor_t *hist = NULL;                ///< Historial de posiciones (memoria dinámica)
config_t config;                      ///< Configuración de la simulación
int timer;                            ///< Contador de ticks
struct sensor *rob = (struct sensor*)&r.sensor;  ///< Puntero público a sensores
int sim_should_stop = 0;              ///< Flag de detención

/* ============================================================================
 * CONTROL DE SIMULACIÓN
 * ============================================================================ */

/**
 * @brief Solicita la detención de la simulación
 * 
 * Establece el flag sim_should_stop para que el bucle principal
 * termine en la siguiente iteración.
 */
void sim_request_stop(void){
  sim_should_stop = 1;
}

/**
 * @brief Registra un tick en el historial
 * @param action Tipo de acción realizada (-1 para no incrementar timer)
 * 
 * Guarda el estado actual del robot en el historial y verifica si
 * la batería está por debajo del umbral crítico.
 */
void sim_log_tick(int action){
  if(timer < config.exec_time){
    hist[timer].x = rob->x;
    hist[timer].y = rob->y;
    hist[timer].heading = rob->heading;
    hist[timer].bumper = rob->bumper;
    hist[timer].infrared = rob->infrared;
    hist[timer].battery = rob->battery;
    if(action != -1) ++timer;
  }
  if(rob->battery < MIN_BATTERY_THRESHOLD)
    sim_request_stop();
}

/* ============================================================================
 * FUNCIONES DE LIMPIEZA (atexit handlers)
 * ============================================================================ */

/**
 * @brief Wrapper para guardar el log al finalizar
 * 
 * Función registrada con atexit() para asegurar que el log
 * se guarda incluso si el programa termina abruptamente.
 */
static void _save_log_wrapper(void){ 
  if(!hist) return;
  save_log(hist, timer); 
}

/**
 * @brief Wrapper para guardar estadísticas al finalizar
 * 
 * Calcula la batería media y guarda las estadísticas finales.
 * Registrada con atexit().
 */
static void _save_stats_wrapper(void){
  if(!hist) return;
  float sum = 0.0f;
  for(int i = 0; i < timer; i++) sum += hist[i].battery;
  stats_set_mean_battery((timer > 0) ? sum / (float)timer : 0.0f);
  save_stats(stats_get());
}

/**
 * @brief Libera la memoria del historial
 * 
 * Función registrada con atexit() para liberar la memoria
 * dinámica del historial al finalizar el programa.
 */
static void _cleanup_hist(void){
  if(hist) {
    free(hist);
    hist = NULL;
  }
}

/* ============================================================================
 * API PÚBLICA
 * ============================================================================ */

/**
 * @brief Configura el simulador antes de ejecutar
 * @param start Función de inicialización (puede ser NULL)
 * @param beh Función de comportamiento cíclico (obligatoria)
 * @param stop Función de finalización (puede ser NULL)
 * @param exec_time Tiempo máximo de ejecución en ticks
 * 
 * Esta función debe llamarse antes de run(). Reserva memoria para el
 * historial, genera un mapa si no hay uno cargado, y registra las
 * funciones de limpieza.
 */
void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time){
  float density;
  if(!beh){
    fprintf(stderr, "Error: Behavior function cannot be NULL\n");
    exit(1);
  }
  srand(time(0));
  config.on_start = start;
  config.exec_beh = beh;
  config.on_stop = stop;
  // if exec_time is out of bounds, set a default value
  config.exec_time = (exec_time > 0 && exec_time <= MAX_EXEC_TIME) 
                     ? exec_time 
                     : WORLDSIZE*WORLDSIZE;
  
  // Reserve memory for history
  if(hist) free(hist);
  hist = (sensor_t*)calloc(config.exec_time, sizeof(sensor_t));
  if(!hist){
    fprintf(stderr, "Error: Cannot allocate memory for history\n");
    exit(1);
  }
  
  // Register cleanup handlers (LIFO order - last registered executes first)
  // _cleanup_hist must be registered BEFORE stop so it executes AFTER
  atexit(_cleanup_hist);
  atexit(_save_log_wrapper);
  atexit(_save_stats_wrapper);
#ifndef COMPETITION_MODE
  if(stop != NULL) atexit(stop);
#endif
  density = rand()/(float)RAND_MAX * MAX_OBSTACLE_DENSITY;
  if(map.name[0] == '\0')
    sim_world_generate(&map, WORLDSIZE, WORLDSIZE, DEFAULT_DIRT_CELLS, density);
  if(map.name[0] == '\0')
    stats_rebuild_from_map(&map);
}

/**
 * @brief Ejecuta la simulación
 * 
 * Ejecuta el bucle principal de simulación. Llama a la función on_start
 * una vez, luego ejecuta exec_beh repetidamente hasta que se alcance el
 * tiempo límite o se solicite detención.
 * 
 * @note Debe llamarse después de configure()
 */
void run(){
  if(!hist){
    fprintf(stderr, "Error: Must call configure() before run()\n");
    exit(1);
  }
  
  timer = 0;
  sim_should_stop = 0;
  if(config.on_start)
    config.on_start();
  while(!sim_should_stop && timer < config.exec_time)
    config.exec_beh();
}

/**
 * @brief Carga un mapa desde un archivo PGM
 * @param filename Ruta del archivo PGM
 * @return 0 si OK, -1 si error
 * 
 * Wrapper de la API pública que carga un mapa y reconstruye
 * las estadísticas basándose en su contenido.
 */
int load_map(char *filename){
  int rc = sim_world_load(&map, filename);
  if(rc == 0) stats_rebuild_from_map(&map);
  return rc;
}
