/**
 * Ejercicio 2: Limpiador Simple
 * Nivel: Básico
 * 
 * Objetivo: Limpia al menos el 50% de la suciedad del mapa.
 * 
 * Pistas:
 * - Usa rmb_ifr() para detectar suciedad
 * - Usa rmb_clean() para limpiar
 * - Combina exploración con limpieza
 * - Puedes usar el ejemplo 4 (rebote) como base
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
    // Objetivo: Limpiar al menos 50% de la suciedad
    
    // Sugerencia: Primero detecta y limpia, luego navega
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 2000);
    run();
    return 0;
}
