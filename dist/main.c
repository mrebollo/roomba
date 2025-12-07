/**
 * @file main.c
 * @brief Plantilla de Proyecto - Simulador Roomba
 * @author Miguel Rebollo (@mrebollo)
 * @date 2025
 * 
 * INSTRUCCIONES:
 * 1. Implementa tu lógica en las funciones inicializar(), 
 *    comportamiento() y finalizar()
 * 2. Compila con: make
 * 3. Ejecuta con: ./roomba
 * 
 * FUNCIONES DISPONIBLES (consulta simula.h para detalles):
 * - Sensores: rmb_state(), rmb_battery(), rmb_bumper(), rmb_ifr()
 * - Actuadores: rmb_forward(), rmb_turn(), rmb_clean(), rmb_load()
 * - Utilidades: visualize()
 */

#include "simula.h"
#include <math.h>    // Para M_PI y funciones trigonométricas
#include <stdio.h>   // Para printf (opcional)
#include <stdlib.h>  // Para rand() (opcional)
#include <time.h>    // Para srand(time(NULL)) (opcional)

// ============================================================================
// VARIABLES GLOBALES (opcional)
// ============================================================================
/**
 * Puedes declarar aquí variables que necesites mantener entre llamadas
 * a la función comportamiento().
 * 
 * Ejemplo: posición de la base, contadores, estados, etc.
 */

int base_x, base_y;  // Ejemplo: posición de la base del robot


// ============================================================================
// FUNCIÓN DE INICIALIZACIÓN
// ============================================================================
/**
 * @brief Función de inicialización del robot
 * 
 * Se ejecuta UNA VEZ al inicio de la simulación.
 * 
 * Úsala para:
 * - Obtener la posición inicial de la base: rmb_awake(&x, &y)
 * - Inicializar variables globales
 * - Inicializar generador aleatorio: srand(time(NULL))
 * - Configurar estados iniciales
 */
void inicializar() {
    // Obtener posición de la base
    rmb_awake(&base_x, &base_y);
    
    // --- Tu código de inicialización aquí ---
    
}


// ============================================================================
// FUNCIÓN DE COMPORTAMIENTO
// ============================================================================
/**
 * @brief Función de comportamiento cíclico del robot
 * 
 * Se ejecuta EN CADA PASO de la simulación (bucle principal).
 * 
 * IMPORTANTE: 
 * - Esta función debe ser RÁPIDA
 * - Ejecuta solo UNA acción por llamada (un forward, un turn, un clean, etc.)
 * - Se llama repetidamente hasta alcanzar exec_time pasos
 * 
 * Implementa aquí la lógica de tu robot. Consulta simula.h para ver
 * todas las funciones disponibles.
 */
void comportamiento() {
    // ========================================================================
    // EJEMPLO BÁSICO: Rebote aleatorio con limpieza
    // ========================================================================
    // Este es solo un ejemplo simple. REEMPLÁZALO con tu propia lógica.
    
    // Prioridad 1: Si hay suciedad en la celda actual, limpiar
    if (rmb_ifr() > 0) {
        rmb_clean();
        return;  // Importante: solo una acción por ciclo
    }
    
    // Prioridad 2: Si choca con obstáculo, girar
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);  // Girar 90 grados a la izquierda
    } else {
        // Si no hay obstáculo, avanzar
        rmb_forward();
    }
    
    // ========================================================================
    // --- REEMPLAZA EL CÓDIGO ANTERIOR CON TU PROPIA LÓGICA ---
    // ========================================================================
    //
    // Ideas para implementar:
    // - Seguimiento de paredes (wall-following)
    // - Gestión de batería (regresar a base cuando < 200)
    // - Máquina de estados (explorar, limpiar, recargar)
    // - Navegación dirigida usando atan2()
    // - Memoria de celdas visitadas
    //
}


// ============================================================================
// FUNCIÓN DE FINALIZACIÓN
// ============================================================================
/**
 * @brief Función de finalización del robot
 * 
 * Se ejecuta UNA VEZ al final de la simulación.
 * 
 * Úsala para:
 * - Mostrar estadísticas personalizadas
 * - Visualizar el recorrido: visualize()
 * - Guardar resultados adicionales
 */
void finalizar() {
    // Mostrar animación del recorrido (recomendado)
    visualize();
    
    // Otras tareas
    // - Guardar estadísticas
    // - Guardar el recorrido (si se ha creado un mapa propio)
    // - Imprimir información adicional (opcional)
    // printf("Simulacion terminada\n");
    // printf("Bateria final: %.1f\n", rmb_battery());
}


// ============================================================================
// FUNCIÓN MAIN
// ============================================================================
/**
 * @brief Función principal del programa
 * 
 * NO MODIFICAR (excepto exec_time si quieres cambiar la duración)
 * 
 * @return 0 si la ejecución fue exitosa
 */
int main(int argc, char *argv[]) {
    // Cargar mapa si se proporciona como argumento
    if (argc > 1) {
        load_map(argv[1]);
    }

    // Configurar simulación
    // Parámetros:
    //   - on_start: función de inicialización
    //   - exec_beh: función de comportamiento (se llama en cada paso)
    //   - on_finish: función de finalización
    //   - exec_time: número máximo de pasos a simular (ajustable)
    configure(inicializar, comportamiento, finalizar, 100);
    
    // Ejecutar simulación
    run();
    
    return 0;
}
