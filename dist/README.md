# Simulador Roomba - Entorno de Programación para Estudiantes

Bienvenido al simulador Roomba. Este paquete contiene todo lo necesario para programar tu robot limpiador.

## Archivos del Proyecto

### Tu Código
- **`main.c`** - Programa aquí el comportamiento de tu robot (EDITAR AQUÍ)

### Biblioteca del Simulador
- **`simula.o`** - Biblioteca precompilada del simulador (NO MODIFICAR)
- **`simula.h`** - API con todas las funciones disponibles (CONSULTAR)

### Mapas de Prueba
- **`maps/`** - Diferentes escenarios para probar tu robot:
  - `noobs.pgm` - Mapa simple sin obstáculos
  - `random1.pgm`, `random3.pgm`, `random5.pgm` - Obstáculos dispersos (1%, 3%, 5%)
  - `walls1.pgm`, `walls2.pgm`, `walls3.pgm`, `walls4.pgm` - Muros verticales u horizontales

---

## Compilar y Ejecutar

### Compilación básica
```bash
make            # Compilar el proyecto
./roomba        # Ejecutar (genera mapa aleatorio)
```

### Usar un mapa específico
```bash
./roomba MAP=maps/noobs.pgm
./roomba MAP=maps/random3.pgm
./roomba MAP=maps/walls2.pgm
```

### Limpiar archivos generados
```bash
make clean
```

---

## Archivos Generados

Después de cada ejecución se crean:

- **`log.csv`** - Trayectoria completa del robot
  - Columnas: x, y, orientación, batería, suciedad, bumper, etc.
  
- **`stats.csv`** - Estadísticas finales del recorrido
  - Celdas visitadas, suciedad limpiada, batería consumida, movimientos
  
- **`map.pgm`** - Imagen del mundo final (formato PGM P2)
  - Se puede abrir con GIMP, Photoshop, o visualizadores de imágenes

---

## API del Simulador

Todas las funciones están documentadas en `simula.h`. Resumen rápido:

### Configuración del Simulador

```c
void configure(void (*on_start)(), void (*exec_beh)(), void (*on_finish)(), int exec_time);
void run();
```

**Ejemplo:**
```c
configure(inicializar, comportamiento, finalizar, 1000);
run();
```

### Sensores

```c
sensor_t rmb_state();              // Estado completo del robot
float rmb_battery();               // Batería actual (0-1000)
int rmb_bumper();                  // ¿Ha chocado? (1=sí, 0=no)
int rmb_ifr();                     // Nivel de suciedad en la celda (0-5)
void rmb_awake(int *x, int *y);    // Obtener posición de la base
```

**Estructura sensor_t:**
```c
typedef struct {
    int x, y;           // Posición (columna, fila)
    float heading;      // Orientación en radianes (0=Este, π/2=Norte)
    float battery;      // Batería restante
    int bumper;         // Colisión detectada
    int dirt;           // Nivel de suciedad
} sensor_t;
```

### Actuadores

```c
void rmb_forward();                // Avanzar 1 celda en la dirección actual
void rmb_turn(float rad);          // Girar (ángulo en radianes)
void rmb_clean();                  // Limpiar 1 unidad de suciedad
void rmb_load();                   // Recargar 1 unidad de batería
```

**Constantes útiles:**
- `M_PI` = π (3.14159...)
- `M_PI / 2` = 90 grados
- `M_PI / 4` = 45 grados
- `-M_PI / 2` = -90 grados (girar a la derecha)

### Utilidades

```c
void visualize();                  // Mostrar animación ASCII del recorrido
void save_stats();                 // Guardar estadísticas (automático)
int load_map(char *filename);      // Cargar un mapa PGM
```

---

## Estructura Básica de un Programa

```c
#include "simula.h"
#include <math.h>

// Variables globales (opcional)
int base_x, base_y;

// Se ejecuta UNA VEZ al inicio
void inicializar() {
    rmb_awake(&base_x, &base_y);
    // Tu código de inicialización
}

// Se ejecuta EN CADA PASO (bucle principal)
void comportamiento() {
    // Ejemplo: rebote simple
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);  // Girar 90° si choca
    } else {
        rmb_forward();       // Avanzar si no hay obstáculo
    }
}

// Se ejecuta UNA VEZ al final
void finalizar() {
    visualize();  // Mostrar animación
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

---

## Ejemplos de Código

### Ejemplo 1: Limpiador simple (rebote aleatorio)

```c
void comportamiento() {
    // Prioridad: limpiar si hay suciedad
    if (rmb_ifr() > 0) {
        rmb_clean();
        return;
    }
    
    // Navegar con rebote aleatorio
    if (rmb_bumper()) {
        float angulo = (rand() % 180 - 90) * M_PI / 180.0;
        rmb_turn(angulo);
    } else {
        rmb_forward();
    }
}
```

### Ejemplo 2: Gestión de batería

```c
typedef enum { EXPLORANDO, REGRESANDO, RECARGANDO } Estado;
Estado estado = EXPLORANDO;

void comportamiento() {
    if (rmb_battery() < 200 && estado == EXPLORANDO) {
        estado = REGRESANDO;
    }
    
    switch(estado) {
        case EXPLORANDO:
            // Lógica de exploración
            break;
        case REGRESANDO:
            // Navegar hacia (base_x, base_y)
            break;
        case RECARGANDO:
            if (rmb_battery() < 900) {
                rmb_load();
            } else {
                estado = EXPLORANDO;
            }
            break;
    }
}
```

### Ejemplo 3: Seguir pared derecha

```c
int pasos = 0;

void comportamiento() {
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);  // Girar izquierda al chocar
        pasos = 0;
    } else {
        rmb_forward();
        pasos++;
        
        // Cada 3 pasos, girar derecha (seguir pared)
        if (pasos > 3) {
            rmb_turn(-M_PI / 4);
            pasos = 0;
        }
    }
}
```

---

## Consejos de Programación

1. **Compila frecuentemente** - Detecta errores pronto con `make`

2. **Usa `visualize()`** - Ver la animación ayuda a entender el comportamiento

3. **Consulta `stats.csv`** - Métricas objetivas:
   - `cell_visited / cell_total` = % de cobertura
   - `dirt_cleaned / dirt_total` = % de limpieza
   - `bat_total` = batería consumida

4. **Prueba en varios mapas** - Tu código debe funcionar en todos:
   ```bash
   for map in maps/*.pgm; do
       echo "Probando $map"
       ./roomba MAP=$map
   done
   ```

5. **Gestiona la batería** - Vuelve a la base antes de quedarte sin energía
   - Cada `forward()` consume ~1 unidad
   - Cada `turn()` consume ~0.5 unidades
   - Cada `clean()` consume ~0.5 unidades
   - Colisiones cuestan batería extra

6. **Usa máquinas de estados** - Más claro que muchos `if` anidados:
   ```c
   typedef enum { ESTADO1, ESTADO2, ESTADO3 } Estado;
   Estado actual = ESTADO1;
   
   switch(actual) {
       case ESTADO1: /* lógica */ break;
       case ESTADO2: /* lógica */ break;
       case ESTADO3: /* lógica */ break;
   }
   ```

---

## Solución de Problemas

### Error: "undefined reference to..."
- Verifica que estés compilando con `make` (no directamente con `gcc`)
- El Makefile incluye automáticamente `simula.o`

### El robot no se mueve
- Verifica que estés llamando a `rmb_forward()` o `rmb_turn()`
- Asegúrate de que el bucle no se detenga prematuramente

### Batería se agota muy rápido
- Evita colisiones (cada choque cuesta batería)
- Implementa recarga cuando `rmb_battery() < 200`

### El robot gira continuamente
- Verifica que no estés acumulando ángulos infinitamente
- Normaliza ángulos si es necesario

### La animación no se muestra
- Asegúrate de llamar a `visualize()` en la función `finalizar()`
- Presiona Ctrl+C para detener la animación si es muy larga

---

## Documentación Adicional

Para información más detallada, consulta:
- **Manual de Usuario** (PDF) - Explicación completa del simulador
- **Archivo `simula.h`** - Documentación de cada función
- **Ejemplos en `samples/`** - Código funcional de referencia

---

## Métricas de Éxito

Tu robot será evaluado según:
1. **Cobertura** - % de celdas visitadas
2. **Limpieza** - % de suciedad eliminada
3. **Eficiencia** - Batería consumida / trabajo realizado
4. **Autonomía** - Gestión de recarga automática

¡Buena suerte programando tu Roomba!
