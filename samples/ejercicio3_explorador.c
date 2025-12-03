/**
 * Ejercicio 3: Explorador
 * Nivel: Básico
 * 
 * Objetivo: Visita al menos el 30% de las celdas del mapa.
 * 
 * Pistas:
 * - Implementa una estrategia de exploración
 * - Rebote aleatorio es una buena base
 * - Sigue paredes para mejor cobertura
 * - No es necesario limpiar, solo explorar
 */

#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    srand(time(NULL));
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Visitar al menos 30% de las celdas
    
    // Sugerencia: Combina movimiento aleatorio con seguimiento de paredes
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 2000);
    run();
    return 0;
}
