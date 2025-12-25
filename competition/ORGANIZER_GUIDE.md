# Guía para Organizadores - Competición Roomba

Manual completo para organizadores sobre cómo configurar, ejecutar y gestionar la competición.

**Versión:** 1.0  
**Fecha:** Diciembre 2025

---

## Índice

1. [Introducción](#1-introducción)
2. [Instalación y Configuración](#2-instalación-y-configuración)
3. [Gestión de Equipos](#3-gestión-de-equipos)
4. [Ejecución de la Competición](#4-ejecución-de-la-competición)
5. [Sistema de Puntuación](#5-sistema-de-puntuación)
6. [Análisis de Resultados](#6-análisis-de-resultados)
7. [Distribución a Participantes](#7-distribución-a-participantes)
8. [Troubleshooting](#8-troubleshooting)

---

## 1. Introducción

### 1.1 Público Objetivo

Esta guía es para:

- Docentes coordinando la competición
- Administradores técnicos del sistema
- Personal de soporte durante la evaluación

### 1.2 Flujo Completo de Competición

```
1. Configuración inicial (esta guía, §2)
2. Preparación de mapas y configuración (§2.3)
3. Distribución de paquete a participantes (§7)
4. Recepción de entregas (§3.2)
5. Validación de código (§3.3)
6. Ejecución masiva con runner (§4)
7. Generación de ranking con score (§5)
8. Publicación de resultados (§6)
9. Análisis y feedback (§6)
```

### 1.3 Responsabilidades del Organizador

- Configurar el entorno de competición
- Crear y gestionar mapas oficiales
- Validar entregas de participantes
- Ejecutar evaluaciones automáticas
- Calcular y publicar rankings
- Resolver disputas y apelaciones
- Proporcionar feedback a participantes

---

## 2. Instalación y Configuración

### 2.1 Requisitos del Sistema

**Software necesario:**

- Sistema operativo: Linux o macOS
- Compilador: GCC 7.0 o superior
- Make: GNU Make 4.0+
- Python: 3.7+ (para `analyze_csv.py`)
- Git (recomendado para control de versiones)

**Recursos de hardware:**

- CPU: 2+ cores (recomendado 4+ para ejecuciones paralelas)
- RAM: 4 GB mínimo (8 GB recomendado)
- Disco: 500 MB libres
- Tiempo estimado de evaluación: 2-5 minutos por equipo (depende de CPU)

### 2.2 Compilación del Sistema

**Paso 1: Clonar/Descargar repositorio**

```bash
git clone <repository-url> roomba
cd roomba
```

**Paso 2: Compilar todo el sistema**

```bash
make tools      # Compila herramientas (validate, generate, etc.)
make arena      # Prepara entorno de competición (runner, score)
```

Esto genera:
- Herramientas en `tools/`
- Sistema de competición listo en `competition/`
- Mapas por defecto generados en `maps/`

### 2.3 Configuración de Mapas

El comando `make tools` ya genera 8 mapas de prueba en `maps/`.
Al ejecutar `make arena`, estos mapas se copian automáticamente a `competition/maps/`.

**Si deseas regenerar mapas con otra configuración:**

```bash
# Desde la raíz del proyecto
rm maps/*.pgm
./tools/generate
```

**Visualizar mapas:**

```bash
./tools/viewmap maps/noobs.pgm
```

### 2.4 Configuración de Puntuación

Editar `competition/scoring.conf`:

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

**Recomendaciones:**

- No modifiques los pesos después de anunciar la competición
- Los valores por defecto están balanceados
- Si cambias pesos, comunícalo con 1 semana de antelación

### 2.5 Prueba del Sistema

**Test completo:**

```bash
cd competition
make init   # Asegurar que el entorno está listo

# Compilar código de ejemplo (simulando un equipo)
cd teams/team01
gcc main.c ../lib/simula.o -lm -o roomba

# Ejecutar manualmente con un mapa
./roomba ../maps/noobs.pgm

# Verificar que genera stats.csv
ls -lh stats.csv

# Probar myscore
cd ../..
../tools/myscore teams/team01/stats.csv
```

Si todo funciona correctamente, el sistema está listo.

---

## 3. Gestión de Equipos

### 3.1 Estructura de Directorios

```bash
competition/teams/
├── team01/
│   └── main.c
├── team02/
│   └── main.c
├── team03/
│   └── main.c
└── ...
```

### 3.2 Recepción de Entregas

**Método 1: Entrega por email**

```bash
# Crear directorio para el equipo
mkdir -p competition/teams/teamXX

# Guardar main.c recibido
cp /path/to/received/main.c competition/teams/teamXX/
```

**Método 2: Entrega por plataforma (Moodle, etc.)**

Descargar archivo y colocar en estructura:

```bash
unzip entregas.zip
for team in team*; do
    mkdir -p competition/teams/$team
    cp $team/main.c competition/teams/$team/
done
```

**Método 3: Git (avanzado)**

```bash
# Si cada equipo tiene un repositorio
git clone team01-repo competition/teams/team01
```

### 3.3 Validación de Entregas

**Validar un equipo:**

```bash
cd competition
./validate teams/team01
```

**Salida esperada:**

```
Roomba Code Validator
===================================

Team: team01
========================================

Source file: main.c

Compilation: PASSED
Warnings: 2
Execution tests: 4/4 PASSED
Stats generation: PASSED

========================================
Summary: 5 passed, 2 warnings, 0 failed
========================================

Validation PASSED - Code is ready for competition!
```

**Validar todos los equipos:**

```bash
for team in teams/team*; do
    echo ""
    echo "=== Validating $(basename $team) ==="
    ../tools/validate $team
    echo ""
done > validation_report.txt
```

**Revisar validation_report.txt** para identificar problemas.

### 3.4 Problemas Comunes en Entregas

**Problema: Compilación fallida**

```bash
# Ver log de compilación
cat teams/teamXX/compile.log

# Errores típicos:
# - Falta incluir <math.h>
# - Sintaxis incorrecta
# - Uso de funciones no permitidas
```

**Solución:** Contactar al equipo para corrección.

**Problema: Archivos adicionales**

```bash
# Verificar contenido
ls -la teams/teamXX/

# Limpiar archivos no permitidos
rm teams/teamXX/*.o teams/teamXX/roomba teams/teamXX/*.pgm
```

**Problema: Timeout en ejecución**

```bash
# El código tarda más de 50 segundos
# Revisar lógica del equipo: posible bucle infinito
```

**Solución:** Pedir al equipo que optimice el código.

### 3.5 Gestión de Entregas Tardías

Dependiendo de las reglas de tu competición:

**Opción 1: No aceptar entregas tardías**

```bash
# Simplemente no incluir en teams/
```

**Opción 2: Aceptar con penalización**

```bash
# Incluir en teams/ pero anotar penalización manualmente
# Restar puntos al calcular ranking final
```

---

## 4. Ejecución de la Competición

### 4.1 Sistema Runner

`runner` es el orquestador que:

1. Descubre automáticamente equipos en `teams/`
2. Compila cada equipo con la biblioteca estándar
3. Ejecuta 20 veces cada equipo (4 mapas × 5 repeticiones)
4. Guarda resultados en `results/teamXX/mapX_stats.csv`
5. Genera logs en `logs/teamXX.stdout` y `logs/teamXX.stderr`

### 4.2 Ejecutar Competición Completa

**Comando básico:**

```bash
cd competition
make run
```

**Con timeout ajustable:**

```bash
timeout 3600 ./runner  # 1 hora máximo
```

**Ejecución en background:**

```bash
nohup ./runner > runner.log 2>&1 &

# Seguir progreso
tail -f runner.log
```

### 4.3 Monitoreo de Ejecución

Durante la ejecución, `runner` muestra:

```
=== ROOMBA COMPETITION RUNNER ===
Discovered 25 teams in teams/

Compiling teams...
[1/25] team01... OK
[2/25] team02... OK
[3/25] team03... FAILED (see logs/team03.stderr)
...

Executing competitions...
[team01] Map 0 Rep 1/5... OK (2.3s)
[team01] Map 0 Rep 2/5... OK (2.1s)
...
```

**Verificar progreso:**

```bash
# Número de archivos stats.csv generados
find results/ -name "*.csv" | wc -l

# Debería ser: número_de_equipos × 20
```

### 4.4 Logs de Ejecución

**Estructura de logs:**

```bash
competition/logs/
├── team01.stdout    # Salida estándar
├── team01.stderr    # Errores
├── team02.stdout
├── team02.stderr
└── ...
```

**Revisar problemas:**

```bash
# Equipos con crashes
grep -l "Segmentation fault" logs/*.stderr

# Equipos con timeouts
grep -l "TIMEOUT" logs/*.stderr

# Equipos con warnings
grep -l "warning:" logs/*.stderr
```

### 4.5 Resultados Parciales

**Durante la ejecución, puedes generar rankings parciales:**

```bash
# Mientras runner está ejecutando
./score  # Calcula con los resultados disponibles
```

**Ver equipos completados:**

```bash
ls results/
```

### 4.6 Reiniciar Ejecución

Si `runner` se interrumpe:

**Opción 1: Continuar desde donde se quedó**

`runner` detecta automáticamente resultados existentes y los salta.

```bash
./runner  # Continúa ejecución
```

**Opción 2: Empezar desde cero**

```bash
rm -rf results/* logs/*
./runner
```

**Opción 3: Re-ejecutar un equipo específico**

```bash
# Borrar resultados de ese equipo
rm -rf results/team05
rm logs/team05.*

# Ejecutar runner (saltará equipos ya completados)
./runner
```

---

## 5. Sistema de Puntuación

### 5.1 Biblioteca libscore

El cálculo de puntuaciones usa `libscore`, que implementa:

- Carga de configuración desde `scoring.conf`
- Cálculo de las 4 métricas principales
- Aplicación de bonificaciones y penalizaciones
- Agregación de resultados por equipo

**Ventajas:**

- Mismo algoritmo para `score` (oficial) y `myscore` (participantes)
- Código auditable y transparente
- Fácil de modificar para ajustar pesos

### 5.2 Generar Ranking Oficial

**Comando:**

```bash
cd competition
./score
```

**Salida:**

```
=== PROCESANDO RESULTADOS ===
Equipos encontrados: 25
Procesando team01... 20 ejecuciones
Procesando team02... 20 ejecuciones
...

=== CALCULANDO PUNTUACIONES ===
Aplicando pesos desde scoring.conf
Agregando por equipo...

=== RANKING GUARDADO ===
- ranking.txt (formato legible)
- scores.csv (formato datos)
```

### 5.3 Archivos Generados

**ranking.txt** - Ranking oficial legible:

```
=== RANKING FINAL ===
Competición Roomba IPR-GIIROB 2025

Pos | Equipo  | Puntuación | Coverage | DirtEff | BatCons | Movement
----+---------|------------|----------|---------|---------|----------
  1 | team15  |      67.24 |    89.2% |   2.45% |   45.6% |    92.3%
  2 | team08  |      64.18 |    86.1% |   2.31% |   48.2% |    88.7%
  3 | team22  |      61.45 |    94.5% |   1.82% |   38.9% |    85.2%
  4 | team03  |      59.87 |    82.3% |   2.67% |   42.1% |    88.4%
  5 | team19  |      58.12 |    79.8% |   2.54% |   46.8% |    86.9%
...

Configuración usada:
- coverage_weight: 30%
- dirt_efficiency_weight: 35%
- battery_conservation_weight: 20%
- movement_quality_weight: 15%
- completion_bonus: +5
- low_bumps_bonus: +3
- crash_penalty: -10
```

**scores.csv** - Datos para análisis:

```csv
team,score
team15,67.24
team08,64.18
team22,61.45
team03,59.87
team19,58.12
...
```

### 5.4 Criterios de Desempate

Si dos equipos tienen la misma puntuación (redondeada a 2 decimales):

1. Mayor cobertura promedio
2. Mayor eficiencia de limpieza promedio
3. Menor número de crashes totales
4. Menor número de colisiones totales
5. Mayor conservación de batería promedio

`score` aplica estos criterios automáticamente.

### 5.5 Verificar Puntuaciones

**Recalcular puntuación de un equipo específico:**

```bash
# Usar myscore con todos los archivos del equipo
cat results/team15/map*.csv > results/team15/all_stats.csv
./myscore results/team15/all_stats.csv
```

**Auditar cálculos:**

El código de `libscore.c` está disponible para revisión:

```bash
cat libscore.c | grep -A 20 "scoring_calculate_map"
```

---

## 6. Análisis de Resultados

### 6.1 Herramienta analyze_csv.py

Analiza resultados históricos y compara ediciones:

```bash
python3 analyze_csv.py results_2024.csv results_2025.csv
```

**Funcionalidades:**

- Comparación entre ediciones
- Análisis estadístico (media, mediana, desviación estándar)
- Detección de anomalías
- Gráficos de distribución (si tienes matplotlib)

### 6.2 Estadísticas por Mapa

**Analizar rendimiento por tipo de mapa:**

```bash
# Extraer resultados por mapa
grep ",0," results/*/map*.csv > map0_results.csv
grep ",1," results/*/map*.csv > map1_results.csv
grep ",2," results/*/map*.csv > map2_results.csv
grep ",3," results/*/map*.csv > map3_results.csv

# Calcular promedios
awk -F',' '{sum+=$4} END {print "Cobertura promedio mapa 0:", sum/NR"%"}' map0_results.csv
```

### 6.3 Identificar Anomalías

**Equipos con crashes frecuentes:**

```bash
grep -c "crash" logs/*.stderr | sort -t: -k2 -n
```

**Equipos con timeouts:**

```bash
grep -l "TIMEOUT" logs/*.stderr
```

**Equipos con puntuaciones inusualmente bajas:**

```bash
awk -F',' '$2 < 30 {print $1}' scores.csv
```

### 6.4 Generar Reportes

**Reporte completo para publicación:**

```bash
cat > competition_report.md << EOF
# Resultados Competición Roomba 2025

## Estadísticas Generales
- Equipos participantes: $(ls results/ | wc -l)
- Puntuación media: $(awk -F',' '{sum+=$2} END {print sum/NR}' results/scores.csv)
- Puntuación máxima: $(sort -t',' -k2 -rn results/scores.csv | head -1 | cut -d',' -f2)

## Top 10
$(head -11 ranking.txt | tail -10)

## Análisis por Métrica
...
EOF
```

### 6.5 Feedback a Participantes

**Generar feedback individual:**

```bash
# Script para generar feedback por equipo
for team in teams/team*; do
    team_name=$(basename $team)
    ./myscore $team/*.csv > feedback_$team_name.txt
done
```

**Enviar resultados:**

```bash
# Feedback individual por email
mail -s "Resultados Roomba - $team_name" email@example.com < feedback_$team_name.txt
```

---

## 7. Distribución a Participantes

### 7.1 Paquete de Distribución

Los participantes necesitan un paquete "Standalone" que incluye el simulador precompilado (`simula.o`), cabeceras, guia y herramienta de autoevaluación.

**Crear paquete automáticamente:**

```bash
# Desde la raíz del proyecto
make dist
```

Esto generará el directorio `dist/` con todo lo necesario y mostrará instrucciones para comprimirlo.

**Contenido del paquete generado:**
- `simula.h`: API del simulador
- `simula.o`: Biblioteca precompilada
- `tools/`: Herramientas (myscore, validate, etc.)
- `maps/`: Mapas de ejemplo
- `STANDALONE_GUIDE.md`: Guía completa para participantes
- `README.md`: Instrucciones de inicio rápido

**Para comprimir y distribuir:**

```bash
tar -czf roomba-standalone.tar.gz dist/
# o
zip -r roomba-standalone.zip dist/
```

**Distribuir:**

```bash
# Por plataforma online (Moodle, etc.)
# O por email
mail -s "Paquete Competición Roomba" -a roomba_competition_dist.tar.gz participantes@example.com < invitation.txt
```

### 7.2 Documentación para Participantes

Proporciona:

- `PARTICIPANT_GUIDE.md` - Guía completa
- `RULES.md` - Reglas oficiales
- Ejemplos de código en `samples/` (opcional)

### 7.3 Comunicación con Participantes

**Anuncio inicial:**

```
Asunto: Competición Roomba 2025 - Instrucciones

Estimados participantes,

Adjunto encontrarás el paquete de desarrollo para la competición.

Fecha límite de entrega: DD/MM/YYYY HH:MM
Formato de entrega: Un único archivo main.c

Documentación:
- PARTICIPANT_GUIDE.md: Guía completa
- RULES.md: Reglas de evaluación

Herramientas:
- myscore: Autoevaluación local

¡Éxito!
```

### 7.4 Preguntas Frecuentes

Prepara respuestas para:

- "¿Cómo compilo mi código?"
- "¿Qué significa el error X?"
- "¿Cómo sé si estoy mejorando?"
- "¿Puedo usar la librería Y?"
- "¿Cuándo se publican los resultados?"

---

## 8. Troubleshooting

### 8.1 Problemas de Compilación

**Error: `runner` no compila**

```bash
# Verificar dependencias
make clean
make all

# Ver errores específicos
gcc runner.c -o runner -Wall -Wextra
```

**Error: `libscore.o not found`**

```bash
cd competition
make libscore.o
```

### 8.2 Problemas de Ejecución

**Error: `runner` no encuentra equipos**

```bash
# Verificar estructura
ls teams/*/main.c

# Debe haber al menos un main.c
```

**Error: `runner` crashea**

```bash
# Ejecutar con debug
gdb ./runner
run

# Ver backtrace
bt
```

### 8.3 Problemas de Puntuación

**Error: `score` no genera ranking**

```bash
# Verificar que hay resultados
ls results/*/map*.csv

# Debe haber archivos stats.csv
```

**Error: Puntuaciones incorrectas**

```bash
# Verificar scoring.conf
cat scoring.conf

# Los pesos deben sumar 100
```

### 8.4 Problemas con Equipos Específicos

**Un equipo no aparece en ranking:**

```bash
# Verificar logs
cat logs/teamXX.stderr

# Verificar resultados
ls results/teamXX/

# Re-ejecutar ese equipo
rm -rf results/teamXX logs/teamXX.*
./runner
```

**Un equipo tiene puntuación 0:**

```bash
# Ver si crasheó
grep "crash\|TIMEOUT\|Segmentation" logs/teamXX.stderr

# Ver stats generados
cat results/teamXX/map0_stats.csv
```

### 8.5 Problemas de Rendimiento

**`runner` tarda demasiado:**

```bash
# Ejecutar en paralelo (si tienes múltiples cores)
# Modificar runner.c para usar fork() o GNU parallel

# O ejecutar por lotes
./runner 2>&1 | tee runner_batch1.log
```

**Disco lleno:**

```bash
# Limpiar logs antiguos
rm -rf logs/*.log

# Comprimir resultados
tar -czf results_backup.tar.gz results/
rm -rf results/
```

### 8.6 Problemas con Mapas

**Mapas no se encuentran:**

```bash
# Verificar ubicación
ls competition/maps/*.pgm

# Runner busca en maps/ relativo a su ubicación
```

**Mapas corruptos:**

```bash
# Verificar formato PGM
file maps/*.pgm

# Regenerar si es necesario
cd maps
./generate noobs.pgm 0
```

### 8.7 Resolución de Disputas

**Participante cuestiona su puntuación:**

1. Obtener archivos stats.csv del equipo
2. Ejecutar myscore manualmente
3. Verificar cálculos con libscore.c
4. Comparar con otros equipos similares
5. Si hay error, documentar y recalcular ranking

**Participante reporta bug en sistema:**

1. Reproducir el problema
2. Verificar logs
3. Corregir si es necesario
4. Re-ejecutar afectados
5. Comunicar solución

### 8.8 Backup y Recuperación

**Hacer backup antes de cada ejecución:**

```bash
# Backup de resultados
tar -czf backup_$(date +%Y%m%d_%H%M%S).tar.gz results/ logs/ ranking.txt scores.csv

# Backup de código de equipos
tar -czf teams_backup_$(date +%Y%m%d).tar.gz teams/
```

**Recuperar de backup:**

```bash
tar -xzf backup_20251206_143000.tar.gz
```

---

## 9. Mejores Prácticas

### 9.1 Antes de la Competición

- Probar el sistema completo con código de ejemplo
- Verificar que todos los mapas funcionan
- Preparar documentación para participantes
- Anunciar reglas con claridad
- Establecer fechas límite realistas

### 9.2 Durante la Competición

- Responder preguntas rápidamente
- Mantener comunicación abierta
- Documentar todos los problemas
- Hacer backups frecuentes
- Monitorear ejecución de runner

### 9.3 Después de la Competición

- Publicar resultados completos
- Proporcionar feedback individual
- Archivar todo (código, resultados, logs)
- Recopilar feedback de participantes
- Documentar mejoras para próxima edición

---

## 10. Contacto y Soporte Técnico

Para problemas no resueltos por esta guía:

- Consultar manual del desarrollador (`docs/developer/`)
- Revisar código fuente de herramientas
- Contactar con el equipo de desarrollo del sistema

---

## Apéndice A: Comandos Rápidos

```bash
# Compilar sistema
make all && cd competition && make all

# Validar equipo
./validate teams/teamXX

# Ejecutar competición
./runner

# Generar ranking
./score

# Ver top 10
head -13 ranking.txt | tail -10

# Limpiar todo
make clean && cd competition && make clean
rm -rf results/* logs/*
```

## Apéndice B: Estructura Completa

```
roomba/
├── competition/
│   ├── runner           # Orquestador
│   ├── score            # Ranking
│   ├── myscore          # Autoevaluación
│   ├── validate         # Validador
│   ├── libscore.c/h     # Biblioteca puntuación
│   ├── scoring.conf     # Configuración
│   ├── maps/            # Mapas oficiales
│   ├── teams/           # Código participantes
│   ├── results/         # Resultados (generado)
│   ├── logs/            # Logs (generado)
│   ├── ranking.txt      # Ranking final (generado)
│   └── scores.csv       # Datos ranking (generado)
├── simula.h             # API simulador
├── simula.o             # Biblioteca compilada
└── docs/                # Documentación
```

---

**Sistema de competición IPR-GIIROB-ETSINF-UPV | Diciembre 2025**
