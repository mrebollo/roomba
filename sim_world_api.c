/**
 * @file sim_world_api.c
 * @brief Implementación de la API pública de mapas
 * 
 * Este archivo actúa como wrapper entre la API pública limpia
 * y las funciones internas del simulador.
 */


/**
 * @file sim_world_api.c
 * @brief Implementación de la API pública de mapas
 *
 * Este archivo actúa como wrapper entre la API pública limpia
 * y las funciones internas del simulador.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sim_world_api.h"
#include "simula_internal.h"


/**
 * @brief Crea un nuevo mapa vacío
 * @return Puntero al mapa creado, o NULL si hay error de memoria
 */
map_t* map_create(void){
    map_t* m = (map_t*)calloc(1, sizeof(map_t));
    return m;
}


/**
 * @brief Libera la memoria de un mapa
 * @param m Puntero al mapa a destruir
 */
void map_destroy(map_t* m){
    if(m) free(m);
}


/**
 * @brief Genera un mapa con dimensiones y suciedad especificadas
 * @param m Puntero al mapa a rellenar
 * @param nrow Número de filas
 * @param ncol Número de columnas
 * @param num_dirty Número de celdas sucias
 * @param nobs Densidad de obstáculos
 * @return 0 si OK, -1 si error
 */
int map_generate(map_t* m, int nrow, int ncol, int num_dirty, float nobs){
    if(!m) return -1;
    return sim_world_generate(m, nrow, ncol, num_dirty, nobs);
}


/**
 * @brief Guarda un mapa en un archivo PGM
 * @param m Puntero al mapa a guardar
 * @param filename Nombre del archivo destino
 * @return 0 si OK, -1 si error
 */
int map_save(const map_t* m, const char* filename){
    if(!m || !filename) return -1;
    FILE *fd = fopen(filename, "w");
    if(!fd){
        fprintf(stderr, "Error: Cannot create %s\n", filename);
        return -1;
    }
    fprintf(fd, "P2\n#roomba map\n%d %d\n%d\n", m->ncol, m->nrow, 255);
    for(int i = 0; i < m->nrow; i++){
        for(int j = 0; j < m->ncol; j++){
            switch(m->cells[i][j]){
                case WALL: fprintf(fd, "%d ", 128); break;
                case EMPTY: fprintf(fd, "%d ", 255); break;
                case 'B': fprintf(fd, "%d ", 0); break;
                default: fprintf(fd, "%d ", m->cells[i][j] - DIGIT_CHAR_BASE);
            }
        }
        fprintf(fd, "\n");
    }
    fclose(fd);
    return 0;
}


/**
 * @brief Carga un mapa desde un archivo PGM
 * @param m Puntero al mapa a cargar
 * @param filename Nombre del archivo origen
 * @return 0 si OK, -1 si error
 */
int map_load(map_t* m, const char* filename){
    if(!m || !filename) return -1;
    return sim_world_load(m, (char*)filename);
}


/**
 * @brief Muestra el mapa por consola en formato ASCII
 * @param m Puntero al mapa a mostrar
 */
void map_display(const map_t* m){
    if(!m) return;
    printf("\n");
    for(int i = 0; i < m->nrow; i++){
        for(int j = 0; j < m->ncol; j++){
            char c = m->cells[i][j];
            switch(c){
                case WALL:  printf("#"); break;
                case EMPTY: printf(" "); break;
                case 'B':   printf("B"); break;
                default:
                    // Dígitos de suciedad
                    if(c >= DIGIT_CHAR_BASE && c <= DIGIT_CHAR_BASE + 5){
                        printf("%c", c);
                    } else {
                        printf(" ");
                    }
            }
        }
        printf("\n");
    }
    printf("\n");
}
