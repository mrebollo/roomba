/**
 * @file viewmap.c
 * @brief Visualizador de mapas PGM para el simulador Roomba
 * 
 * Este programa permite ver el contenido de un mapa PGM sin necesidad
 * de ejecutar el simulador completo.
 * 
 * Uso: ./maps/viewmap <archivo.pgm>
 */

#include <stdio.h>
#include <stdlib.h>
#include "../sim_world_api.h"

/**
 * @brief Imprime la ayuda de uso del visualizador de mapas
 * @param prog Nombre del programa
 */
void print_usage(const char* prog){
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
 * @brief Función principal del visualizador de mapas
 *
 * Carga y muestra un mapa PGM en formato ASCII por consola.
 * @param argc Número de argumentos
 * @param argv Vector de argumentos
 * @return 0 si OK, 1 si error
 */
int main(int argc, char* argv[]){
    if(argc != 2){
        print_usage(argv[0]);
        return 1;
    }
    const char* filename = argv[1];
    // Crear y cargar mapa
    map_t* map = map_create();
    if(!map){
        fprintf(stderr, "Error: No se pudo crear el mapa\n");
        return 1;
    }
    if(map_load(map, filename) != 0){
        fprintf(stderr, "Error: No se pudo cargar el archivo '%s'\n", filename);
        map_destroy(map);
        return 1;
    }
    map_display(map);
    map_destroy(map);  
    return 0;
}
