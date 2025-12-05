/**
 * @brief Generador de mapas de prueba para el simulador Roomba
 * 
 * Genera varios mapas con diferentes configuraciones.
 * Todos los mapas incluyen base aleatoria en las paredes.
 * 
 * Compilación:
 *   make mapgen-dev   (modo desarrollo, desde fuentes)
 *   make lib && make mapgen   (modo distribución, con simula.o)
 * 
 * Uso:
 *   ./maps/mapgen
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../sim_world_api.h"

int main(int argc, char *argv[]){
    srand(time(NULL));
    
    printf("Generating test maps with random bases...\n\n");
    
    map_t* m = map_create();
    if(!m){
        fprintf(stderr, "Error: Cannot create map structure\n");
        return 1;
    }
    
    // Mapa 1: Sin obstáculos, solo suciedad
    printf("Map 1: No obstacles...\n");
    map_generate(m, WORLDSIZE, WORLDSIZE, 50, 0);
    map_save(m, "maps/noobs.pgm");
    
    // Mapas 2-4: Obstáculos dispersos con diferentes densidades
    printf("Map 2: Random obstacles (low density)...\n");
    map_generate(m, WORLDSIZE, WORLDSIZE, 50, 0.01);
    map_save(m, "maps/random1.pgm");
    
    printf("Map 3: Random obstacles (medium density)...\n");
    map_generate(m, WORLDSIZE, WORLDSIZE, 50, 0.03);
    map_save(m, "maps/random3.pgm");
    
    printf("Map 4: Random obstacles (high density)...\n");
    map_generate(m, WORLDSIZE, WORLDSIZE, 50, 0.05);
    map_save(m, "maps/random5.pgm");
    
    // Mapas 5-8: Con muros (1-4 muros de la misma orientación)
    for(int i = 1; i <= 4; i++){
        char filename[64];
        printf("Map %d: %d wall(s)...\n", i+4, i);
        snprintf(filename, sizeof(filename), "maps/walls%d.pgm", i);
        map_generate(m, WORLDSIZE, WORLDSIZE, 50, (float)i);
        map_save(m, filename);
    }
    
    map_destroy(m);
    
    printf("\nMap generation complete\n");
    printf("  All maps have random base positions on walls.\n");
    return 0;
}