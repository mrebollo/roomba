/**
 * Ejemplo 4: Robot que Rebota
 * 
 * Cuando detecta un obstáculo, gira aleatoriamente y continúa.
 * Simula el comportamiento básico de un Roomba real.
 */

#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    srand(time(NULL));  // Inicializa numeros aleatorios
}

void comportamiento() {
    if (rmb_bumper()) {
        // Choco - gira un angulo aleatorio entre 45 y 135 grados
        float angulo = (M_PI / 4) + 
                       (rand() / (float)RAND_MAX) * (M_PI / 2);
        rmb_turn(angulo);
    } else {
        // Libre - avanza
        rmb_forward();
    }
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
