/**
 * Ejemplo 9: Robot Autónomo Completo
 * 
 * Implementa un robot completo con:
 * - Exploración inteligente
 * - Limpieza automática
 * - Regreso a base cuando batería baja
 * - Recarga automática
 */

#include "simula.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    EXPLORANDO,
    LIMPIANDO,
    REGRESANDO,
    RECARGANDO
} EstadoRobot;

EstadoRobot estado = EXPLORANDO;
int base_x, base_y;
int celdas_limpiadas = 0;

void inicializar() {
    rmb_awake(&base_x, &base_y);
    printf("Robot iniciado en base (%d, %d)\n", base_x, base_y);
}

void explorar() {
    if (rmb_ifr() > 0) {
        estado = LIMPIANDO;
        return;
    }
    
    if (rmb_bumper()) {
        float angulo = M_PI / 2 + (rand() % 90) * M_PI / 180;
        rmb_turn(angulo);
    } else {
        rmb_forward();
    }
}

void limpiar() {
    if (rmb_ifr() > 0) {
        rmb_clean();
    } else {
        celdas_limpiadas++;
        estado = EXPLORANDO;
    }
}

void regresar_base() {
    sensor_t s = rmb_state();
    
    if (s.x == base_x && s.y == base_y) {
        estado = RECARGANDO;
        return;
    }
    
    int dx = base_x - s.x;
    int dy = base_y - s.y;
    float angulo_objetivo = atan2(dy, dx);
    float dif = angulo_objetivo - s.heading;
    
    while (dif > M_PI) dif -= 2 * M_PI;
    while (dif < -M_PI) dif += 2 * M_PI;
    
    if (fabs(dif) > 0.2) {
        rmb_turn(dif * 0.3);
    } else {
        if (!rmb_bumper()) {
            rmb_forward();
        } else {
            rmb_turn(M_PI / 2);
        }
    }
}

void recargar() {
    if (rmb_battery() < 900) {
        rmb_load();
    } else {
        estado = EXPLORANDO;
    }
}

void comportamiento() {
    if (rmb_battery() < 150 && estado != REGRESANDO 
        && estado != RECARGANDO) {
        estado = REGRESANDO;
    }
    
    switch(estado) {
        case EXPLORANDO:
            explorar();
            break;
        case LIMPIANDO:
            limpiar();
            break;
        case REGRESANDO:
            regresar_base();
            break;
        case RECARGANDO:
            recargar();
            break;
    }
}

void finalizar() {
    // Mostrar resumen final
    printf("\n=== Resumen Final ===\n");
    printf("Celdas limpiadas: %d\n", celdas_limpiadas);
    printf("Bateria restante: %.1f\n", rmb_battery());
    
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 5000);
    run();
    return 0;
}
