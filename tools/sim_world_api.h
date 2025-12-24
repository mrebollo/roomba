
#ifndef _SIM_WORLD_API_H
#define _SIM_WORLD_API_H

/**
 * @file sim_world_api.h
 * @brief API pública para generación y manipulación de mapas
 * 
 * Este header proporciona una interfaz limpia para crear y guardar
 * mapas sin exponer detalles internos del simulador.
 */

/* ============================================================================
 * CONSTANTES PÚBLICAS
 * ============================================================================ */

#define WORLDSIZE 50              ///< Tamaño máximo del mundo (filas y columnas)

/* ============================================================================
 * TIPOS OPACOS
 * ============================================================================ */

/**
 * @brief Estructura opaca que representa un mapa
 * 
 * Los detalles internos están ocultos. Solo se puede manipular
 * mediante las funciones de la API.
 */
typedef struct _map map_t;

/* ============================================================================
 * API DE GENERACIÓN DE MAPAS
 * ============================================================================ */

/**
 * @brief Crea un nuevo mapa vacío
 * @return Puntero al mapa creado, o NULL si hay error
 * 
 * El mapa debe liberarse con map_destroy() cuando ya no se necesite.
 */
map_t* map_create(void);

/**
 * @brief Libera la memoria de un mapa
 * @param m Puntero al mapa a liberar
 */
void map_destroy(map_t* m);

/**
 * @brief Genera un mapa aleatorio completo
 * @param m Puntero al mapa
 * @param nrow Número de filas (máximo WORLDSIZE)
 * @param ncol Número de columnas (máximo WORLDSIZE)
 * @param num_dirty Número de celdas con suciedad
 * @param nobs Densidad de obstáculos:
 *             - >= 1: número de muros (todos de la misma orientación)
 *             - < 1: densidad de obstáculos dispersos (0.0 a 1.0)
 * @return 0 si OK, -1 si error
 * 
 * Genera un mapa completo con:
 * - Bordes perimetrales
 * - Base aleatoria en una pared (orientación perpendicular)
 * - Obstáculos según el parámetro nobs
 * - Suciedad distribuida aleatoriamente
 */
int map_generate(map_t* m, int nrow, int ncol, int num_dirty, float nobs);

/**
 * @brief Guarda el mapa en formato PGM
 * @param m Puntero al mapa
 * @param filename Nombre del archivo (ej: "mapa1.pgm")
 * @return 0 si OK, -1 si error
 * 
 * Formato PGM (P2):
 * - 128 = obstáculos/muros
 * - 255 = celdas vacías
 * - 0 = base del robot
 * - 1-5 = nivel de suciedad
 */
int map_save(const map_t* m, const char* filename);

/**
 * @brief Carga un mapa desde un archivo PGM
 * @param m Puntero al mapa
 * @param filename Nombre del archivo a cargar
 * @return 0 si OK, -1 si error
 */
int map_load(map_t* m, const char* filename);

/* ============================================================================
 * API DE VISUALIZACIÓN
 * ============================================================================ */

/**
 * @brief Muestra el mapa en formato ASCII por la consola
 * @param m Puntero al mapa a visualizar
 * 
 * Leyenda:
 * - '#' = obstáculos/muros
 * - ' ' = espacio vacío
 * - 'B' = base del robot
 * - '1'-'5' = nivel de suciedad
 */
void map_display(const map_t* m);

#endif /* _SIM_WORLD_API_H */
