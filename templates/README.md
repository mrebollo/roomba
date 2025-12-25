# Simulador Roomba - Entorno de Programación

Bienvenido al simulador Roomba. Este directorio contiene todo lo necesario para programar tu robot limpiador autónomo.

## Estructura de archivos

### Archivos del proyecto
- **`main.c`** - **EDITA ESTE ARCHIVO** - Programa aquí el comportamiento de tu robot
- **`Makefile`** - Sistema de compilación (no modificar)

### Biblioteca del simulador
- **`simula.o`** - Biblioteca precompilada del simulador (no modificar)
- **`simula.h`** - Documentación de funciones disponibles (consultar)

### Mapas de Prueba
- **`maps/`** - Diferentes escenarios para probar tu robot
  - `noobs.pgm` - Mapa simple sin obstáculos (ideal para empezar)
  - `random1.pgm`, `random3.pgm`, `random5.pgm` - Obstáculos aleatorios dispersos
  - `walls1.pgm`, `walls2.pgm`, `walls3.pgm`, `walls4.pgm` - Muros verticales/horizontales
- En cualquier momento puedes generar mapas adicionales las herramientas proporcionadas el `tools`. 


## Inicio Rápido

### 1. Compilar tu código
```bash
make
```

### 2. Ejecutar el simulador
```bash
./roomba                   # Genera un mapa aleatorio
./roomba maps/noobs.pgm    # Usa un mapa específico de la carpeta `maps`
```

### 3. Ver resultados
Después de la ejecución se generan:
- **Visualización animada** en la terminal (si está habilitada)
- **`log.csv`** - Trayectoria completa del robot (posición, orientación, batería en cada paso)
- **`stats.csv`** - Estadísticas finales (celdas visitadas, suciedad limpiada, consumo de batería)
- **`map.pgm`** - Imagen del mundo final (abrir con GIMP/Photoshop/visualizador PGM)

### 4. Limpiar archivos generados
```bash
make clean
```

## API del Simulador

Consulta `simula.h` para documentación completa. Aquí un resumen:

### Configuración Básica
```c
void configure(void (*on_start)(), void (*exec_beh)(), 
               void (*on_finish)(), int exec_time);
void run();
```

### Funciones de Sensores
```c
sensor_t rmb_state();           // Estado completo: x, y, heading, battery, bumper, ifr
float rmb_battery();            // Batería restante (0-1000)
int rmb_bumper();               // 1 si detecta colisión, 0 si libre
int rmb_ifr();                  // Nivel de suciedad en celda actual (0-5)
void rmb_awake(int *x, int *y); // Obtener coordenadas de la base
```

### Funciones de Actuadores
```c
void rmb_forward();             // Avanzar 1 celda en dirección actual
void rmb_turn(float rad);       // Girar (en radianes, positivo=izquierda)
void rmb_clean();               // Limpiar 1 unidad de suciedad
void rmb_load();                // Recargar 1 unidad de batería (solo en la base)
```

### Funciones de Utilidad
```c
void visualize();               // Mostrar animación ASCII del recorrido
```

## Plantilla Básica

```c
#include "simula.h"
#include <math.h>

// Variables globales (opcional)
int base_x, base_y;

// Se ejecuta UNA VEZ al inicio
void inicializar() {
    rmb_awake(&base_x, &base_y);  // Obtener posición de la base
    // Tu código de inicialización aquí
}

// Se ejecuta EN CADA PASO de la simulación
void comportamiento() {
    // Prioridad 1: Si hay suciedad, limpiar
    if (rmb_ifr() > 0) {
        rmb_clean();
        return;
    }
    
    // Prioridad 2: Si choca, girar
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);  // Girar 90 grados
    } else {
        rmb_forward();       // Avanzar
    }
}

// Se ejecuta UNA VEZ al final
void finalizar() {
    visualize();  // Mostrar animación del recorrido
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

## Consejos para Programar

### Gestión de Batería
```c
// Verificar batería antes de acciones costosas
if (rmb_battery() < 200) {
    // Regresar a la base para recargar
}
```

### Limpieza Completa
```c
// Limpiar hasta vaciar completamente la celda
while (rmb_ifr() > 0) {
    rmb_clean();
}
```

### Navegación Dirigida
```c
// Calcular ángulo hacia un objetivo (bx, by)
sensor_t s = rmb_state();
int dx = bx - s.x;
int dy = by - s.y;
float angulo = atan2(dy, dx);

// Normalizar diferencia de ángulos
float dif = angulo - s.heading;
while (dif > M_PI) dif -= 2 * M_PI;
while (dif < -M_PI) dif += 2 * M_PI;

// Girar hacia el objetivo
if (fabs(dif) > 0.2) {
    rmb_turn(dif * 0.5);
}
```

### Estrategias Comunes

**Rebote Aleatorio:**
- Simple pero ineficiente
- Bueno para empezar

**Seguimiento de Paredes:**
- Mayor cobertura
- Algoritmo: wall-following (pared derecha o izquierda)

**Comportamiento Adaptativo:**
- Máquina de estados
- Diferentes modos según batería/suciedad

## Pruebas y Depuración

### 1. Compilar frecuentemente
Detecta errores sintácticos pronto.

### 2. Usar visualize()
Ver el comportamiento del robot es fundamental.

### 3. Analizar stats.csv
Métricas objetivas de rendimiento:
```
cell_total,cell_visited,dirt_total,dirt_cleaned,bat_total,bat_mean,...
```

### 4. Probar en múltiples mapas
Tu código debe funcionar en diferentes escenarios:
```bash
./roomba maps/noobs.pgm
./roomba maps/random3.pgm
./roomba maps/walls2.pgm
```

### 5. Iterar y mejorar
1. Implementar versión básica
2. Medir resultados (stats.csv)
3. Identificar problemas
4. Mejorar algoritmo
5. Repetir

## Errores Comunes

### Error: "undefined reference to..."
- Falta compilar con `-lm` (biblioteca matemática)
- Solución: Usa `make` (ya incluye los flags necesarios)

### Robot se queda sin batería
- Implementa recarga automática
- Umbral recomendado: batería < 200 (20%)

### Robot gira infinitamente
- Posible bucle en la lógica de giros
- Revisa condiciones de los `if`/`while`

### Robot no limpia suficiente
- Prioriza limpieza sobre exploración
- Limpia completamente cada celda antes de moverse

## Métricas de Éxito

Dependiendo del ejercicio, evalúa:
- **Cobertura**: `cell_visited / cell_total` (%)
- **Limpieza**: `dirt_cleaned / dirt_total` (%)
- **Eficiencia**: Batería consumida para lograr objetivo
- **Tiempo**: Número de pasos hasta completar tarea

## Recursos Adicionales

- **Guía Standalone** (`STANDALONE_GUIDE.md`) - Guía de inicio y evaluación
- **Manual de Usuario** (`MANUAL_USUARIO.pdf`) - Referencia completa
- **Carpeta `samples/`** (si disponible) - Ejemplos funcionando
- **Archivo `simula.h`** - Documentación completa de API con comentarios

## Soporte

Si encuentras problemas:
1. Revisa el **Manual de Usuario**
2. Consulta los comentarios en **`simula.h`**
3. Analiza los **ejemplos** (si disponibles)
4. Verifica mensajes de error del compilador
5. Usa **`visualize()`** para depurar visualmente

---

**¡Buena suerte programando tu robot Roomba!**
