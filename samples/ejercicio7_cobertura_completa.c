/**
 * Ejercicio 7: Cobertura Completa
 * Nivel: Avanzado
 * 
 * Objetivo: Visita el 95% de las celdas accesibles del mapa.
 * 
 * Pistas:
 * - Necesitas una estrategia de exploración muy eficiente
 * - Considera algoritmos como seguir paredes sistemáticamente
 * - Evita quedarte atrapado en ciclos
 * - Implementa memoria de celdas visitadas si es necesario
 * - Gestión de batería es crucial para alcanzar 95%
 */

#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Estructura para rastrear celdas visitadas (opcional)
// int visitado[50][50] = {0};  // Mapa de celdas visitadas

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    srand(time(NULL));
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Visitar 95% de celdas accesibles
    
    // Estrategias avanzadas:
    // 1. Seguimiento sistemático de paredes (siempre girar en la misma dirección)
    // 2. Memoria de celdas visitadas (array 2D)
    // 3. Priorizar movimiento hacia zonas no visitadas
    // 4. Gestión inteligente de batería
    // 5. Detección de callejones sin salida
}

void finalizar() {
    sensor_t estado = rmb_state();
    printf("\n=== Estadisticas Finales ===\n");
    printf("Posicion final: (%d, %d)\n", estado.x, estado.y);
    printf("Bateria restante: %.1f\n", estado.battery);
    
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 8000);
    run();
    return 0;
}
