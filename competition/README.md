# Sistema de Competición de Roombas

Sistema automatizado para ejecutar y evaluar competiciones entre múltiples equipos de robots Roomba.

## Arquitectura

El sistema de competición utiliza una arquitectura modular que separa:

1. **Simulador base** (`simula.c` y módulos): Sin modificaciones
2. **Extensiones de competición** (`competition_ext.c`): Lógica específica de competición
3. **Biblioteca precompilada** (`lib/libsimula.a`): Versión unificada para todos los equipos
4. **Runner** (`runner.c`): Orquestador de la competición

### Ventajas de esta arquitectura:

- ✅ Simulador base sin modificar (desarrollo normal no afectado)
- ✅ Todos los equipos usan la **misma versión** del simulador (libsimula.a)
- ✅ Compilación rápida (sin recompilar fuentes con cada equipo)
- ✅ Extensible mediante `competition_ext.c`
- ✅ Overrides de funciones mediante `simula_comp.c` con weak symbols

## Estructura de directorios

```
competition/
├── README.md                  # Esta documentación
├── Makefile                   # Build system de competición
├── runner.c                   # Programa orquestador
├── runner                     # Ejecutable del runner
├── competition_ext.h          # API de extensiones
├── competition_ext.c          # Implementación de lógica de competición
├── simula_comp.c             # Overrides de funciones (run, save_stats)
├── lib/
│   ├── simula.o              # Biblioteca precompilada (21KB)
│   ├── simula.h              # Headers (copia desde raíz)
│   ├── simula_internal.h
│   └── README.md             # Documentación de la biblioteca
├── teams/                    # Código fuente de equipos
│   ├── team01/
│   │   ├── main.c
│   │   ├── config.txt        # Generado automáticamente
│   │   └── roomba           # Ejecutable compilado
│   ├── team02/
│   └── ...
├── maps/                     # Mapas oficiales de competición
│   ├── README.md
│   └── (copiar mapas .pgm aquí)
├── results/                  # Resultados por timestamp
│   ├── run_20251205_143000/
│   │   ├── stats.csv
│   │   └── ranking.txt
│   └── latest -> run_20251205_143000/
├── scripts/                  # Scripts auxiliares
│   └── (scripts de análisis)
├── stats.csv                 # Estadísticas actuales
└── ranking.txt               # Clasificación actual
```

## Uso rápido

### Ejecutar competición completa:

```bash
cd competition
make run         # O: make run-archived (guarda con timestamp)
```

El runner automáticamente:
1. Construye `lib/simula.o` si no existe
2. Descubre todos los equipos en `entregas/`
3. Compila cada equipo con la biblioteca unificada
4. Ejecuta 20 rondas por equipo (4 mapas × 5 repeticiones)
5. Genera `stats.csv` y `ranking.txt`

### Reconstruir biblioteca manualmente:

```bash
cd ..  # Ir al directorio raíz del proyecto
make lib-competition
```

Esto genera `competition/lib/simula.o` con todos los módulos del simulador.

## Compilación manual de un equipo

Si quieres compilar un equipo manualmente:

```bash
cd competition/teams/team01
gcc -I../../lib main.c ../../lib/simula.o ../../competition_ext.c ../../simula_comp.c -lm -o roomba
```

O usa el Makefile:

```bash
cd competition
make test TEAM=team01
```

## Sistema de mapas rotatorio

Cada equipo ejecuta 20 simulaciones:
- **4 tipos de mapa** (configurables en `config.txt`)
- **5 repeticiones** por mapa
- Rotación automática gestionada por `competition_ext.c`

El archivo `config.txt` se genera y actualiza automáticamente:

```
map_file=0        # 0-3: tipo de mapa actual
map_type=0        # Mismo valor
run_counter=1     # 1-5: repetición actual
```

## Formato de estadísticas

`stats.csv` contiene todas las métricas:

```csv
team,map_type,cell_total,cell_visited,dirt_total,dirt_cleaned,bat_total,bat_mean,forward,turn,bumps,clean,load
team01,0,2400,1523,127,95,100.0,67.3,1245,89,23,95,1
team01,1,2400,1687,134,121,100.0,52.1,1534,102,18,121,2
...
```

## Clasificación

El ranking se calcula según:
- **Celdas visitadas** (cobertura del mapa)
- **Suciedad limpiada** (eficacia)
- **Batería utilizada** (eficiencia)

Fórmula de puntuación: `score = cells_visited + (dirt_cleaned * 10) - (battery_used * 5)`

Ejemplo de `ranking.txt`:

```
Rank  Team            Score    Cells    Dirt    Battery
-----------------------------------------------------------
1     team07          8734     2145     189     32.5
2     team13          8521     2087     201     35.2
3     team04          8234     1989     176     28.9
...
```

## Configuración de la competición

Editar `competition_ext.h` para ajustar:

```c
#define COMP_MAPS_COUNT 4           // Número de mapas diferentes
#define COMP_REPS_PER_MAP 5         // Repeticiones por mapa
#define COMP_STATS_FILE "../stats.csv"  // Archivo de estadísticas
```

## Solución de problemas

### Error: `simula.o not found`

```bash
cd /path/to/roomba
make lib-competition
```

### Error: Compilación de equipo falla

Verificar que el archivo del equipo use la API correcta:
- Usar `rob.heading` (no `rob.head` - versión antigua)
- Incluir `simula.h` correctamente

### Limpiar y reconstruir todo

```bash
cd competition
rm -f lib/simula.o runner stats.csv ranking.txt
rm -f teams/*/roomba teams/*/config.txt
make lib         # Regenera la biblioteca
make runner      # Recompila el runner
make run         # Ejecuta competición
```

## Desarrollo y mantenimiento

### Modificar el simulador base

Si modificas `simula.c`, `sim_robot.c`, etc.:

```bash
make lib-competition  # Reconstruir biblioteca
```

### Añadir funcionalidad de competición

Editar `competition_ext.c` para nueva lógica sin tocar el simulador base.

### Debugging

Compilar runner con símbolos de depuración:

```bash
gcc runner.c -o runner -Wall -Wextra -g
```

Ejecutar un equipo individual con valgrind:

```bash
cd entregas/team01
valgrind ./roomba
```

## Créditos

Sistema de competición desarrollado para IPR-GIIROB-ETSINF-UPV
Fecha: Diciembre 2025
