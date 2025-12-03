/**
 * Ejemplo 1: Robot Inmóvil
 * 
 * Robot que solo despierta y se queda quieto.
 * Es el programa más simple para entender la estructura básica.
 */

#include "simula.h"
#include <stdio.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    printf("Robot activado en posicion (%d, %d)\n", x, y);
}

void comportamiento() {
    // No hace nada - el robot esta quieto
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 100);
    run();
    return 0;
}
