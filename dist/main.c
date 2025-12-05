/**
 * Plantilla de Proyecto - Simulador Roomba
 * 
 * INSTRUCCIONES:
 * 1. Implementa tu logica en las funciones inicializar(), 
 *    comportamiento() y finalizar()
 * 2. Compila con: make
 * 3. Ejecuta con: ./roomba
 * 
 * FUNCIONES DISPONIBLES (consulta simula.h para mas detalles):
 * - Sensores: rmb_state(), rmb_battery(), rmb_bumper(), rmb_ifr(), rmb_awake()
 * - Actuadores: rmb_forward(), rmb_turn(), rmb_clean(), rmb_load()
 * - Utilidades: visualize(), save_stats()
 */

#include "simula.h"
#include <math.h>    // Para M_PI y funciones trigonometricas
#include <stdio.h>   // Para printf (opcional)
#include <stdlib.h>  // Para rand() (opcional)
#include <time.h>    // Para srand(time(NULL)) (opcional)

// ============================================================================
// VARIABLES GLOBALES (opcional)
// ============================================================================
// Puedes declarar aqui variables que necesites mantener entre llamadas

int base_x, base_y;  // Ejemplo: posicion de la base


// ============================================================================
// FUNCION DE INICIALIZACION
// ============================================================================
/**
 * Se ejecuta UNA VEZ al inicio de la simulacion.
 * Usala para:
 * - Obtener la posicion inicial (base)
 * - Inicializar variables globales
 * - Inicializar generador aleatorio (si lo usas)
 */
void inicializar() {
    // Obtener posicion de la base
    rmb_awake(&base_x, &base_y);
    
    // Inicializar generador de numeros aleatorios (opcional)
    // srand(time(NULL));
    
    // *** Tu codigo de inicializacion aqui ***
    
}


// ============================================================================
// FUNCION DE COMPORTAMIENTO
// ============================================================================
/**
 * Se ejecuta EN CADA PASO de la simulacion (bucle principal).
 * 
 * IMPORTANTE: Esta funcion debe ser RAPIDA y ejecutar solo UNA accion
 * por llamada (un forward, un turn, un clean, etc.)
 * 
 * Implementa aqui la logica de tu robot.
 */
void comportamiento() {
    // EJEMPLO BASICO: Rebote simple con limpieza
    
    // Si hay suciedad, limpiar
    if (rmb_ifr() > 0) {
        rmb_clean();
        return;
    }
    
    // Si choca con obstaculo, girar
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);  // Girar 90 grados
    } else {
        // Si no hay obstaculo, avanzar
        rmb_forward();
    }
    
    // *** REEMPLAZA EL CODIGO ANTERIOR CON TU PROPIA LOGICA ***
}


// ============================================================================
// FUNCION DE FINALIZACION
// ============================================================================
/**
 * Se ejecuta UNA VEZ al final de la simulacion.
 * Usala para:
 * - Mostrar estadisticas
 * - Visualizar el recorrido
 * - Guardar resultados
 */
void finalizar() {
    // Mostrar animacion del recorrido (recomendado)
    visualize();
    
    // Guardar estadisticas en stats.csv (opcional, se hace automaticamente)
    // save_stats();
    
    // Imprimir informacion adicional (opcional)
    // printf("Simulacion terminada\n");
    // printf("Base en: (%d, %d)\n", base_x, base_y);
    // printf("Bateria final: %.1f\n", rmb_battery());
}


// ============================================================================
// FUNCION MAIN
// ============================================================================
/**
 * NO MODIFICAR (excepto exec_time si quieres mas/menos pasos)
 */
int main() {
    // Configurar simulacion
    // Parametros: on_start, exec_beh, on_finish, exec_time (pasos maximos)
    configure(inicializar, comportamiento, finalizar, 1000);
    
    // Ejecutar simulacion
    run();
    
    return 0;
}
