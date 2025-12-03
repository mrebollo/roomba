/**
 * Ejercicio 4: Gestor de Batería
 * Nivel: Intermedio
 * 
 * Objetivo: Implementa recarga automática cuando la batería 
 * caiga por debajo del 20% (200 unidades).
 * 
 * Pistas:
 * - Guarda la posición de la base en rmb_awake()
 * - Usa rmb_battery() para monitorear batería
 * - Implementa navegación dirigida hacia la base
 * - Usa rmb_load() para recargar en la base
 * - Usa atan2() para calcular dirección a la base
 */

#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int base_x, base_y;

void inicializar() {
    rmb_awake(&base_x, &base_y);
    srand(time(NULL));
    printf("Base en: (%d, %d)\n", base_x, base_y);
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Regresar a base cuando batería < 20%
    
    // Sugerencias:
    // 1. Verifica batería con rmb_battery()
    // 2. Si batería < 200, navega hacia (base_x, base_y)
    // 3. Al llegar a la base, usa rmb_load()
    // 4. Cuando batería > 900, vuelve a explorar/limpiar
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 3000);
    run();
    return 0;
}
