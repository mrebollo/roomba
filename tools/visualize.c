
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../simula_internal.h"
#include "../sim_visual.h"

// Prototipo para cargar el log en el historial
int load_log_as_history(const char *filename);

int main(int argc, char *argv[]) {
    const char *logfile = "log.csv";
    const char *mapfile = "map.pgm";
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Visualize - Visualizador de logs de Roomba\n");
            printf("Uso: visualize [log.csv] [map.pgm]\n");
            printf("\nControles durante la animación:\n");
            printf("  Espacio : Pausa/continúa la animación\n");
            printf("  n       : Siguiente frame (en pausa)\n");
            printf("  p       : Frame anterior (en pausa)\n");
            printf("  q       : Salir\n");
            printf("  Ctrl-C  : Salir\n");
            return 0;
        }
        logfile = argv[1];
    }
    if (argc > 2) {
        mapfile = argv[2];
    }
    // Cargar el mapa antes de visualizar
    extern int sim_world_load(map_t *m, char *filename);
    extern float sim_world_put_base(map_t *m, int x, int y);
    if (sim_world_load(&map, (char *)mapfile) != 0) {
        fprintf(stderr, "No se pudo abrir o procesar el mapa: %s\n", mapfile);
        return 2;
    }
    if (!load_log_as_history(logfile)) {
        fprintf(stderr, "No se pudo abrir o procesar el archivo: %s\n", logfile);
        return 1;
    }
    // Ya no se fuerza la base en el mapa: la visualización la marca en la vista
    // ...existing code...
    visualize();
    return 0;
}

// Carga el log en el historial global 'hist' y actualiza 'timer'.
int load_log_as_history(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    // Saltar cabecera
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return 0; }
    // Contar líneas de datos
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file)) count++;
    rewind(file);
    fgets(buffer, sizeof(buffer), file); // Saltar cabecera otra vez
    // Reservar memoria para el historial
    if (hist) free(hist);
    hist = (sensor_t*)malloc(sizeof(sensor_t) * count);
    if (!hist) { fclose(file); return 0; }
    timer = count;
    int idx = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        int x, y, bumper, infrared;
        float heading, battery;
        if (sscanf(buffer, "%d,%d,%f,%d,%d,%f", &y, &x, &heading, &bumper, &infrared, &battery) == 6) {
            // Intercambia x e y para que el recorrido coincida con el mapa
            hist[idx].x = x;
            hist[idx].y = y;
            hist[idx].heading = heading;
            hist[idx].bumper = bumper;
            hist[idx].infrared = infrared;
            hist[idx].battery = battery;
            idx++;
        }
    }
    fclose(file);
    return 1;
}
