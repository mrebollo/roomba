/**
 * @file sim_io.c
 * @brief Entrada/salida de archivos CSV
 * 
 * Gestiona la escritura de logs y estadísticas en archivos CSV
 * en el directorio de trabajo actual.
 */

#include <stdio.h>
#include "simula_internal.h"

#define STATS_FILE "stats.csv"        ///< Nombre del archivo de estadísticas
#define LOG_FILE   "log.csv"          ///< Nombre del archivo de log
#define RAD_TO_DEG (180.0 / M_PI)     ///< Factor de conversión radianes a grados

/**
 * @brief Guarda el historial de posiciones en log.csv
 * @param hist Array con el historial de estados del robot
 * @param len Número de elementos en el historial
 * 
 * Genera un archivo CSV con columnas: y, x, head, bump, ifr, batt.
 * La orientación se convierte de radianes a grados.
 */
void save_log(const sensor_t *hist, int len){
  if(!hist || len < 0) {
    fprintf(stderr, "Error: Invalid parameters for save_log\n");
    return;
  }
  
  FILE *file = fopen(LOG_FILE,"w");
  if(!file) {
    fprintf(stderr, "Error: Cannot open %s for writing\n", LOG_FILE);
    return;
  }
  
  fprintf(file, "y, x, head, bump, ifr, batt\n");
  for(int i = 0; i < len; i++)
    fprintf(file, "%d, %d, %.1f, %d, %d, %.1f\n",
      hist[i].y, hist[i].x,
      hist[i].heading * RAD_TO_DEG,
      hist[i].bumper,
      hist[i].infrared,
      hist[i].battery);
  fclose(file);
}

/**
 * @brief Guarda las estadísticas finales en stats.csv
 * @param st Puntero a la estructura de estadísticas
 * 
 * Genera un archivo CSV con las métricas de la simulación:
 * celdas totales/visitadas, suciedad total/limpiada, batería total/media,
 * y contadores de movimientos por tipo.
 */
void save_stats(const struct _stat *st){
  if(!st) {
    fprintf(stderr, "Error: Invalid parameters for save_stats\n");
    return;
  }
  
  FILE *file = fopen(STATS_FILE,"w");
  if(!file) {
    fprintf(stderr, "Error: Cannot open %s for writing\n", STATS_FILE);
    return;
  }
  
  fprintf(file, "cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, forward, turn, bumps, clean, load\n");
  fprintf(file, "%d, %d, %d, %d, %.1f, %.1f, %d, %d, %d, %d, %d\n",
    st->cell_total, st->cell_visited, st->dirt_total, st->dirt_cleaned,
    st->bat_total, st->bat_mean,
    st->moves[FWD], st->moves[TURN], st->moves[BUMP], st->moves[CLEAN], st->moves[LOAD]);
  fclose(file);
}
