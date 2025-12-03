/**
 * Ejercicio 6: Limpieza Profunda
 * Nivel: Intermedio
 * 
 * Objetivo: Limpia el 80% de la suciedad del mapa.
 * 
 * Pistas:
 * - Necesitas una buena estrategia de exploración
 * - Limpia COMPLETAMENTE cada celda sucia (while rmb_ifr() > 0)
 * - Considera gestión de batería para no quedarte sin energía
 * - Combina exploración sistemática con limpieza
 */

#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int celdas_limpiadas = 0;

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    srand(time(NULL));
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Limpiar al menos 80% de la suciedad
    
    // Estrategia sugerida:
    // 1. Si hay suciedad (rmb_ifr() > 0), limpia completamente
    // 2. Si no hay suciedad, explora sistemáticamente
    // 3. Considera volver a base si batería baja
    // 4. Lleva cuenta de celdas limpiadas
}

void finalizar() {
    printf("\n=== Estadisticas ===\n");
    printf("Celdas completamente limpiadas: %d\n", celdas_limpiadas);
    printf("Bateria restante: %.1f\n", rmb_battery());
    
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 5000);
    run();
    return 0;
}
