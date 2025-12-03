/**
 * Ejemplo 5: Robot que Sigue Paredes
 * 
 * Estrategia más inteligente usando máquina de estados.
 * El robot intenta seguir las paredes del entorno.
 */

#include "simula.h"
#include <math.h>

typedef enum { AVANZANDO, GIRANDO, AJUSTANDO } Estado;
Estado estado_actual = AVANZANDO;
int pasos_desde_choque = 0;

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    switch(estado_actual) {
        case AVANZANDO:
            if (rmb_bumper()) {
                rmb_turn(M_PI / 2);
                estado_actual = GIRANDO;
                pasos_desde_choque = 0;
            } else {
                rmb_forward();
                pasos_desde_choque++;
                
                if (pasos_desde_choque > 5) {
                    estado_actual = AJUSTANDO;
                }
            }
            break;
            
        case GIRANDO:
            rmb_forward();
            estado_actual = AVANZANDO;
            break;
            
        case AJUSTANDO:
            rmb_turn(-M_PI / 4);
            pasos_desde_choque = 0;
            estado_actual = AVANZANDO;
            break;
    }
}

void finalizar() {
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
