/**
 * @file viewmap.c
 * @brief PGM map visualizer for Roomba simulator
 *
 * Allows viewing PGM map content without running the full simulator.
 *
 * Usage: ./tools/viewmap <file.pgm>
 */

#include "../sim_world_api.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Print visualizer usage help
 * @param prog Program name
 */
void print_usage(const char *prog) {
  printf("\n");
  printf("Uso: %s <archivo.pgm>\n", prog);
  printf("\n");
  printf("Visualiza un mapa del simulador Roomba en formato ASCII.\n");
  printf("\n");
  printf("Leyenda:\n");
  printf("  #     = Obstáculo/muro\n");
  printf("  B     = Base del robot\n");
  printf("  1-5   = Nivel de suciedad\n");
  printf("  (espacio) = Celda vacía\n");
  printf("\n");
}

/**
 * @brief Map visualizer main function
 *
 * Loads and displays a PGM map in ASCII format to console.
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on OK, 1 on error
 */
int main(int argc, char *argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return 1;
  }
  const char *filename = argv[1];
  // Crear y cargar mapa
  map_t *map = map_create();
  if (!map) {
    fprintf(stderr, "Error: No se pudo crear el mapa\n");
    return 1;
  }
  if (map_load(map, filename) != 0) {
    fprintf(stderr, "Error: No se pudo cargar el archivo '%s'\n", filename);
    map_destroy(map);
    return 1;
  }
  map_display(map);
  map_destroy(map);
  return 0;
}
