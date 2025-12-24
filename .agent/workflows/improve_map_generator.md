---
description: Workflow para mejorar la generación de mapas con niveles de dificultad
---

# Plan de Mejora: Generador de Mapas Avanzado

Este plan describe cómo transformar el generador de mapas actual (`maps/generate.c`) en una herramienta versátil que soporte niveles de dificultad y generación personalizada.

## Fase 1: Soporte de Argumentos CLI

Modificar `maps/generate.c` para aceptar argumentos de línea de comandos:
- `./generate [mode] [output_file]`
- Si no hay argumentos: Ejecutar comportamiento actual (batch de 8 mapas).
- Si hay argumentos: Generar un único mapa según el modo.

## Fase 2: Implementación de Modos de Dificultad

Definir los siguientes modos en el código:

1.  **EASY** (`--mode=easy`):
    - Obstáculos: 0 (Campo abierto).
    - Suciedad: Uniformemente distribuida.
    - Llamada actual: `map_generate(..., density=0)`

2.  **MEDIUM** (`--mode=medium`):
    - Obstáculos: 5% de densidad aleatoria o 1-2 muros simples.
    - Llamada actual: `map_generate(..., density=0.05)`

3.  **HARD** (`--mode=hard`):
    - Obstáculos: Alta densidad (8-10 muros o 15% obstáculos).
    - Objetivo: Crear pasillos estrechos y callejones sin salida.
    - Llamada actual: `map_generate(..., density=8.0)` (Truco: usar muros como obstáculos complejos).

## Fase 3: Futuro (Requiere tocar `sim_world.c`)

Cuando sea posible refactorizar el simulador, añadir soporte real para:

4.  **MULTIROOM** (`--mode=multiroom`):
    - Dividir el mapa en cuadrantes.
    - Abrir puertas aleatorias.
    - Requiere nueva función `sim_world_generate_rooms(...)`.

5.  **DIRT CLUSTERS**:
    - Concentrar la basura en puntos específicos.
    - Requiere tocar `map_dirty_random(...)`.

## Notas de Implementación

- Mantener la retrocompatibilidad: `make mapgen` debe seguir funcionando sin cambios.
- Usar `getopt` o parseo manual simple en `main`.
- Factorizar la generación del batch por defecto en una función separada `generate_default_suite()`.
