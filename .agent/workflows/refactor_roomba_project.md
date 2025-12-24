---
description: Workflow para refactorizar la estructura del proyecto Roomba post-competición
---

# Plan de Refactorización del Proyecto Roomba

Este workflow reorganiza el proyecto para separar claramente el núcleo del simulador, las herramientas de gestión y el kit de estudiante.

**Pre-requisitos:**
- La competición debe haber finalizado.
- Hacer un backup del proyecto actual antes de empezar.

## Fase 1: Creación de Estructura de Directorios

1. Crear la nueva jerarquía de carpetas:
   ```bash
   mkdir -p core/src core/include
   mkdir -p tools/runner tools/validator tools/mapgen tools/scorer tools/viewer
   mkdir -p competition/maps competition/config
   mkdir -p student_kit
   mkdir -p examples
   ```

## Fase 2: Migración del Core (Simulador)

2. Mover archivos fuente del simulador a `core/src`:
   - `simula.c`, `sim_world.c`, `sim_robot.c`, `sim_io.c`, `sim_stats.c`, `sim_visual.c`, `sim_world_api.c` -> `core/src/`
   
3. Mover cabeceras a `core/include`:
   - `simula.h`, `sim_world_api.h`, `simula_internal.h`, `sim_visual.h` -> `core/include/`

4. Crear `core/Makefile` para compilar la librería estática `simula.o` (o `.a`).

## Fase 3: Migración de Herramientas (Tools)

5. Mover y organizar herramientas:
   - `competition/runner.c` -> `tools/runner/main.c`
   - `tools/validate.c` -> `tools/validator/main.c`
   - `maps/generate.c` (o `tools/generate.c`) -> `tools/mapgen/main.c`
   - `competition/score.c` -> `tools/scorer/main.c`
   - `tools/viewmap.c` -> `tools/viewer/main.c`
   - `tools/libscore.*` -> `tools/scorer/`

6. Actualizar los `#include` en cada herramienta para apuntar a `../../core/include`.

## Fase 4: Preparación del Entorno de Competición

7. Establecer archivos de datos:
   - Mover mapas oficiales (`.pgm`) a `competition/maps/`.
   - Mover `scoring.conf` a `competition/config/`.
   - Asegurar que `competition/teams` existe y está vacío (o con ejemplos).

8. Crear `competition/Makefile` que invoque a las herramientas compiladas en `tools/`.

## Fase 5: Creación del Kit de Estudiante

9. Preparar `student_kit`:
   - Copiar `core/include/simula.h`.
   - Copiar el objeto compilado `simula.o` (desde core).
   - Crear un `main.c` básico (plantilla).
   - Crear un `Makefile` sencillo para que el alumno compile con `make`.

## Fase 6: Ejemplos y Limpieza

10. Mover implementaciones de referencia:
    - `competition/teams/reference/main.c` -> `examples/reference_wall_follow.c`
    - Otros bots de ejemplo -> `examples/`

11. Eliminar archivos redundantes o antiguos de la raíz y limpiar residuos (`.o`, ejecutables antiguos).

## Fase 7: Verificación Final

12. Ejecutar un test de integración:
    - Compilar Core.
    - Compilar Tools.
    - Compilar un bot de ejemplo en `student_kit`.
    - Ejecutar una mini-competición con el `runner` refactorizado.

// Fin del plan
