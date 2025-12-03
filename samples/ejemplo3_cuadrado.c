/**
 * Ejemplo 3: Robot que Explora en Cuadrado
 * 
 * Robot que dibuja un patrón cuadrado usando giros de 90 grados.
 * Demuestra el uso de rmb_turn() y control de pasos.
 */

#include "simula.h"
#include <math.h>

int pasos = 0;
int lado_actual = 0;

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    if (pasos < 10) {
        rmb_forward();
        pasos++;
    } else {
        rmb_turn(M_PI / 2);  // Gira 90 grados al completar un lado
        pasos = 0;
        lado_actual++;
        
        if (lado_actual >= 4) {
            lado_actual = 0;  // Comienza nuevo cuadrado
        }
    }
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 200);
    run();
    return 0;
}
