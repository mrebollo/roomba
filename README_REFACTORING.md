# Roomba Simulator - Refactorización 2025

## Estructura del Proyecto

```
roomba/
├── simula.h                    # API pública para estudiantes
├── simula.c                    # Simulador completo (versión estudiante)
├── main.c                      # Proyecto del estudiante
├── Makefile                    # Compilación para estudiantes
├── maps/                       # Mapas de entorno
│   └── generate.c
├── samples/                    # Ejemplos para estudiantes
│   ├── main.c
│   ├── roomba.c
│   └── ...
└── competition/                # Versión competición (profesor)
    ├── simula_comp.c          # Wrapper modo competición
    ├── runner.c               # Ejecutor de torneos
    └── Makefile               # Compilación competición
```

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

### 1. Unificación
- **Antes:** `simula_v3_2024.c` y `simulacomp_24.c` (código duplicado)
- **Ahora:** `simula.c` + flags condicionales

### 2. Compilación Condicional
```c
#ifndef COMPETITION_MODE
  #define DEBUG_PRINT(...) printf(__VA_ARGS__)
  #define ENABLE_VISUALIZATION 1
#else
  #define DEBUG_PRINT(...)  // Sin output
  #define ENABLE_VISUALIZATION 0
#endif
```

### 3. Limpieza de Código
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

### Generar Mapa Aleatorio
```bash
./roomba              # Genera mapa automático
```

### Cargar Mapa Existente
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

---

**Nota:** Los estudiantes solo necesitan conocer `simula.h` y modificar `main.c`. La carpeta `competition/` es solo para el profesor.
