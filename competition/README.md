# Sistema de Competición Roomba

Sistema completo de evaluación automática para competiciones de programación de robots Roomba.

## Descripción General

Este sistema permite a organizadores ejecutar competiciones de forma automática, evaluando múltiples equipos en diferentes escenarios y generando rankings basados en métricas objetivas.

## ¿Para quién es esto?

- **Organizadores/Profesores**: Usa `ORGANIZER_GUIDE.md` para configurar y ejecutar la competición
- **Participantes**: Consulta `STUDENT_GUIDE.md` para preparar tu entrega
- **Todos**: Lee `RULES.md` para entender los criterios de evaluación

## Quick Start (Organizadores)

```bash
# 1. Compilar herramientas
make all

# 2. Validar código de un equipo
./validate ../teams/team01

# 3. Ejecutar competición completa
./runner

# 4. Generar ranking
./score
```

## Estructura de Directorios

```
competition/
├── README.md              # Este archivo
├── RULES.md              # Reglas oficiales de puntuación
├── ORGANIZER_GUIDE.md    # Guía completa para organizadores
├── STUDENT_GUIDE.md      # Guía para participantes
│
├── Makefile              # Compilación de herramientas
├── scoring.conf          # Configuración de pesos y bonificaciones
│
├── runner.c              # Orquestador de ejecuciones
├── score.c               # Generador de ranking oficial
├── libscore.c/h          # Biblioteca compartida de puntuación
├── myscore.c             # Herramienta de autoevaluación (participantes)
├── competition_ext.c/h   # Extensiones de API
│
├── maps/                 # Mapas de prueba oficiales
│   ├── noobs.pgm         # Sin obstáculos (0%)
│   ├── random1.pgm       # Obstáculos al 1%
│   ├── random3.pgm       # Obstáculos al 3%
│   └── random5.pgm       # Obstáculos al 5%
│
├── teams/                # Directorio de equipos participantes
│   ├── team01/
│   │   └── main.c
│   ├── team02/
│   │   └── main.c
│   └── ...
│
├── results/              # Resultados de ejecuciones (generado)
│   ├── team01/
│   │   ├── map0_stats.csv
│   │   ├── map1_stats.csv
│   │   └── ...
│   └── ...
│
├── logs/                 # Logs de ejecución (generado)
└── lib/                  # Herramientas para distribuir a participantes
    └── myscore           # Ejecutable de autoevaluación
```

## Herramientas Principales

### Para Organizadores

| Herramienta | Propósito | Documentación |
|-------------|-----------|---------------|
| `runner` | Ejecuta todos los equipos en todos los mapas | ORGANIZER_GUIDE.md §4 |
| `score` | Genera ranking oficial con puntuaciones | ORGANIZER_GUIDE.md §5 |
| `validate` | Valida código antes de aceptar entregas | ORGANIZER_GUIDE.md §3 |
| `analyze_csv.py` | Analiza resultados históricos | ORGANIZER_GUIDE.md §6 |

### Para Participantes

| Herramienta | Propósito | Documentación |
|-------------|-----------|---------------|
| `myscore` | Autoevaluación de rendimiento local | STUDENT_GUIDE.md §2 |
| `validate` | Pre-validación antes de entregar | STUDENT_GUIDE.md §3 |

## Criterios de Puntuación

El sistema evalúa **4 métricas principales** (ver `RULES.md` para detalles):

1. **Cobertura (30%)**: Porcentaje de celdas visitadas
2. **Eficiencia de limpieza (35%)**: Suciedad limpiada por unidad de batería
3. **Conservación de batería (20%)**: Batería media mantenida
4. **Calidad de movimiento (15%)**: Ratio de movimientos exitosos sin colisiones

**Bonificaciones:**
- +5 puntos por completar el mapa
- +3 puntos por navegación limpia (<5 colisiones)

**Penalizaciones:**
- -10 puntos por cada crash del programa

**Detalles completos en `RULES.md`**

## Flujo de Evaluación

```
┌─────────────┐
│ Equipos     │
│ (main.c)    │
└──────┬──────┘
       │
       ▼
┌─────────────┐    ┌──────────────┐
│   runner    │───▶│ Ejecuta en   │
│             │    │ 4 mapas      │
└──────┬──────┘    └──────────────┘
       │
       ▼
┌─────────────┐
│ results/    │
│ stats.csv   │
└──────┬──────┘
       │
       ▼
┌─────────────┐    ┌──────────────┐
│   score     │───▶│ Calcula      │
│             │    │ puntuaciones │
└──────┬──────┘    └──────────────┘
       │
       ▼
┌─────────────┐
│ ranking.txt │
│ scores.csv  │
└─────────────┘
```

## Configuración

### `scoring.conf` - Configuración de Puntuación

```ini
# Pesos de métricas (deben sumar 100)
coverage_weight=30
dirt_efficiency_weight=35
battery_conservation_weight=20
movement_quality_weight=15

# Bonificaciones
completion_bonus=5
low_bumps_bonus=3
bumps_threshold=5

# Penalizaciones
crash_penalty=10
```

## Archivos Generados

### `stats.csv` (por equipo y mapa)
```
cell_total,cell_visited,dirt_total,dirt_cleaned,bat_total,bat_mean,...
2304,1543,230,198,1000,456.2,...
```

### `ranking.txt` (ranking oficial)
```
=== RANKING FINAL ===
Pos | Equipo  | Puntuación | Coverage | DirtEff | BatCons | Movement
  1 | team15  |      67.24 |    89.2% |   2.45% |   45.6% |    92.3%
  2 | team08  |      64.18 |    86.1% |   2.31% |   48.2% |    88.7%
...
```

### `scores.csv` (datos para análisis)
```csv
team,score
team15,67.24
team08,64.18
...
```

## Guías Específicas

### Soy Organizador
**Lee:** `ORGANIZER_GUIDE.md`

Aprenderás a:
- Configurar el sistema de competición
- Agregar y validar equipos
- Ejecutar evaluaciones masivas
- Interpretar y publicar resultados
- Resolver problemas comunes

### Soy Participante
**Lee:** `STUDENT_GUIDE.md`

Aprenderás a:
- Preparar tu código para entrega
- Usar `myscore` para autoevaluarte
- Interpretar el feedback
- Validar antes de entregar
- Estrategias de optimización

### Quiero Entender las Reglas
**Lee:** `RULES.md`

Entenderás:
- Cómo se calculan las puntuaciones
- Qué comportamientos dan más puntos
- Qué está permitido y prohibido
- Criterios de desempate

## Troubleshooting Rápido

### Problema: `runner` no ejecuta ningún equipo
```bash
# Verificar estructura de directorios
ls teams/*/main.c

# Verificar compilación
make all
```

### Problema: `score` no encuentra resultados
```bash
# Verificar que runner se ejecutó
ls results/*/map0_stats.csv

# Ejecutar runner primero
./runner
```

### Problema: Un equipo no aparece en ranking
```bash
# Verificar logs
cat logs/teamXX.stdout
cat logs/teamXX.stderr

# Validar código
./validate teams/teamXX
```

## Documentación Completa

- **Manual del Usuario** (participantes): `docs/usuario/MANUAL_USUARIO.md`
- **Manual del Organizador**: `docs/organizer/MANUAL_ORGANIZADOR.md`
- **Manual del Desarrollador**: `docs/developer/manual_desarrollador.tex`

## Arquitectura Técnica (Desarrolladores)

El sistema usa arquitectura modular:

1. **Simulador base** (`simula.c` y módulos): Sin modificaciones
2. **Extensiones de competición** (`competition_ext.c`): Lógica específica
3. **Biblioteca compartida** (`libscore`): Puntuación consistente
4. **Runner** (`runner.c`): Orquestador de la competición

**Ventajas:**
- Simulador base sin modificar
- Todos los equipos usan la misma versión
- Compilación rápida
- Extensible mediante `competition_ext.c`
- Puntuación consistente via `libscore`

## Contribuir

Para modificar o extender el sistema:
1. Consulta `docs/developer/manual_desarrollador.tex`
2. La arquitectura de `libscore` permite añadir métricas fácilmente
3. El formato de `stats.csv` es extensible

## Soporte

Para dudas:
- **Organizadores**: Consulta ORGANIZER_GUIDE.md §8 (Troubleshooting)
- **Participantes**: Consulta STUDENT_GUIDE.md §5 (FAQ)
- **Desarrolladores**: Consulta manual del desarrollador

---

**Sistema de competición IPR-GIIROB-ETSINF-UPV | Diciembre 2025**

**¡Éxito en tu competición Roomba!**
