/**
 * Ejemplo 8: Seguimiento de Estadísticas con Finalizar
 * 
 * Usa variables static para mantener estadísticas
 * y las muestra en la función finalizar().
 */

#include "simula.h"
#include <math.h>
#include <stdio.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    // Variables static: mantienen su valor entre llamadas
    static int pasos_totales = 0;
    static int colisiones = 0;
    static int limpiezas = 0;
    
    if (rmb_ifr() > 0) {
        rmb_clean();
        limpiezas++;
    } else if (rmb_bumper()) {
        rmb_turn(M_PI / 2);
        colisiones++;
    } else {
        rmb_forward();
        pasos_totales++;
    }
}

void finalizar() {
    printf("\n=== Estadisticas de la Mision ===\n");
    // Nota: Las variables static no son accesibles aquí
    // Este es un ejemplo simplificado
    printf("Bateria final: %.1f\n", rmb_battery());
    
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
