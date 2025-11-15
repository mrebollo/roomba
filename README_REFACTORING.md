# Roomba Simulator - Refactorización 2025

## Estructura del Proyecto

```
roomba/
├── simula.h              # API pública (visible a estudiantes)
├── simula.c              # Núcleo: configure(), run(), wrappers públicos
├── sim_robot.c           # Acciones del robot y helpers (rmb_*)
├── sim_world.c           # Mundo: generar/cargar/guardar mapas (con map_t*)
├── sim_visual.c          # Visualización ASCII + HUD + Ctrl-C
├── sim_io.c              # I/O de fichero: log.csv y stats.csv
├── main.c                # Proyecto del estudiante
├── Makefile              # Compilación para estudiantes
├── samples/              # Ejemplos para estudiantes
│   ├── main.c
│   ├── roomba.c
│   └── ...
└── competition/          # Versión competición (profesor)
  ├── simula_comp.c    # Wrapper modo competición
  ├── runner.c         # Ejecutor de torneos
  └── Makefile         # Compilación competición
```

Nota: existe un generador de mapas de apoyo en `maps/generate.c`, pensado para uso del profesor. No es necesario para el flujo del estudiante y no forma parte del binario ni del Makefile por defecto.

## Para Estudiantes

### Compilar y Ejecutar
```bash
make              # Compila el proyecto
make run          # Compila y ejecuta
make clean        # Limpia archivos generados
```

### Estructura del Código del Estudiante
```c
#include "simula.h"

void on_start() {
    int basex, basey;
    rmb_awake(&basex, &basey);
    // Inicialización...
}

void cyclic_behav() {
    rmb_forward();
    if(rmb_bumper()) {
        rmb_turn(angle);
    }
    // Comportamiento cíclico...
}

void on_stop() {
    visualize();  // Visualización disponible
}

int main(int argc, char *argv[]) {
    if(argc > 1) load_map(argv[1]);
    configure(on_start, cyclic_behav, on_stop, 100);
    run();
    return 0;
}
```

### Características Modo Estudiante
- ✓ Visualización en tiempo real (`visualize()`)
- ✓ Mensajes de debug (`printf` habilitados)
- ✓ Archivos locales (`log.csv`, `stats.csv`)
- ✓ Carga de mapas personalizados
- ✓ Un solo mapa por ejecución

## Para Competición (Profesor)

### Estructura de Equipos
```
teams/
├── team1/
│   └── main.c
├── team2/
│   └── main.c
└── ...
```

### Ejecutar Competición
```bash
cd competition/
make runner       # Compila el ejecutor
make run          # Ejecuta la competición completa
make test TEAM=team1  # Prueba compilar un equipo
```

### Características Modo Competición
- ✗ Sin visualización (compilación con `-DCOMPETITION_MODE`)
- ✗ Sin mensajes de debug
- ✓ Ejecución automática de 4 mapas × 5 repeticiones
- ✓ Estadísticas centralizadas en `stats.csv`
- ✓ Ranking automático en `ranking.txt`
- ✓ Timeout de seguridad (30s por ejecución)

### Archivos de Competición

#### `competition/simula_comp.c`
Wrapper que:
- Incluye `simula.c` con flag `COMPETITION_MODE`
- Gestiona rotación de mapas vía `config.txt`
- Acumula estadísticas centralizadas
- Identifica equipos por carpeta

#### `competition/runner.c`
Ejecutor que:
- Descubre automáticamente equipos en `../teams/`
- Compila cada equipo con `simula_comp.c`
- Ejecuta todas las rondas (4 mapas × 5 reps)
- Genera ranking basado en:
  - Suciedad limpiada (prioridad)
  - Celdas visitadas
  - Batería consumida (penalización)

## Cambios Principales

### 1. Modularización interna
- **sim_robot.c:** acciones del robot (`rmb_*`) y lógica de batería/tiempos.
- **sim_world.c:** generación/carga/guardado de mapas; API con `map_t*` (sin globales ocultos).
- **sim_visual.c:** impresión del mapa, HUD y manejo de `SIGINT` (Ctrl-C para salir).
- **sim_io.c:** volcado de `log.csv` y `stats.csv` desacoplado de estado global.
- **simula.c:** orquestación (configuración, ciclo principal, atexit) manteniendo la API pública estable.

### 2. Compilación condicional
```c
#ifndef COMPETITION_MODE
  #define DEBUG_PRINT(...) printf(__VA_ARGS__)
  #define ENABLE_VISUALIZATION 1
#else
  #define DEBUG_PRINT(...)  // Sin output
  #define ENABLE_VISUALIZATION 0
#endif
```

### 3. Limpieza de código
- ✓ Eliminado código comentado obsoleto
- ✓ Eliminados TODOs resueltos
- ✓ Eliminadas variables no utilizadas
- ✓ Verificaciones de punteros NULL
- ✓ Corrección de tipos (float vs int)

### 4. Reutilización
- **~95%** del código es compartido
- Solo difieren: visualización, debug, gestión de archivos
- Mantenimiento centralizado

## Ventajas

### Para Estudiantes
1. Proyecto simple y cohesivo
2. Include único: `simula.h`
3. Visualización y debugging incluidos
4. Makefile simple

### Para Profesor
1. Carpeta separada para competición
2. Ejecución automática sin intervención
3. Ranking automático
4. Reutiliza el código de estudiantes
5. Fácil actualizar el simulador (un solo lugar)

## Migración desde Versión Anterior

Si tenías proyectos con la versión anterior:
1. Reemplazar `#include "simula_v3_2024.h"` → `#include "simula.h"`
2. El código del estudiante no necesita cambios
3. Recompilar con el nuevo Makefile

## Uso de Mapas

### Generar mapa aleatorio
```bash
./roomba              # Genera mapa automático
```

### Cargar mapa existente
```bash
./roomba maps/custom.pgm
```

### Formato PGM
```
P2
# roomba map
50 50
255
128 128 128 ...    # 128=muro, 255=vacío, 0=base, 1-9=suciedad
```

## Desarrollo Futuro

Posibles mejoras manteniendo compatibilidad:
- Añadir nuevos sensores en `sensor_t`
- Nuevas acciones `rmb_*()` en `simula.c`
- Diferentes algoritmos de scoring en `runner.c`
- Visualización web (sin afectar código estudiante)

## Detalles de módulos

- sim_robot.c
  - Expone la API del robot usada por estudiantes (`rmb_awake/turn/forward/clean/load` y consultas `rmb_*`).
  - Controla el avance del tiempo (tick) y acumula media de batería (expuesta internamente vía `sim_robot_battery_mean()`).

- sim_world.c
  - Todas las funciones aceptan un `map_t*` explícito (sin depender de `map` global).
  - `sim_world_generate`, `sim_world_load`, `sim_world_save`, `sim_world_put_base`, `sim_world_set_base_origin`.

- sim_visual.c
  - Funciones de visualización, barra de progreso, brújula y trayectoria.
  - Maneja Ctrl-C con `SIGINT` y `nanosleep` para una salida limpia.

- sim_io.c
  - `save_log(hist, len)` y `save_stats(&stats)` escriben en el directorio actual (`log.csv`, `stats.csv`).
  - No usan estado global oculto: reciben los datos por parámetro.

---

**Nota:** Los estudiantes solo necesitan conocer `simula.h` y modificar `main.c`. La carpeta `competition/` es solo para el profesor.
