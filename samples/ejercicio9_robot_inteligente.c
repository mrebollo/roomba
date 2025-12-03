/**
 * Ejercicio 9: Robot Inteligente
 * Nivel: Avanzado
 * 
 * Objetivo: Implementa una estrategia que adapte su comportamiento 
 * según el nivel de batería y suciedad detectada.
 * 
 * Pistas:
 * - Usa máquina de estados para diferentes modos
 * - Prioriza limpieza cuando batería > 50%
 * - Prioriza conservar batería cuando batería < 50%
 * - Regresa a base cuando batería < 20%
 * - Adapta la agresividad de exploración según batería
 */

#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

typedef enum {
    EXPLORACION_AGRESIVA,
    LIMPIEZA_PROFUNDA,
    CONSERVACION_ENERGIA,
    REGRESO_BASE,
    RECARGANDO
} ModoRobot;

ModoRobot modo_actual = EXPLORACION_AGRESIVA;
int base_x, base_y;
int celdas_limpiadas = 0;

void inicializar() {
    rmb_awake(&base_x, &base_y);
    srand(time(NULL));
    printf("Robot inteligente iniciado en (%d, %d)\n", base_x, base_y);
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Comportamiento adaptativo inteligente
    
    float bateria = rmb_battery();
    float porcentaje_bateria = (bateria / 1000.0) * 100.0;
    
    // Decisión de modo basada en batería
    // TODO: Implementa la lógica de cambio de modo
    
    // Sugerencias de comportamiento por modo:
    
    // EXPLORACION_AGRESIVA (batería > 50%):
    // - Explora rápidamente
    // - Limpia si encuentras suciedad
    // - Usa movimientos diagonales
    
    // LIMPIEZA_PROFUNDA (batería > 50% y suciedad detectada):
    // - Limpia completamente cada celda
    // - Explora áreas cercanas
    
    // CONSERVACION_ENERGIA (batería 20-50%):
    // - Movimientos eficientes (evita diagonales)
    // - Limpia solo suciedad alta (nivel 3+)
    // - Evita colisiones
    
    // REGRESO_BASE (batería < 20%):
    // - Navega directamente a base
    // - No limpies ni explores
    
    // RECARGANDO (en base):
    // - Usa rmb_load() hasta batería > 80%
    // - Vuelve a EXPLORACION_AGRESIVA
    
    switch(modo_actual) {
        case EXPLORACION_AGRESIVA:
            // TODO: Implementa exploración agresiva
            break;
            
        case LIMPIEZA_PROFUNDA:
            // TODO: Implementa limpieza profunda
            break;
            
        case CONSERVACION_ENERGIA:
            // TODO: Implementa conservación de energía
            break;
            
        case REGRESO_BASE:
            // TODO: Implementa regreso a base
            break;
            
        case RECARGANDO:
            // TODO: Implementa recarga
            break;
    }
}

void finalizar() {
    printf("\n=== Estadisticas del Robot Inteligente ===\n");
    printf("Celdas limpiadas: %d\n", celdas_limpiadas);
    printf("Bateria final: %.1f\n", rmb_battery());
    printf("Modo final: ");
    
    switch(modo_actual) {
        case EXPLORACION_AGRESIVA: printf("Exploracion Agresiva\n"); break;
        case LIMPIEZA_PROFUNDA: printf("Limpieza Profunda\n"); break;
        case CONSERVACION_ENERGIA: printf("Conservacion Energia\n"); break;
        case REGRESO_BASE: printf("Regreso Base\n"); break;
        case RECARGANDO: printf("Recargando\n"); break;
    }
    
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 5000);
    run();
    return 0;
}
