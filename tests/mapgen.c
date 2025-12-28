// mapgen.c: Generador de mapas PGM para tests unitarios
// Uso: ./mapgen <archivo_salida> <tam> [base_x base_y] [suciedad_x suciedad_y ...]
// Si no se indican base_x y base_y, no se coloca base
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_VAL 0
#define WALL_VAL 128
#define EMPTY_VAL 255
#define DIRT_VAL 1

int main(int argc, char *argv[]) {
    if (argc < 3 || !argv[1] || !argv[2]) {
        fprintf(stderr, "Uso: %s <archivo_salida> <tam> [base_x base_y] [suciedad_x suciedad_y ...]\n", argv[0]);
        return 1;
    }
    const char *fname = argv[1];
    int N = atoi(argv[2]);
    int bx = -1, by = -1;
    int argi = 3;
    if (argc >= 5) {
        bx = atoi(argv[3]);
        by = atoi(argv[4]);
        argi = 5;
    }
    if (N < 3) {
        fprintf(stderr, "El tamaño debe ser >= 3\n");
        return 1;
    }
    unsigned char **map = malloc(N * sizeof(unsigned char*));
    for (int y = 0; y < N; ++y)
        map[y] = malloc(N * sizeof(unsigned char));
    // Inicializa todo vacío
    for (int y = 0; y < N; ++y)
        for (int x = 0; x < N; ++x)
            map[y][x] = EMPTY_VAL;
    // Bordes como muros
    for (int i = 0; i < N; ++i) {
        map[0][i] = WALL_VAL;
        map[N-1][i] = WALL_VAL;
        map[i][0] = WALL_VAL;
        map[i][N-1] = WALL_VAL;
    }
    // Base (si se indica)
    if (bx >= 0 && by >= 0 && bx < N && by < N)
        map[by][bx] = BASE_VAL;
    // Suciedad
    for (int i = argi; i+1 < argc; i += 2) {
        int dx = atoi(argv[i]);
        int dy = atoi(argv[i+1]);
        if (dx > 0 && dx < N-1 && dy > 0 && dy < N-1)
            map[dy][dx] = DIRT_VAL;
    }
    // Escribe PGM
    FILE *f = fopen(fname, "w");
    if (!f) { perror("fopen"); return 3; }
    fprintf(f, "P2\n# %s %dx%d generado por mapgen\n%d %d\n255\n", fname, N, N, N, N);
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x)
            fprintf(f, "%d ", map[y][x]);
        fprintf(f, "\n");
    }
    fclose(f);
    for (int y = 0; y < N; ++y) free(map[y]);
    free(map);
    return 0;
}
