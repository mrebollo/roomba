/**
 * Ejemplo 7: Limpieza Completa de una Celda
 * 
 * Robot que limpia completamente cada celda sucia
 * (repitiendo rmb_clean() hasta que esté limpia).
 */

#include "simula.h"
#include <math.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    int suciedad = rmb_ifr();
    
    if (suciedad > 0) {
        // Limpia hasta que este completamente limpia
        while (rmb_ifr() > 0) {
            rmb_clean();
        }
        return;
    }
    
    // Navegacion con rebote aleatorio
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);
    } else {
        rmb_forward();
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
