/**
 * Ejercicio 8: Eficiencia Energética
 * Nivel: Avanzado
 * 
 * Objetivo: Limpia 50+ unidades de suciedad con menos de 500 unidades de batería.
 * 
 * Pistas:
 * - Optimiza cada movimiento (evita colisiones = -0.5 batería)
 * - Usa estrategia de exploración eficiente
 * - Evita movimientos diagonales innecesarios (cuestan 1.4 vs 1.0)
 * - Limpia solo cuando encuentres suciedad
 * - Planifica rutas cortas
 */

#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int unidades_limpiadas = 0;
float bateria_inicial = 1000.0;

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    srand(time(NULL));
    bateria_inicial = rmb_battery();
}

void comportamiento() {
    // TODO: Implementa tu solución aquí
    // Objetivo: Limpiar 50+ unidades con < 500 batería consumida
    
    // Optimizaciones clave:
    // 1. Minimiza colisiones (cada colisión = -0.5 batería)
    // 2. Usa giros eficientes (cada giro = -0.1 batería)
    // 3. Prioriza movimientos rectos sobre diagonales
    // 4. Limpia eficientemente (cada clean = -0.5 batería)
    // 5. Para cuando hayas limpiado 50 unidades
    
    static int veces_limpiado = 0;
    
    // Si detectas suciedad, cuenta las veces que limpias
    if (rmb_ifr() > 0) {
        rmb_clean();
        veces_limpiado++;
        unidades_limpiadas = veces_limpiado;
    }
    
    // Detener si alcanzas el objetivo
    if (unidades_limpiadas >= 50) {
        // Objetivo cumplido
        return;
    }
}

void finalizar() {
    float bateria_final = rmb_battery();
    float bateria_consumida = bateria_inicial - bateria_final;
    
    printf("\n=== Resultados ===\n");
    printf("Unidades de suciedad limpiadas: %d\n", unidades_limpiadas);
    printf("Bateria consumida: %.1f / 500.0\n", bateria_consumida);
    
    if (unidades_limpiadas >= 50 && bateria_consumida < 500.0) {
        printf("¡EXITO! Objetivo cumplido.\n");
    } else if (unidades_limpiadas >= 50) {
        printf("Limpiaste suficiente pero gastaste demasiada bateria.\n");
    } else {
        printf("No limpiaste suficiente suciedad.\n");
    }
    
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 3000);
    run();
    return 0;
}
