/**
 * @file sim_stats.c
 * @brief Módulo de estadísticas de simulación
 * 
 * Encapsula todas las métricas de la simulación en una estructura
 * estática, proporcionando una API limpia para actualizar y consultar
 * estadísticas.
 */

#include <stdio.h>
#include <stdbool.h>
#include "simula_internal.h"

// Stats is now exported for competition_ext.c access
struct _stat stats = {0};     ///< Estadísticas globales
static bool visited[WORLDSIZE][WORLDSIZE] = {0}; ///< Celdas visitadas

/* ============================================================================
 * API PÚBLICA DE ESTADÍSTICAS
 * ============================================================================ */

/**
 * @brief Registra un movimiento en las estadísticas
 * @param kind Tipo de movimiento (enum movement)
 */
void stats_move(int kind){ 
  stats.moves[kind]++; 
}

/**
 * @brief Incrementa el contador de celdas visitadas
 */
void stats_visit_cell(void){
  int x = rob->x;
  int y = rob->y;
  if (x >= 0 && x < WORLDSIZE && y >= 0 && y < WORLDSIZE) {
    if (!visited[y][x]) {
      visited[y][x] = true;
      stats.cell_visited++;
    }
  }
}

/**
 * @brief Registra una colisión (bumper)
 */
void stats_bump(void){ 
  stats.moves[BUMP]++; 
}

/**
 * @brief Registra una acción de limpieza
 * @param before Nivel de suciedad antes de limpiar
 * @param after Nivel de suciedad después de limpiar
 * 
 * Incrementa el contador de acciones de limpieza. Si la celda
 * quedó completamente limpia (after == 0), incrementa dirt_cleaned.
 */
void stats_clean_action(int before, int after){
  stats.moves[CLEAN]++;
  if(before > 0 && after == 0) 
    stats.dirt_cleaned++;
}

/**
 * @brief Acumula consumo de batería
 * @param amount Cantidad de batería consumida
 */
void stats_battery_consume(float amount){ 
  stats.bat_total += amount; 
}

/**
 * @brief Reconstruye estadísticas basándose en el estado del mapa
 * @param m Puntero al mapa
 * 
 * Recalcula cell_total (celdas no-murales) y dirt_total
 * (suma de niveles de suciedad) a partir del mapa actual.
 */
void stats_rebuild_from_map(const map_t* m){
  int cells = 0;
  int dirt_sum = 0;
  // Reset visited matrix y contador de celdas visitadas
  for(int i = 0; i < WORLDSIZE; i++)
    for(int j = 0; j < WORLDSIZE; j++)
      visited[i][j] = false;
  stats.cell_visited = 0;
  for(int i = 0; i < m->nrow; i++){
    for(int j = 0; j < m->ncol; j++){
      char c = m->cells[i][j];
      if(c != WALL) cells++;
      if(c != WALL && c != EMPTY && c != 'B') 
        dirt_sum += (c - DIGIT_CHAR_BASE);
    }
  }
  stats.cell_total = cells;
  stats.dirt_total = dirt_sum;
}

/**
 * @brief Establece la batería media por tick
 * @param mean Valor de batería media
 */
void stats_set_mean_battery(float mean){
  stats.bat_mean = mean;
}

/**
 * @brief Obtiene las estadísticas actuales
 * @return Puntero de solo lectura a la estructura de estadísticas
 */
const struct _stat* stats_get(void){
  return &stats;
}

/**
 * @brief Reinicia todas las estadísticas a cero
 */
void stats_reset(void){
  stats = (struct _stat){0};
  for(int i = 0; i < WORLDSIZE; i++)
    for(int j = 0; j < WORLDSIZE; j++)
      visited[i][j] = false;
}
