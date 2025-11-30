/**
 * @file main_template.c
 * @brief Proyecto de robot aspirador Roomba - Plantilla para estudiantes
 * 
 * Este archivo contiene el código de control del robot aspirador.
 * Los estudiantes deben modificar las funciones on_start() y cyclic_behav()
 * para implementar su propio algoritmo de limpieza.
 * 
 * Estructura del programa:
 * - on_start(): Se ejecuta una vez al inicio (inicialización)
 * - cyclic_behav(): Se ejecuta repetidamente (comportamiento del robot)
 * - on_stop(): Se ejecuta una vez al final (visualización y limpieza)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "simula.h"

// ============================================================================
// CONSTANTES ÚTILES (puedes añadir más según necesites)
// ============================================================================
#define PI 3.14159265f
#define PI_2 (PI / 2.0f)        // 90 grados
#define PI_4 (PI / 4.0f)        // 45 grados
#define BATTERY_LOW 250.0f      // Umbral de batería baja

// ============================================================================
// FUNCIONES AUXILIARES (puedes crear las tuyas propias)
// ============================================================================

/**
 * @brief Gira el robot 90 grados a la izquierda
 */
void turn_left() {
    rmb_turn(PI_2);
}

/**
 * @brief Gira el robot 90 grados a la derecha  
 */
void turn_right() {
    rmb_turn(-PI_2);
}

/**
 * @brief Gira el robot 180 grados (media vuelta)
 */
void turn_back() {
    rmb_turn(PI);
}

/**
 * @brief Genera un ángulo aleatorio entre 0 y 2*PI
 * @return Ángulo aleatorio en radianes
 */
float random_angle() {
    return (rand() / (float)RAND_MAX) * 2 * PI;
}

/**
 * @brief Limpia toda la suciedad de la posición actual
 */
void clean_all() {
    while(rmb_ifr() > 0) {
        rmb_clean();
    }
}

/**
 * @brief Carga la batería completamente si está en la base
 * @return 1 si cargó, 0 si no está en la base
 */
int charge_if_needed() {
    if(rmb_at_base() && rmb_battery() < BATTERY_LOW) {
        while(rmb_battery() < 1000) {
            rmb_load();
        }
        return 1;
    }
    return 0;
}

// ============================================================================
// COMPORTAMIENTO DEL ROBOT (modifica estas funciones)
// ============================================================================

/**
 * @brief Función de inicialización - se ejecuta una vez al inicio
 * 
 * Aquí debes:
 * - Activar el robot con rmb_awake()
 * - Inicializar variables globales si las necesitas
 * - Configurar el estado inicial del robot
 */
void on_start() {
    int base_x, base_y;
    
    // Activar el robot y obtener posición de la base
    rmb_awake(&base_x, &base_y);
    
    printf("Robot activado en la base: (%d, %d)\n", base_x, base_y);
    printf("Iniciando comportamiento...\n\n");
}


/**
 * @brief Función de comportamiento cíclico - se ejecuta repetidamente
 * 
 * Esta función se ejecutará en cada ciclo del simulador.
 * Aquí debes implementar la lógica de tu robot:
 * - Movimiento
 * - Detección de obstáculos
 * - Limpieza de suciedad
 * - Gestión de batería
 * 
 * ALGORITMO ACTUAL: Movimiento aleatorio simple
 * TODO: Implementa tu propio algoritmo aquí
 */
void cyclic_behav() {
    // 1. Intentar avanzar en la dirección actual
    rmb_forward();
    
    // 2. Obtener estado de los sensores
    sensor_t estado = rmb_state();
    printf("Posición: (%d, %d) | Batería: %.1f | ", 
           estado.x, estado.y, estado.battery);
    
    // 3. Detección de obstáculos
    if(rmb_bumper()) {
        printf("¡Obstáculo! Girando... ");
        // Estrategia: giro aleatorio cuando choca
        float angle = random_angle();
        rmb_turn(angle);
    }
    
    // 4. Limpieza de suciedad
    if(rmb_ifr() > 0) {
        printf("Limpiando suciedad (nivel: %d)... ", rmb_ifr());
        clean_all();
        printf("¡Limpio! ");
    }
    
    // 5. Gestión de batería
    if(charge_if_needed()) {
        printf("Batería recargada. ");
    }
    
    printf("\n");
}


/**
 * @brief Función de finalización - se ejecuta una vez al final
 * 
 * Aquí puedes:
 * - Mostrar estadísticas
 * - Visualizar la simulación
 * - Guardar resultados
 */
void on_stop() {
    char respuesta[10];
    
    printf("\n========================================\n");
    printf("Simulación completada\n");
    printf("========================================\n");
    printf("\nResultados guardados en:\n");
    printf("  • log.csv   - Trayectoria del robot\n");
    printf("  • stats.csv - Estadísticas de ejecución\n");
    
    printf("\n¿Ver visualización? (s=sí, Enter=salir): ");
    fgets(respuesta, sizeof(respuesta), stdin);
    
    if(respuesta[0] == 's' || respuesta[0] == 'S') {
        visualize();
    }
}

// ============================================================================
// FUNCIÓN PRINCIPAL
// ============================================================================

int main(int argc, char *argv[]) {
    // Inicializar generador de números aleatorios
    srand(time(NULL));
    
    // Cargar mapa si se proporciona como argumento
    if(argc > 1) {
        printf("Cargando mapa: %s\n", argv[1]);
        if(load_map(argv[1]) != 0) {
            printf("Error al cargar el mapa\n");
            return 1;
        }
    } else {
        printf("Generando mapa aleatorio...\n");
    }
    
    // Configurar el simulador
    // Parámetros: (inicio, cíclico, fin, max_ciclos)
    configure(on_start, cyclic_behav, on_stop, 2500);
    
    // Ejecutar la simulación
    printf("\n¡Iniciando simulación!\n\n");
    run();
    
    return 0;
}
