/**
 * Ejemplo 6: Robot que Limpia al Detectar Suciedad
 * 
 * Robot que detecta suciedad con el sensor infrarrojo
 * y limpia cuando la encuentra.
 */

#include "simula.h"
#include <math.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    // Primero verifica si hay suciedad
    if (rmb_ifr() > 0) {
        rmb_clean();  // Limpia la celda actual
        return;       // Dedica este ciclo solo a limpiar
    }
    
    // Si no hay suciedad, navega
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
