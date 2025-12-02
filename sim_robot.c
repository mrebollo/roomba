/**
 * @file sim_robot.c
 * @brief Implementación de las acciones del robot
 * 
 * Contiene la API completa del robot (rmb_*) incluyendo movimiento,
 * sensores, limpieza y recarga de batería.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simula_internal.h"

#define LOAD_BATTERY_AMOUNT 10.0f     ///< Cantidad de batería por recarga
#define FLOAT_EPSILON 0.0001f         ///< Tolerancia para comparación de floats

/* ============================================================================
 * FUNCIONES AUXILIARES PRIVADAS
 * ============================================================================ */

/**
 * @brief Guarda el estado actual del robot en una estructura sensor_t
 * @param state Puntero donde guardar el estado
 */
static void save_state(sensor_t *state){
  state->x = rob->x;
  state->y = rob->y;
  state->heading = rob->heading;
  state->bumper = rob->bumper;
  state->infrared = rob->infrared;
  state->battery = rob->battery;
}

/**
 * @brief Registra un tick en el historial
 * @param action Tipo de acción realizada
 */
static void tick(int action){
  sim_log_tick(action);
}

/**
 * @brief Reduce la batería y actualiza estadísticas
 * @param amount Cantidad de batería a consumir
 */
static void apply_battery(float amount){
  rob->battery -= amount;
  stats_battery_consume(amount);
}

/**
 * @brief Actualiza el sensor infrarrojo según la celda actual
 */
static void update_ifr_at_cell(){
  rob->infrared = sim_world_cell_dirt(&map, rob->y, rob->x);
}

/**
 * @brief Calcula los vectores de movimiento según la orientación
 * @param heading Orientación en radianes
 * @param rx Puntero para almacenar nueva posición x entera
 * @param ry Puntero para almacenar nueva posición y entera
 * @param dx Puntero para almacenar desplazamiento x continuo
 * @param dy Puntero para almacenar desplazamiento y continuo
 */
static void step_vectors(float heading, int *rx, int *ry, float *dx, float *dy){
  *dy = rounda(sin(heading));
  *dx = rounda(cos(heading));
  *rx = (int)(r.precise_x + *dx);
  *ry = (int)(r.precise_y + *dy);
}

/**
 * @brief Gestiona una colisión contra un obstáculo
 * 
 * Activa el bumper, registra estadísticas y consume batería.
 */
static void handle_collision(){
  rob->bumper = 1;
  stats_bump();
  tick(-1);
  apply_battery(COST_BUMP);
}

/**
 * @brief Verifica si un movimiento es diagonal
 * @param dy Desplazamiento vertical
 * @param dx Desplazamiento horizontal
 * @return 1 si es diagonal, 0 si no
 */
static int is_diagonal_move(float dy, float dx){
  return fabsf(dy * dx) >= FLOAT_EPSILON;
}

/**
 * @brief Actualiza la posición del robot
 * @param dx Desplazamiento horizontal continuo
 * @param dy Desplazamiento vertical continuo
 * @param rx Nueva posición x entera
 * @param ry Nueva posición y entera
 */
static void update_position(float dx, float dy, int rx, int ry){
  r.precise_x += dx;
  r.precise_y += dy;
  rob->x = rx;
  rob->y = ry;
}

/* ============================================================================
 * API PÚBLICA DEL ROBOT
 * ============================================================================ */

/**
 * @brief Despierta al robot y lo coloca en la base
 * @param x Puntero donde devolver la coordenada x inicial
 * @param y Puntero donde devolver la coordenada y inicial
 * @return 1 si OK, 0 si error
 * 
 * Coloca al robot en la base del mapa (cargado o generado) y
 * establece su orientación inicial. Inicializa la batería al máximo.
 */
int rmb_awake(int *x, int *y){
  if(!x || !y) {
    fprintf(stderr, "Error: Invalid parameters for rmb_awake\n");
    return 0;
  }
  
  DEBUG_PRINT("Awaking...\n");
  DEBUG_PRINT("Map: %s\n", map.name);
  if(map.name[0] != '\0'){
    if(map.base_x >= 0 && map.base_y >= 0){
      rob->heading = sim_world_put_base(&map, map.base_x, map.base_y);
      *x = map.base_x;
      *y = map.base_y;
    } else {
      sim_world_set_base_origin(&map, x, y, &rob->heading);
    }
  } else {
    DEBUG_PRINT("No map loaded\n");
    sim_world_set_base_origin(&map, x, y, &rob->heading);
    sim_world_save(&map);
  }
  
  // Initialize robot position and battery
  r.precise_x = *x;
  r.precise_y = *y;
  rob->x = *x;
  rob->y = *y;
  rob->battery = MAXBAT;
  rob->bumper = 0;
  tick(0);
  return 1;
}

/**
 * @brief Gira el robot
 * @param alpha Ángulo de giro en radianes (positivo = antihorario)
 * 
 * Modifica la orientación del robot y normaliza el ángulo al rango [0, 2π).
 * Consume batería y desactiva el bumper.
 */
void rmb_turn(float alpha){
  rob->heading += alpha;
  if(rob->heading < 0)
    rob->heading += 2 * M_PI;
  rob->heading = fmod(rob->heading, 2 * M_PI);
  rob->bumper = 0;
  apply_battery(COST_TURN);
  stats_move(TURN);
  tick(0);
}

/**
 * @brief Mueve el robot hacia adelante en la dirección actual
 * 
 * Intenta mover el robot un paso en la dirección de su orientación.
 * Si hay un obstáculo, activa el bumper y no se mueve. Los movimientos
 * diagonales consumen más batería que los ortogonales.
 */
void rmb_forward(){
  float dy, dx;
  int rx, ry;
  step_vectors(rob->heading, &rx, &ry, &dx, &dy);
  // Check for wall collision
  if(sim_world_is_wall(&map, ry, rx)){
    handle_collision();
    return;
  }
  // Move successful - update position
  update_position(dx, dy, rx, ry);
  update_ifr_at_cell();
  rob->bumper = 0;
  // Update stats
  stats_move(FWD);
  stats_visit_cell();
  tick(rob->infrared);
  // Apply battery cost (diagonal moves cost more)
  float cost = is_diagonal_move(dy, dx) ? COST_MOVE_DIAG : COST_MOVE;
  apply_battery(cost);
}

/**
 * @brief Limpia la celda actual
 * 
 * Si la celda tiene suciedad, reduce su nivel en 1 y actualiza
 * el sensor infrarrojo. Consume batería solo si había suciedad.
 */
void rmb_clean(){
  int dirt = sim_world_cell_dirt(&map, rob->y, rob->x);
  if(dirt > 0){
    int before = dirt;
    dirt = sim_world_clean_cell(&map, rob->y, rob->x);
    apply_battery(COST_CLEAN);
    rob->infrared = dirt;
    stats_clean_action(before, dirt);
  }
  tick(dirt);
}

/**
 * @brief Recarga la batería si el robot está en la base
 * @return 1 si estaba en la base y recargó, 0 si no
 * 
 * Incrementa la batería en LOAD_BATTERY_AMOUNT sin superar MAXBAT.
 */
int rmb_load(){
  if(sim_world_is_base(&map, rob->y, rob->x)){
    rob->battery += LOAD_BATTERY_AMOUNT;
    if(rob->battery > MAXBAT)
      rob->battery = MAXBAT;
    tick(0);
    return 1;
  }
  stats_move(LOAD);
  return 0;
}

/**
 * @brief Obtiene una copia del estado actual de los sensores
 * @return Estructura sensor_t con el estado actual
 */
sensor_t rmb_state(){
  sensor_t s;
  save_state(&s);
  return s;
}

/**
 * @brief Lee el estado del bumper
 * @return 1 si hay colisión detectada, 0 si libre
 */
int rmb_bumper(){
  return rob->bumper;
}

/**
 * @brief Lee el sensor infrarrojo
 * @return Nivel de suciedad en la celda actual (0 = limpio)
 */
int rmb_ifr(){
  return rob->infrared;
}

/**
 * @brief Lee el nivel de batería
 * @return Batería disponible (0.0 a MAXBAT)
 */
float rmb_battery(){
  return rob->battery;
}

/**
 * @brief Verifica si el robot está en la base
 * @return 1 si está en la base, 0 si no
 */
int rmb_at_base(){
  return sim_world_is_base(&map, rob->y, rob->x);
}

