#pragma once

/**
 * @file simula_internal.h
 * @brief API interna del simulador de Roomba
 * 
 * Este header contiene todas las estructuras de datos, constantes y 
 * declaraciones de funciones compartidas entre los módulos internos
 * del simulador (sim_*.c).
 */

#include <stdio.h>
#include <math.h>
#include "simula.h"

/* ============================================================================
 * CONFIGURACIÓN Y DEBUG
 * ============================================================================ */

#ifndef COMPETITION_MODE
  #define DEBUG_PRINT(...) printf(__VA_ARGS__)
  #define ENABLE_VISUALIZATION 1
#else
  #define DEBUG_PRINT(...)
  #define ENABLE_VISUALIZATION 0
#endif

/* ============================================================================
 * CONSTANTES DEL MUNDO
 * ============================================================================ */

#define WORLDSIZE 50              ///< Tamaño máximo del mundo (filas y columnas)
#define WALL '#'                  ///< Carácter que representa un obstáculo
#define EMPTY ' '                 ///< Carácter que representa una celda vacía
#define MAXDIRT 5                 ///< Nivel máximo de suciedad por celda
#define M_PI_8 ((M_PI) / 8)       ///< PI/8 para cálculos de orientación
#define DIGIT_CHAR_BASE '0'       ///< Base para conversión de dígitos a caracteres

/* ============================================================================
 * CONSTANTES DE BATERÍA Y COSTES
 * ============================================================================ */

#define MAXBAT 1000               ///< Batería máxima del robot
#define PROGBARLEN 50             ///< Longitud de la barra de progreso
#define rounda(a) (round((a)*100000) / 100000.0)  ///< Redondeo a 5 decimales

/// Costes de batería por acción
#define COST_TURN 0.1f            ///< Coste de girar
#define COST_MOVE 1.0f            ///< Coste de mover en línea recta
#define COST_MOVE_DIAG 1.4f       ///< Coste de mover en diagonal
#define COST_BUMP 0.5f            ///< Coste de colisión
#define COST_CLEAN 0.5f           ///< Coste de limpiar

/* ============================================================================
 * ESTRUCTURAS DE DATOS
 * ============================================================================ */

/**
 * @brief Representa una celda sucia en el mapa
 */
typedef struct _dirt{
  int x, y;                       ///< Coordenadas de la celda sucia
  int depth;                      ///< Nivel de suciedad (1-MAXDIRT)
} dirt_t;

/**
 * @brief Estructura principal que representa el mapa del mundo
 */
typedef struct _map{
  char cells[WORLDSIZE][WORLDSIZE];  ///< Matriz de celdas (WALL o EMPTY)
  int nrow, ncol;                    ///< Dimensiones reales del mapa
  dirt_t dirt[WORLDSIZE*WORLDSIZE];  ///< Lista de celdas sucias
  int ndirt;                         ///< Número de celdas sucias
  char name[256];                    ///< Nombre del archivo del mapa
  int base_x, base_y;                ///< Coordenadas de la base
} map_t;

/**
 * @brief Configuración de la simulación
 */
typedef struct _config{
  void (*on_start)();             ///< Función de inicialización
  void (*exec_beh)();             ///< Función de comportamiento (cíclica)
  void (*on_stop)();              ///< Función de finalización
  FILE *output;                   ///< Stream de salida para logs
  int exec_time;                  ///< Tiempo máximo de ejecución (ticks)
} config_t;

/**
 * @brief Estado interno del robot (posición precisa)
 */
typedef struct _robot{
  float precise_x, precise_y;     ///< Posición en coordenadas continuas
  sensor_t sensor;                ///< Lecturas actuales de los sensores
} robot_t;

/**
 * @brief Tipos de movimientos para estadísticas
 */
enum movement {FWD, TURN, BUMP, CLEAN, LOAD};

/**
 * @brief Estadísticas de la simulación
 */
struct _stat{
  int cell_total;                 ///< Total de celdas en el mapa
  int cell_visited;               ///< Número de celdas visitadas
  int dirt_total;                 ///< Total de suciedad inicial
  int dirt_cleaned;               ///< Total de suciedad limpiada
  float bat_total;                ///< Batería total consumida
  float bat_mean;                 ///< Batería media por tick
  int moves[5];                   ///< Contador de movimientos por tipo
};

/* ============================================================================
 * ESTADO GLOBAL (definido en simula.c)
 * ============================================================================ */

extern map_t map;                 ///< Mapa del mundo
extern robot_t r;                 ///< Estado interno del robot
extern sensor_t *hist;            ///< Historial de posiciones (memoria dinámica)
extern config_t config;           ///< Configuración de la simulación
extern int timer;                 ///< Contador de ticks
extern struct sensor *rob;        ///< Puntero público a sensores (apunta a r.sensor)

/* ============================================================================
 * MÓDULO: CORE (simula.c)
 * ============================================================================ */

/**
 * @brief Solicita la detención de la simulación
 */
void sim_request_stop(void);

/**
 * @brief Flag que indica si se debe detener la simulación
 */
extern int sim_should_stop;

/**
 * @brief Registra un tick en el historial
 * @param action Tipo de acción realizada (enum movement)
 */
void sim_log_tick(int action);

/* ============================================================================
 * MÓDULO: ESTADÍSTICAS (sim_stats.c)
 * ============================================================================ */

/**
 * @brief Registra un movimiento en las estadísticas
 * @param kind Tipo de movimiento (enum movement)
 */
void stats_move(int kind);

/**
 * @brief Marca una celda como visitada
 */
void stats_visit_cell(void);

/**
 * @brief Registra una colisión
 */
void stats_bump(void);

/**
 * @brief Registra una acción de limpieza
 * @param before Nivel de suciedad antes de limpiar
 * @param after Nivel de suciedad después de limpiar
 */
void stats_clean_action(int before, int after);

/**
 * @brief Registra consumo de batería
 * @param amount Cantidad de batería consumida
 */
void stats_battery_consume(float amount);

/**
 * @brief Reconstruye estadísticas basándose en el estado actual del mapa
 * @param m Puntero al mapa
 */
void stats_rebuild_from_map(const map_t* m);

/**
 * @brief Establece la batería media por tick
 * @param mean Valor de batería media
 */
void stats_set_mean_battery(float mean);

/**
 * @brief Obtiene las estadísticas actuales
 * @return Puntero a la estructura de estadísticas (solo lectura)
 */
const struct _stat* stats_get(void);

/**
 * @brief Reinicia todas las estadísticas a cero
 */
void stats_reset(void);

/* ============================================================================
 * MÓDULO: ENTRADA/SALIDA (sim_io.c)
 * ============================================================================ */

/**
 * @brief Guarda el historial de posiciones en log.csv
 * @param hist Array de posiciones visitadas
 * @param len Longitud del array
 */
void save_log(const sensor_t *hist, int len);

/**
 * @brief Guarda las estadísticas finales en stats.csv
 * @param st Puntero a la estructura de estadísticas
 */
void save_stats(const struct _stat *st);

/* ============================================================================
 * MÓDULO: MUNDO (sim_world.c)
 * ============================================================================ */

/**
 * @brief Genera un mapa aleatorio
 * @param m Puntero al mapa a generar
 * @param nrow Número de filas
 * @param ncol Número de columnas
 * @param num_dirty Número de celdas sucias
 * @param nobs Densidad de obstáculos (0.0 a 1.0)
 * @return 0 si OK, -1 si error
 */
int sim_world_generate(map_t* m, int nrow, int ncol, int num_dirty, float nobs);

/**
 * @brief Carga un mapa desde un archivo PGM
 * @param m Puntero al mapa
 * @param filename Ruta del archivo
 * @return 0 si OK, -1 si error
 */
int sim_world_load(map_t* m, char *filename);

/**
 * @brief Guarda el mapa actual en formato PGM
 * @param m Puntero al mapa
 */
void sim_world_save(const map_t* m);

/**
 * @brief Coloca la base en una posición específica
 * @param m Puntero al mapa
 * @param x Columna
 * @param y Fila
 * @return Distancia desde la posición al centro del mapa
 */
float sim_world_put_base(map_t* m, int x, int y);

/**
 * @brief Establece la posición inicial del robot en la base
 * @param m Puntero al mapa
 * @param x Puntero donde guardar la columna inicial
 * @param y Puntero donde guardar la fila inicial
 * @param h Puntero donde guardar la orientación inicial
 */
void sim_world_set_base_origin(map_t* m, int *x, int *y, float *h);

/**
 * @brief Verifica si una celda es un obstáculo
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return 1 si es obstáculo, 0 si no
 */
int sim_world_is_wall(const map_t* m, int y, int x);

/**
 * @brief Verifica si una celda es la base
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return 1 si es la base, 0 si no
 */
int sim_world_is_base(const map_t* m, int y, int x);

/**
 * @brief Obtiene el nivel de suciedad de una celda
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return Nivel de suciedad (0 = limpio)
 */
int sim_world_cell_dirt(const map_t* m, int y, int x);

/**
 * @brief Establece el nivel de suciedad de una celda
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @param dirt Nivel de suciedad a establecer
 */
void sim_world_set_cell_dirt(map_t* m, int y, int x, int dirt);

/**
 * @brief Limpia una celda (reduce suciedad en 1)
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return Nivel de suciedad después de limpiar
 */
int sim_world_clean_cell(map_t* m, int y, int x);

/* ============================================================================
 * MÓDULO: ROBOT (sim_robot.c)
 * ============================================================================ */

/* Las funciones del robot están declaradas en simula.h (API pública) */

/* ============================================================================
 * MÓDULO: VISUALIZACIÓN (sim_visual.c)
 * ============================================================================ */

/**
 * @brief Muestra una representación visual del estado actual de la simulación
 */
void visualize(void);

/* ============================================================================
 * EXPORTS FOR COMPETITION EXTENSIONS
 * ============================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Exported symbols for competition_ext.c
 * 
 * These allow competition extensions to access core simulator state
 * and functions without modifying simula.c.
 */

// Core state (already declared above, but ensuring visibility)
// extern map_t map;
// extern robot_t r;
// extern sensor_t *hist;
// extern config_t config;
// extern int timer;

/**
 * @brief Statistics structure (for competition stats export)
 */
extern struct _stat stats;

/**
 * @brief Load a map from file (needed by competition_ext.c)
 * @param filename Path to PGM map file
 * @return 0 if OK, -1 if error
 */
int load_map(char* filename);

#ifdef __cplusplus
}
#endif
