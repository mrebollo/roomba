

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "simula.h"

// Estado global para giro alterno
int giro_izq = 1;

// Busca suciedad en las 4 direcciones cardinales
int buscar_suciedad(sensor_t rob, float *ang_objetivo) {
    float dir[4] = {0, M_PI/2, M_PI, -M_PI/2}; // Este, Norte, Oeste, Sur
    for (int i = 0; i < 4; i++) {
        rmb_turn(dir[i] - rob.heading); // gira temporalmente
        rmb_forward();
        int suciedad = rmb_ifr();
        if (suciedad > 0) {
            *ang_objetivo = dir[i];
            // Vuelve atrás para no avanzar realmente
            rmb_turn(M_PI); rmb_forward(); rmb_turn(-M_PI);
            return 1;
        }
        // Vuelve atrás para no avanzar realmente
        rmb_turn(M_PI); rmb_forward(); rmb_turn(-M_PI);
    }
    return 0;
}

void on_start() {
    int basex, basey;
    rmb_awake(&basex, &basey);
}

void cyclic_behav() {
    sensor_t rob = rmb_state();
    // Si batería baja, buscar base
    if (rob.battery < 200) {
        if (!rmb_at_base()) {
            int basex, basey;
            rmb_base_pos(&basex, &basey);
            float dx = basex - rob.x;
            float dy = basey - rob.y;
            float ang = atan2(dy, dx);
            rmb_turn(ang - rob.heading);
        } else {
            while (rmb_battery() < 1000) rmb_load();
        }
    } else {
        // Optimización: buscar suciedad cercana
        float ang_objetivo = 0;
        if (buscar_suciedad(rob, &ang_objetivo)) {
            rmb_turn(ang_objetivo - rob.heading);
            rmb_forward();
        } else {
            rmb_forward();
        }
        // Si choca, alterna giro
        if (rmb_bumper()) {
            float ang = giro_izq ? M_PI/2 : -M_PI/2;
            rmb_turn(ang);
            giro_izq = !giro_izq;
        }
        // Limpia si hay suciedad
        while (rmb_ifr() > 0) rmb_clean();
    }
}

void on_stop() {
    visualize();
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc > 1) load_map(argv[1]);
    configure(on_start, cyclic_behav, on_stop, 100);
    run();
    return 0;
}
