/**
 * Ejercicio 5: Espiral Completa
 * Nivel: Intermedio
 * 
 * Objetivo: Cubre el área en espiral sin repetir celdas.
 * 
 * Pistas:
 * - Comienza en el centro o una esquina
 * - Aumenta gradualmente la longitud de cada lado
 * - Patrón: 1 paso, gira, 1 paso, gira, 2 pasos, gira, 2 pasos, gira, 3 pasos...
 * - Maneja colisiones cuando llegues a una pared
 */

#include "simula.h"
#include <math.h>
#include <stdio.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Movimiento en espiral
    
    // Algoritmo sugerido:
    // 1. Avanza N pasos en una dirección
    // 2. Gira 90 grados
    // 3. Avanza N pasos
    // 4. Gira 90 grados
    // 5. Incrementa N
    // 6. Repite
    
    // Variables estáticas útiles:
    // static int pasos_en_lado = 0;
    // static int pasos_objetivo = 1;
    // static int lados_completados = 0;
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 2000);
    run();
    return 0;
}
