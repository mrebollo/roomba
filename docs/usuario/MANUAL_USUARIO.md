# 🤖 Manual del Usuario - Simulador Roomba

## 📘 Guía Paso a Paso para Programar tu Robot

Este manual te enseñará a programar comportamientos para un robot de limpieza virtual. No necesitas experiencia previa en robótica, solo conocimientos básicos de programación en C.

---

## 📑 Tabla de Contenidos

1. [Introducción](#1-introducción)
2. [Tu Primer Programa](#2-tu-primer-programa)
3. [Entendiendo los Sensores](#3-entendiendo-los-sensores)
4. [Movimiento Básico](#4-movimiento-básico)
5. [Detección de Obstáculos](#5-detección-de-obstáculos)
6. [Limpieza Inteligente](#6-limpieza-inteligente)
7. [Gestión de Batería](#7-gestión-de-batería)
8. [Estrategias Completas](#8-estrategias-completas)
9. [Compilación del Proyecto](#9-compilación-del-proyecto)
10. [Depuración y Visualización](#10-depuración-y-visualización)

---

## 1. Introducción

### ¿Qué es el Simulador?

El simulador te permite programar un robot de limpieza virtual que se mueve en un mundo 2D. El robot puede:
- ✅ Moverse y girar
- ✅ Detectar obstáculos con su bumper
- ✅ Detectar suciedad con sensor infrarrojo
- ✅ Limpiar celdas sucias
- ✅ Recargar su batería en la base

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

**📝 Ejercicio:** Compila y ejecuta este programa. Observa dónde aparece el robot. Ver sección de **Compilación del Proyecto** más adelante para las opciones de compilación.

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

**🤔 Problema:** El robot no sabe evitar obstáculos todavía.

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

**💡 Recuerda:** 
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

**📝 Ejercicio:** Modifica el programa para que haga un triángulo en lugar de un cuadrado.

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

**💡 Estrategia:** Este es el comportamiento básico de un Roomba real - rebota aleatoriamente.

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

**💡 Importante:** `rmb_clean()` reduce el nivel de suciedad en 1. Si una celda tiene nivel 3, necesitarás limpiar 3 veces.

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

**⚠️ Cuidado:** Cada `rmb_clean()` consume batería (0.5 unidades). Asegúrate de tener suficiente.

---

## 7. Gestión de Batería

### Costes de Batería

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

**💡 Importante:** La función `finalizar()` es **OBLIGATORIA** y debe incluir `visualize()`. Se ejecuta automáticamente cuando termina `run()` mediante `atexit()`. También puedes usarla para mostrar estadísticas personalizadas.

---

## 8. Estrategias Completas

### Ejemplo 9: Robot Autónomo Completo

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

## 9. Compilación del Proyecto

Existen tres formas de compilar tu programa con el simulador:

### Opción 1: Compilación Directa desde Fuentes

Compila todos los archivos fuente directamente con `gcc`:

```bash
gcc main.c simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c -lm -o roomba
./roomba
```

**✅ Ventajas:**
- Control total sobre el proceso de compilación
- No requiere archivos adicionales
- Útil para entender cómo funciona la compilación

**❌ Desventajas:**
- Comando largo y propenso a errores
- Recompila todo cada vez (lento)

---

### Opción 2: Usar Makefile (Recomendado)

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

**✅ Ventajas:**
- Comando corto y fácil de recordar
- Recompila solo lo necesario (rápido)
- Estándar en proyectos C/C++

---

### Opción 3: Compilación con Objeto Precompilado

Si el profesor proporciona un archivo `simula.o` precompilado, puedes compilar solo tu `main.c`:

```bash
gcc main.c simula.o -lm -o roomba
./roomba
```

**✅ Ventajas:**
- Compilación muy rápida (solo tu código)
- No necesitas los fuentes del simulador
- Útil para competiciones o exámenes

**⚠️ Nota:** El archivo `simula.o` debe ser compatible con tu sistema operativo y versión de compilador.

---

**💡 Consejo:** Para proyectos, usa **Opción 2 (make)**. Para aprender o depurar, usa **Opción 1**. Si te dan `simula.o`, usa **Opción 3**.

---

## 10. Depuración y Visualización

### Ver los Resultados

Después de ejecutar tu programa, se generan automáticamente:

1. **`log.csv`** - Historial completo de posiciones
2. **`stats.csv`** - Estadísticas finales
3. **`map.pgm`** - Imagen del mapa

### Leer las Estadísticas

```bash
cat stats.csv
```

Ejemplo de salida:
```
cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, forward, turn, bumps, clean, load
2285, 87, 310, 25, 850.5, 720.3, 87, 45, 12, 50, 3
```

**Interpretación:**
- `cell_total`: 2285 celdas totales en el mapa
- `cell_visited`: 87 celdas visitadas (3.8% del mapa)
- `dirt_total`: 310 unidades de suciedad iniciales
- `dirt_cleaned`: 25 unidades limpiadas (8% del total)
- `bat_total`: 850.5 unidades de batería consumidas
- `bat_mean`: 720.3 promedio de batería por tick
- Movimientos: 87 avances, 45 giros, 12 choques, 50 limpiezas, 3 recargas

### Cargar un Mapa Existente

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

**💡 Ventaja:** La opción por línea de comandos te permite probar el mismo código en diferentes mapas sin recompilar.

### Imprimir Información de Depuración

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

## 🎯 Desafíos y Ejercicios

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

## 📚 Referencia Rápida de Funciones

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

## 💡 Consejos Finales

1. **Prueba Incrementalmente**: Empieza simple y añade complejidad gradualmente
2. **Usa Estados**: Las máquinas de estados hacen el código más claro
3. **Gestiona la Batería**: Siempre ten un plan para volver a la base
4. **Visualiza**: Usa `visualize()` para ver qué está haciendo tu robot
5. **Analiza Estadísticas**: Revisa `stats.csv` para mejorar tu estrategia

---

## 🆘 Problemas Comunes

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

## 🎓 Conclusión

¡Felicidades! Ahora tienes todas las herramientas para programar robots de limpieza inteligentes. Experimenta con diferentes estrategias y compara los resultados.

**Siguiente paso:** Revisa el código de `samples/` para ver más ejemplos avanzados.

---

**Versión:** 2025  
**Última actualización:** Diciembre 2025
