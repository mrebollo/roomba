# Manual del Usuario - Simulador Roomba

## Guía Paso a Paso para Programar tu Robot

Este manual te enseñará a programar comportamientos para un robot de limpieza virtual. No necesitas experiencia previa en robótica, solo conocimientos básicos de programación en C.

---

## Tabla de Contenidos

1. [Introducción](#1-introducción)
2. [Tu Primer Programa](#2-tu-primer-programa)
3. [Entendiendo los Sensores](#3-entendiendo-los-sensores)
4. [Movimiento Básico](#4-movimiento-básico)
5. [Detección de Obstáculos](#5-detección-de-obstáculos)
6. [Limpieza Inteligente](#6-limpieza-inteligente)
7. [Herramientas de Evaluación](#7-herramientas-de-evaluación)
8. [Gestión de Batería](#8-gestión-de-batería)
9. [Estrategias Completas](#9-estrategias-completas)
10. [Compilación del Proyecto](#10-compilación-del-proyecto)
11. [Generación y Visualización de Mapas](#11-generación-y-visualización-de-mapas)
12. [Depuración y Visualización](#12-depuración-y-visualización)

---

## 1. Introducción

### ¿Qué es el Simulador?

El simulador te permite programar un robot de limpieza virtual que se mueve en un mundo 2D. El robot puede:
- Moverse y girar
- Detectar obstáculos con su bumper
- Detectar suciedad con sensor infrarrojo
- Limpiar celdas sucias
- Recargar su batería en la base

### Estructura de un Programa

Todo programa sigue este patrón:

```c
#include "simula.h"

void inicializar() {
    // Código que se ejecuta UNA vez al inicio
}

void comportamiento() {
    // Código que se ejecuta REPETIDAMENTE (ciclo principal)
}

void finalizar() {
    visualize();  // Muestra la animación al terminar
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

**Importante:** La función `finalizar()` es **OBLIGATORIA** y debe incluir la llamada a `visualize()`. Se ejecuta automáticamente cuando termina `run()` (mediante `atexit()`).

---

## 2. Tu Primer Programa

### Ejemplo 1: Robot Inmóvil

Empecemos con lo más simple - un robot que solo despierta y se queda quieto:

```c
#include "simula.h"
#include <stdio.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    printf("Robot activado en posición (%d, %d)\n", x, y);
}

void comportamiento() {
    // No hace nada - el robot está quieto
}

int main() {
    configure(inicializar, comportamiento, NULL, 100);
    run();
    visualize();
    return 0;
}
```

**¿Qué hace este código?**
- `rmb_awake(&x, &y)` - Activa el robot y obtiene su posición inicial
- `configure(...)` - Configura el simulador con 100 ciclos máximo
- `run()` - Ejecuta la simulación
- `visualize()` - Muestra el resultado animado

**Ejercicio:** Compila y ejecuta este programa. Observa dónde aparece el robot. Ver sección de **Compilación del Proyecto** más adelante para las opciones de compilación.

---

### Ejemplo 2: Robot que Avanza

Ahora hagamos que el robot se mueva:

```c
#include "simula.h"

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    rmb_forward();  // Avanza un paso en cada ciclo
}

int main() {
    configure(inicializar, comportamiento, NULL, 50);
    run();
    visualize();
    return 0;
}
```

**¿Qué observas?**
- El robot avanza en línea recta
- Cuando choca con una pared, el bumper se activa
- El robot intenta seguir avanzando contra la pared

**Problema:** El robot no sabe evitar obstáculos todavía.

---

## 3. Entendiendo los Sensores

El robot tiene varios sensores que puedes consultar:

### Sensor Bumper

Detecta si el robot chocó con algo:

```c
if (rmb_bumper()) {
    printf("¡Obstáculo detectado!\n");
}
```

- Devuelve `1` si hay colisión
- Devuelve `0` si está libre

### Sensor Infrarrojo

Detecta suciedad en la posición actual:

```c
int nivel_suciedad = rmb_ifr();
if (nivel_suciedad > 0) {
    printf("Hay suciedad de nivel %d\n", nivel_suciedad);
}
```

- Devuelve `0` si está limpio
- Devuelve `1-5` según el nivel de suciedad

### Sensor de Batería

Consulta la batería restante:

```c
float bateria = rmb_battery();
printf("Batería: %.1f\n", bateria);
```

- Máximo: 1000
- Mínimo: 0 (robot se detiene)

### Posición y Orientación

Lee todos los sensores a la vez:

```c
sensor_t estado = rmb_state();
printf("Posición: (%d, %d)\n", estado.x, estado.y);
printf("Orientación: %.2f radianes\n", estado.heading);
printf("Batería: %.1f\n", estado.battery);
```

---

## 4. Movimiento Básico

### Girar

El robot puede girar especificando un ángulo en radianes:

```c
#include <math.h>

rmb_turn(M_PI / 2);     // Gira 90° a la izquierda
rmb_turn(-M_PI / 2);    // Gira 90° a la derecha
rmb_turn(M_PI);         // Gira 180° (media vuelta)
rmb_turn(M_PI / 4);     // Gira 45° a la izquierda
```

Recuerda: 
- Ángulos positivos = giro a la izquierda (antihorario)
- Ángulos negativos = giro a la derecha (horario)
- 2π radianes = 360° (vuelta completa)

### Ejemplo 3: Robot que Explora en Cuadrado

```c
#include "simula.h"
#include <math.h>

int pasos = 0;
int lado_actual = 0;

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    if (pasos < 10) {
        rmb_forward();
        pasos++;
    } else {
        rmb_turn(M_PI / 2);  // Gira 90° al completar un lado
        pasos = 0;
        lado_actual++;
        
        if (lado_actual >= 4) {
            lado_actual = 0;  // Comienza nuevo cuadrado
        }
    }
}

int main() {
    configure(inicializar, comportamiento, NULL, 200);
    run();
    visualize();
    return 0;
}
```

**Ejercicio:** Modifica el programa para que haga un triángulo en lugar de un cuadrado.

---

## 5. Detección de Obstáculos

### Ejemplo 4: Robot que Rebota

Cuando detecta un obstáculo, gira y continúa:

```c
#include "simula.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    srand(time(NULL));  // Inicializa números aleatorios
}

void comportamiento() {
    if (rmb_bumper()) {
        // Chocó - gira un ángulo aleatorio entre 45° y 135°
        float angulo = (M_PI / 4) + (rand() / (float)RAND_MAX) * (M_PI / 2);
        rmb_turn(angulo);
    } else {
        // Libre - avanza
        rmb_forward();
    }
}

int main() {
    configure(inicializar, comportamiento, NULL, 1000);
    run();
    visualize();
    return 0;
}
```

Estrategia: Este es el comportamiento básico de un Roomba real - rebota aleatoriamente.

---

### Ejemplo 5: Robot que Sigue Paredes

Una estrategia más inteligente:

```c
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
                rmb_turn(M_PI / 2);  // Gira 90° al chocar
                estado_actual = GIRANDO;
                pasos_desde_choque = 0;
            } else {
                rmb_forward();
                pasos_desde_choque++;
                
                // Cada cierto tiempo, intenta volver a la pared
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
            rmb_turn(-M_PI / 4);  // Gira ligeramente hacia la pared
            pasos_desde_choque = 0;
            estado_actual = AVANZANDO;
            break;
    }
}

int main() {
    configure(inicializar, comportamiento, NULL, 1000);
    run();
    visualize();
    return 0;
}
```

---

## 6. Limpieza Inteligente

### Ejemplo 6: Robot que Limpia al Detectar Suciedad

```c
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

int main() {
    configure(inicializar, comportamiento, NULL, 1000);
    run();
    visualize();
    return 0;
}
```

Importante: `rmb_clean()` reduce el nivel de suciedad en 1. Si una celda tiene nivel 3, necesitarás limpiar 3 veces.

---

### Ejemplo 7: Limpieza Completa de una Celda

```c
#include "simula.h"
#include <math.h>

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento() {
    int suciedad = rmb_ifr();
    
    if (suciedad > 0) {
        // Limpia hasta que esté completamente limpia
        while (rmb_ifr() > 0) {
            rmb_clean();
        }
        return;
    }
    
    // Navegación con rebote aleatorio
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);
    } else {
        rmb_forward();
    }
}

int main() {
    configure(inicializar, comportamiento, NULL, 2000);
    run();
    visualize();
    return 0;
}
```

Cuidado: Cada `rmb_clean()` consume batería (0.5 unidades). Asegúrate de tener suficiente.

---

## 7. Herramientas de Evaluación

### 7.1 Autoevaluación con myscore

La herramienta `myscore` te permite evaluar el rendimiento de tu robot de forma local, usando los mismos criterios que se usarán en la competición oficial.

**¿Qué hace myscore?**
- Analiza el archivo `stats.csv` generado por tu robot
- Calcula una puntuación basada en 4 métricas principales
- Te muestra qué aspectos necesitas mejorar
- Usa exactamente el mismo algoritmo que el sistema oficial de evaluación

**Compilar myscore:**

```bash
cd competition
make myscore
```

**Ejecutar tu robot y autoevaluarte:**

```bash
# 1. Ejecuta tu robot (genera stats.csv)
./roomba map.pgm

# 2. Evalúa el resultado
./competition/myscore stats.csv
```

**Salida de myscore:**

```
=== AUTOEVALUACION ROOMBA ===
Archivo: stats.csv

Metricas Individuales:
- Cobertura (30%): 45.23% -> 13.57 puntos
- Eficiencia Limpieza (35%): 2.15% -> 12.25 puntos
- Conservacion Bateria (20%): 38.50% -> 7.70 puntos
- Calidad Movimiento (15%): 89.45% -> 13.42 puntos

Bonificaciones:
+ Completitud: +5.00 (mapa completamente limpio)
+ Pocos choques: +3.00 (5 o menos colisiones)

Penalizaciones:
- Crashes: -10.00 (1 crash detectado)

Puntuacion Final: 44.94 / 108

=== CONSEJOS DE MEJORA ===
- Cobertura: Explora más áreas del mapa
- Eficiencia: Reduce movimientos innecesarios
- Batería: Optimiza uso de energía
```

**Interpretación de resultados:**

1. **Métricas principales (0-100 puntos base):**
   - **Cobertura (30%)**: Porcentaje de celdas visitadas
   - **Eficiencia de limpieza (35%)**: Suciedad limpiada vs movimientos
   - **Conservación de batería (20%)**: Batería restante al final
   - **Calidad de movimiento (15%)**: Minimizar colisiones

2. **Bonificaciones (+8 máximo):**
   - **+5 puntos**: Si limpiaste toda la suciedad del mapa
   - **+3 puntos**: Si tuviste 5 o menos colisiones

3. **Penalizaciones (-10 por crash):**
   - **-10 puntos**: Por cada crash (batería agotada o error fatal)

**Proceso de mejora iterativa:**

```bash
# Ciclo de desarrollo:
# 1. Modifica tu código
vim main.c

# 2. Compila
make

# 3. Ejecuta
./roomba map.pgm

# 4. Evalúa
./competition/myscore stats.csv

# 5. Analiza resultados y vuelve al paso 1
```

**Estrategia de optimización:**

1. **Primera iteración**: Enfócate en cobertura (explorar el mapa)
2. **Segunda iteración**: Mejora limpieza (detectar y limpiar suciedad)
3. **Tercera iteración**: Optimiza batería (gestión eficiente de energía)
4. **Cuarta iteración**: Reduce colisiones (navegación inteligente)

### 7.2 Generación de Estadísticas

Cuando tu robot termina de ejecutarse, el simulador genera automáticamente el archivo `stats.csv` con estadísticas detalladas de la ejecución.

**Estructura de stats.csv:**

```csv
map_id,rep_id,cell_total,cell_visited,dirt_total,dirt_cleaned,bat_init,bat_final,forward,turn,bumps,clean,load
```

**Significado de cada columna:**

| Columna | Descripción | Ejemplo |
|---------|-------------|--------|
| `map_id` | Identificador del mapa | 0, 1, 2, 3 |
| `rep_id` | Número de repetición | 0-4 |
| `cell_total` | Total de celdas navegables | 2304 |
| `cell_visited` | Celdas visitadas por el robot | 1245 |
| `dirt_total` | Unidades de suciedad iniciales | 310 |
| `dirt_cleaned` | Unidades de suciedad limpiadas | 285 |
| `bat_init` | Batería inicial | 1000.0 |
| `bat_final` | Batería final | 234.5 |
| `forward` | Número de movimientos forward | 450 |
| `turn` | Número de giros | 123 |
| `bumps` | Número de colisiones | 45 |
| `clean` | Número de acciones de limpieza | 285 |
| `load` | Número de recargas | 2 |

**Ejemplo de stats.csv:**

```csv
0,0,2304,1245,310,285,1000.0,234.5,450,123,45,285,2
```

**Interpretación del ejemplo:**
- Mapa 0, repetición 0
- De 2304 celdas totales, visitó 1245 (54% de cobertura)
- De 310 unidades de suciedad, limpió 285 (92% de eficiencia)
- Batería: empezó con 1000, terminó con 234.5 (23.5% conservado)
- Acciones: 450 avances, 123 giros, 45 colisiones, 285 limpiezas, 2 recargas

**Uso en competición:**

Este archivo `stats.csv` es el que se usará para calcular tu puntuación oficial en la competición. El sistema ejecutará tu robot múltiples veces (4 mapas × 5 repeticiones = 20 ejecuciones) y agregará los resultados para obtener tu puntuación final.

**Verificar stats.csv manualmente:**

```bash
# Ver contenido
cat stats.csv

# Verificar que se generó correctamente
ls -lh stats.csv

# Si no existe, revisa que tu robot:
# 1. Se ejecutó completamente
# 2. Llamó a rmb_awake() para iniciar
# 3. No crasheó antes de terminar
```

### 7.3 Visualización de Resultados

La función `visualize()` muestra una animación visual de lo que hizo tu robot durante la ejecución.

**Uso obligatorio:**

```c
void finalizar() {
    visualize();  // OBLIGATORIO - muestra animación
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

**¿Qué muestra la visualización?**

1. **Trayectoria del robot**: Línea mostrando el camino recorrido
2. **Celdas visitadas**: Áreas exploradas destacadas
3. **Suciedad limpiada**: Animación de limpieza en tiempo real
4. **Colisiones**: Indicadores visuales de choques con obstáculos
5. **Nivel de batería**: Barra de estado de energía
6. **Estadísticas finales**: Resumen al terminar la animación

**Interpretación visual:**

- **Líneas verdes**: Trayectoria exitosa
- **Puntos rojos**: Colisiones detectadas
- **Áreas amarillas**: Celdas con suciedad detectada
- **Áreas azules**: Celdas limpias después de pasar el robot
- **Cuadrado negro**: Posición de la base de carga

**Desactivar visualización temporalmente:**

Si necesitas ejecutar muchas pruebas rápidamente sin esperar la animación:

```c
void finalizar() {
    // Comentar temporalmente para pruebas rápidas
    // visualize();
}
```

**Importante:** Recuerda volver a activar `visualize()` antes de la entrega final.

**Análisis visual para optimización:**

1. **Cobertura baja**: Si ves muchas áreas sin explorar (blancas), mejora tu estrategia de navegación
2. **Muchas colisiones**: Si hay muchos puntos rojos, implementa mejor detección de obstáculos
3. **Suciedad sin limpiar**: Áreas amarillas indican que detectaste suciedad pero no la limpiaste
4. **Trayectoria errática**: Si la línea da muchas vueltas sin sentido, optimiza la lógica de movimiento
5. **Batería crítica**: Si la barra llega a rojo, implementa gestión de batería con recargas

**Comparar ejecuciones:**

Para comparar dos versiones de tu código:

```bash
# Versión 1
./roomba map.pgm
mv visualization.gif v1.gif

# Versión 2 (después de modificar código)
make
./roomba map.pgm  
mv visualization.gif v2.gif

# Comparar visualmente
open v1.gif v2.gif
```

---

## 8. Gestión de Batería

### 8.1 Costes de Batería

Cada acción consume batería:

| Acción | Coste |
|--------|-------|
| `rmb_turn()` | 0.1 |
| `rmb_forward()` (recto) | 1.0 |
| `rmb_forward()` (diagonal) | 1.4 |
| `rmb_clean()` | 0.5 |
| Colisión (bumper activo) | 0.5 |
| `rmb_load()` | +10 (recarga) |

### Ejemplo 8: Seguimiento de Estadísticas con Finalizar

La función `finalizar()` es opcional pero útil para mostrar estadísticas personalizadas. En este ejemplo usamos **variables static** dentro de `comportamiento()` en lugar de variables globales:

```c
#include "simula.h"
#include <math.h>

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
    printf("Pasos realizados: %d\n", pasos_totales);
    printf("Colisiones: %d\n", colisiones);
    printf("Limpiezas: %d\n", limpiezas);
    printf("Bateria final: %.1f\n", rmb_battery());
    printf("Eficiencia: %.2f limpiezas/paso\n", 
           (float)limpiezas / pasos_totales);
    
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

Importante: La función `finalizar()` es **OBLIGATORIA** y debe incluir `visualize()`. Se ejecuta automáticamente cuando termina `run()` mediante `atexit()`. También puedes usarla para mostrar estadísticas personalizadas.

### 8.2 Ejemplo de Gestión de Batería

Ver sección 9 para ejemplos completos que incluyen gestión de batería con regreso a base y recarga automática.

---

## 9. Estrategias Completas

### 9.1 Ejemplo: Robot Autónomo Completo

```c
#include "simula.h"
#include <math.h>
#include <stdio.h>

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
    
    // Navegación con rebote aleatorio mejorado
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
    
    // Navegación simple hacia la base
    int dx = base_x - s.x;
    int dy = base_y - s.y;
    float angulo_objetivo = atan2(dy, dx);
    float dif = angulo_objetivo - s.heading;
    
    // Normaliza el ángulo
    while (dif > M_PI) dif -= 2 * M_PI;
    while (dif < -M_PI) dif += 2 * M_PI;
    
    if (fabs(dif) > 0.2) {
        rmb_turn(dif * 0.3);
    } else {
        if (!rmb_bumper()) {
            rmb_forward();
        } else {
            // Obstáculo en el camino - esquiva
            rmb_turn(M_PI / 2);
        }
    }
}

void recargar() {
    if (rmb_battery() < 900) {
        rmb_load();
    } else {
        printf("Recarga completa. Celdas limpiadas: %d\n", celdas_limpiadas);
        estado = EXPLORANDO;
    }
}

void comportamiento() {
    // Verifica batería crítica
    if (rmb_battery() < 150 && estado != REGRESANDO && estado != RECARGANDO) {
        printf("¡Batería baja! Regresando a base...\n");
        estado = REGRESANDO;
    }
    
    // Máquina de estados
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
    printf("\n=== Resumen Final ===\n");
    printf("Celdas limpiadas: %d\n", celdas_limpiadas);
    printf("Batería restante: %.1f\n", rmb_battery());
}

int main() {
    configure(inicializar, comportamiento, finalizar, 5000);
    run();
    visualize();
    return 0;
}
```

---

## 10. Compilación del Proyecto

Existen tres formas de compilar tu programa con el simulador:

### 10.1 Opción 1: Compilación Directa desde Fuentes

Compila todos los archivos fuente directamente con `gcc`:

```bash
gcc main.c simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c -lm -o roomba
./roomba
```

Ventajas:
- Control total sobre el proceso de compilación
- No requiere archivos adicionales
- Útil para entender cómo funciona la compilación

**Desventajas:**
- Comando largo y propenso a errores
- Recompila todo cada vez (lento)

---

### 10.2 Opción 2: Usar Makefile (Recomendado)

La forma más simple y profesional es usar `make`:

```bash
make
./roomba
```

O directamente:

```bash
make run
```

El `Makefile` incluye varios objetivos útiles:

```bash
make          # Compila el proyecto
make run      # Compila y ejecuta
make debug    # Compila con símbolos de depuración
make clean    # Limpia archivos generados
```

Ventajas:
- Comando corto y fácil de recordar
- Recompila solo lo necesario (rápido)
- Estándar en proyectos C/C++

---

### 10.3 Opción 3: Compilación con Objeto Precompilado

Si el profesor proporciona un archivo `simula.o` precompilado, puedes compilar solo tu `main.c`:

```bash
gcc main.c simula.o -lm -o roomba
./roomba
```

Ventajas:
- Compilación muy rápida (solo tu código)
- No necesitas los fuentes del simulador
- Útil para competiciones o exámenes

Nota: El archivo `simula.o` debe ser compatible con tu sistema operativo y versión de compilador.

---

Consejo: Para proyectos, usa **Opción 2 (make)**. Para aprender o depurar, usa **Opción 1**. Si te dan `simula.o`, usa **Opción 3**.

---

## 11. Generación y Visualización de Mapas

### 11.1 Generador de Mapas Aleatorios

El simulador incluye una herramienta para generar mapas de prueba con diferentes configuraciones de obstáculos y suciedad.

#### Compilar el Generador

```bash
make mapgen      # Compila con biblioteca precompilada (distribución)
make mapgen-dev  # Compila desde fuentes (desarrollo)
```

#### Generar Mapas

```bash
./maps/generate
```

Esto creará 8 mapas de prueba en el directorio `maps/`:

- **noobs.pgm** - Sin obstáculos, solo suciedad (ideal para principiantes)
- **random1.pgm** - Obstáculos dispersos (densidad baja: 1%)
- **random3.pgm** - Obstáculos dispersos (densidad media: 3%)
- **random5.pgm** - Obstáculos dispersos (densidad alta: 5%)
- **walls1.pgm** - 1 muro (vertical u horizontal)
- **walls2.pgm** - 2 muros de la misma orientación
- **walls3.pgm** - 3 muros de la misma orientación
- **walls4.pgm** - 4 muros de la misma orientación

Todos los mapas incluyen:
- Base del robot en posición aleatoria en las paredes
- Orientación perpendicular a la pared donde está la base
- 50 celdas con suciedad distribuidas aleatoriamente
- Garantía de que los muros no tocan los bordes del mapa

### 11.2 Visualizador de Mapas

Puedes visualizar cualquier mapa sin necesidad de ejecutar el simulador completo.

#### Compilar el Visualizador

```bash
make viewmap      # Compila con biblioteca precompilada (distribución)
make viewmap-dev  # Compila desde fuentes (desarrollo)
```

#### Visualizar un Mapa

```bash
./maps/viewmap maps/walls2.pgm
./maps/viewmap maps/random3.pgm
./maps/viewmap maps/noobs.pgm
```

El visualizador muestra el mapa en formato ASCII:
- `#` = Obstáculos/muros
- `B` = Base del robot
- `1`-`5` = Nivel de suciedad (1=bajo, 5=alto)
- Espacios = Celdas vacías

#### Ejemplo de Uso

```bash
# Generar mapas
./maps/generate

# Visualizar un mapa específico
./maps/viewmap maps/walls3.pgm

# Ejecutar el simulador con ese mapa
./roomba maps/walls3.pgm
```

## 12. Depuración y Visualización

### 12.1 Ver los Resultadostados

Después de ejecutar tu programa, se generan automáticamente:

1. **`log.csv`** - Historial completo de posiciones del robot
2. **`stats.csv`** - Estadísticas finales (usado para puntuación)
3. **`map.pgm`** - Imagen del mapa usado

### 12.2 Leer las Estadísticas

El archivo `stats.csv` contiene toda la información necesaria para calcular tu puntuación. Ver **Sección 7.2** para detalles completos sobre el formato y significado de cada columna.

```bash
cat stats.csv
```

Ejemplo de salida:
```
0,0,2304,1245,310,285,1000.0,234.5,450,123,45,285,2
```

**Interpretación rápida:**
- Cobertura: 1245/2304 = 54% de celdas visitadas
- Limpieza: 285/310 = 92% de suciedad limpiada
- Batería: 234.5/1000 = 23.5% conservada
- Acciones: 450 avances, 123 giros, 45 colisiones, 285 limpiezas, 2 recargas

**Para autoevaluarte:**
```bash
./competition/myscore stats.csv
```

Ver **Sección 7.1** para más información sobre autoevaluación con `myscore`.

### 12.3 Cargar un Mapa Existente

Puedes cargar un mapa guardado para practicar en el mismo escenario. Hay dos formas:

#### Opción 1: Desde el Código

Llama a `load_map()` antes de `configure()`:

```c
int main() {
    load_map("map.pgm");  // Carga un mapa específico
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

#### Opción 2: Por Línea de Comandos (Recomendado)

Permite especificar el mapa al ejecutar el programa:

```c
int main(int argc, char *argv[]) {
    // Si hay argumento, cargar el mapa especificado
    if (argc > 1) {
        if (load_map(argv[1]) != 0) {
            printf("Error al cargar el mapa\n");
            return 1;
        }
    }
    // Si no hay argumento, se genera un mapa aleatorio
    
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

Ejecución:

```bash
./roomba                    # Genera mapa aleatorio
./roomba mi_mapa.pgm        # Usa mi_mapa.pgm
./roomba maps/dificil.pgm   # Usa maps/dificil.pgm
```

**Ventaja:** La opción por línea de comandos te permite probar el mismo código en diferentes mapas sin recompilar.

### 12.4 Imprimir Información de Depuración

```c
void comportamiento() {
    sensor_t s = rmb_state();
    
    // Imprime estado cada 100 ciclos
    static int contador = 0;
    if (++contador % 100 == 0) {
        printf("Tick %d - Pos:(%d,%d) Bat:%.1f Suciedad:%d\n",
               contador, s.x, s.y, s.battery, s.infrared);
    }
    
    // Tu código aquí...
}
```

---

## Desafíos y Ejercicios

### Nivel Básico

1. **Cuadrado Perfecto**: Haz que el robot dibuje un cuadrado de 10x10 celdas
2. **Limpiador Simple**: Limpia al menos 50% de la suciedad del mapa
3. **Explorador**: Visita al menos 30% de las celdas del mapa

### Nivel Intermedio

4. **Gestor de Batería**: Implementa recarga automática cuando batería < 20%
5. **Espiral Completa**: Cubre el área en espiral sin repetir celdas
6. **Limpieza Profunda**: Limpia el 80% de la suciedad

### Nivel Avanzado

7. **Cobertura Completa**: Visita el 95% de las celdas accesibles
8. **Eficiencia Energética**: Limpia 50+ unidades de suciedad con menos de 500 unidades de batería
9. **Robot Inteligente**: Implementa una estrategia que adapte su comportamiento según el nivel de batería y suciedad detectada

---

## Referencia Rápida de Funciones

| Función | Descripción | Coste Batería |
|---------|-------------|---------------|
| `rmb_awake(&x, &y)` | Activa el robot, devuelve posición inicial | 0 |
| `rmb_forward()` | Avanza un paso | 1.0 o 1.4 |
| `rmb_turn(angulo)` | Gira en radianes | 0.1 |
| `rmb_clean()` | Limpia celda actual | 0.5 |
| `rmb_load()` | Recarga +10 (solo en base) | 0 |
| `rmb_state()` | Lee todos los sensores | 0 |
| `rmb_bumper()` | Lee bumper (0 o 1) | 0 |
| `rmb_ifr()` | Lee infrarrojo (0-5) | 0 |
| `rmb_battery()` | Lee batería (0-1000) | 0 |
| `rmb_at_base()` | Verifica si está en base | 0 |

---

## Consejos Finales

1. **Prueba Incrementalmente**: Empieza simple y añade complejidad gradualmente
2. **Usa Estados**: Las máquinas de estados hacen el código más claro
3. **Gestiona la Batería**: Siempre ten un plan para volver a la base
4. **Visualiza**: Usa `visualize()` para ver qué está haciendo tu robot
5. **Analiza Estadísticas**: Revisa `stats.csv` para mejorar tu estrategia

---

## Problemas Comunes

### El robot no se mueve
- ¿Llamaste a `rmb_awake()` en `inicializar()`?
- ¿Estás llamando a `rmb_forward()` o `rmb_turn()`?

### El robot choca constantemente
- Verifica con `rmb_bumper()` antes de avanzar
- Gira cuando detectes obstáculo

### Se queda sin batería
- Reduce la cantidad de acciones
- Implementa regreso a base cuando batería < 200
- Usa `rmb_load()` en la base

### No limpia nada
- Verifica que estés llamando a `rmb_clean()`
- Asegúrate de que `rmb_ifr() > 0` antes de limpiar
- Recuerda que puede necesitar múltiples limpiezas por celda

---

## Conclusión

¡Felicidades! Ahora tienes todas las herramientas para programar robots de limpieza inteligentes. Experimenta con diferentes estrategias y compara los resultados.

**Siguiente paso:** Revisa el código de `samples/` para ver más ejemplos avanzados.

---

**Versión:** 2025  
**Última actualización:** Diciembre 2025
