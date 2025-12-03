/**
 * Ejemplo 2: Robot que Avanza
 * 
 * Robot que avanza en línea recta hasta chocar con una pared.
 * Demuestra el movimiento básico.
 */

#include "simula.h"

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    rmb_forward();  // Avanza un paso en cada ciclo
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 50);
    run();
    return 0;
}
