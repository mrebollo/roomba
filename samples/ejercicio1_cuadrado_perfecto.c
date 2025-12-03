/**
 * Ejercicio 1: Cuadrado Perfecto
 * Nivel: Básico
 * 
 * Objetivo: Haz que el robot dibuje un cuadrado de 10x10 celdas 
 * sin salirse del trazado.
 * 
 * Pistas:
 * - Usa rmb_forward() para avanzar 10 pasos
 * - Usa rmb_turn(M_PI/2) para girar 90 grados
 * - Cuenta los pasos y lados recorridos
 * - Detente después de completar 4 lados
 */

#include "simula.h"
#include <math.h>

// TODO: Añade aquí las variables que necesites

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Dibujar un cuadrado de 10x10 celdas
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 200);
    run();
    return 0;
}
