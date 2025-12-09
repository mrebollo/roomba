# Roomba Tools

Herramientas de utilidad para el simulador Roomba. Independientes y reutilizables tanto para desarrollo, práctica de participantes, como para gestión de competiciones.

## Herramientas Disponibles


### 1. **generate** - Generador de Mapas

Genera automáticamente un lote de mapas de prueba en formato PGM con diferentes configuraciones de obstáculos y suciedad.

**Uso:**
```bash
# Compilar
make generate

# Generar mapas de prueba
./generate
```

**Características:**
- Genera 8 mapas de prueba en la carpeta `maps/` con diferentes densidades y configuraciones
- Base del robot en posición aleatoria en las paredes
- Suciedad distribuida aleatoriamente (niveles 1-5)
- Garantiza que los muros no toquen los bordes

---

### 2. **viewmap** - Visualizador de Mapas

Muestra mapas PGM en formato ASCII en la terminal.

**Uso:**
```bash
# Compilar
make viewmap

# Visualizar mapa
./viewmap maps/walls2.pgm
./viewmap maps/random3.pgm
```

**Leyenda:**
- `#` = Obstáculos/muros
- `B` = Base del robot
- `1`-`5` = Nivel de suciedad
- ` ` (espacio) = Celdas vacías

**Ideal para:**
- Verificar mapas generados antes de usarlos
- Inspección visual de configuraciones de mapas

---

### 3. **validate** - Validador de Código

Valida código de equipos antes de la competición. Detecta problemas de compilación, crashes, y generación de estadísticas.

**Uso:**
```bash
# Validación por defecto (directorio actual)
./validate

# Vallidación del código en el directorio myfolder
./validate myfolder

# Validación por organizador con informe
./validate ../competition/teams/team01 --output report.txt

# Modo estricto (fallo con warnings)
./validate ../competition/teams/team02 --strict
```

**Opciones:**
```
--maps <dir>      Path to maps directory (default: maps)
--output <file>   Save validation report to file
--strict          Fail on warnings
--timeout <sec>   Execution timeout per test (default: 100s)
```

**Validaciones realizadas:**
- Estructura del directorio correcta
- Compilación exitosa (con/sin warnings)
- Ejecución sin errores de ejecución en múltiples mapas
- Generación de `stats.csv` válido
- Límites de tiempo de ejecución
- Reporte detallado con métricas
 

**Casos de uso:**
1. **Participante:** Verificar código antes de enviar
   ```bash
   cd mi_roomba/
   ./tools/validate.s . --output mi_validacion.txt
   ```

2. **Organizador:** Validar entregas
   ```bash
   for team in teams/*/; do
       ./validate "$team" --output "reports/$(basename $team).txt"
   done
   ```

3. **Competición:** Pre-validación de un equipo
   ```bash
   .validate teams/team15 --strict --timeout 5
   ```

---

### 4. **myscore** - Cálculo del rendimiento

Herramienta para analizar el fichero `stats.csv` generado por el simulador y calcular métricas de rendimiento del robot.

**Uso básico:**
```bash
./myscore stats.csv
```

**Recomendaciones para acumulación de resultados:**
- Por defecto, el simulador sobrescribe `stats.csv` en cada ejecución. Para analizar varias ejecuciones, es necesario guardar cada resultado con un nombre diferente:
   ```bash
   ./roomba > stats1.csv
   ./roomba > stats2.csv
   ./roomba > stats3.csv
   # ...
   ```

**Cómo combinar varios ficheros de resultados para analizarlos juntos:**
Supongamos que tienes varios ficheros `stats1.csv`, `stats2.csv`, `stats3.csv`, etc. Puedes combinarlos en uno solo para analizarlos con `myscore`:
```bash
# Copia la cabecera del primer fichero
head -n 1 stats1.csv > stats.csv
# Añade los datos de todos los ficheros (sin repetir la cabecera)
for f in stats*.csv; do tail -n +2 "$f" >> stats.csv; done
# Analiza el fichero combinado
./myscore stats.csv
```

**Notas:**
- Es importante no repetir la cabecera (línea 1) al combinar los ficheros.
- Puedes cambiar el nombre de salida en cada ejecución para evitar sobrescribir resultados.
- El análisis es más útil cuando se acumulan resultados de diferentes mapas o ejecuciones.

**Ejemplo de uso completo:**
```bash
for i in {1..5}; do ./roomba > "stats${i}.csv"; done
head -n 1 stats1.csv > stats.csv
for f in stats*.csv; do tail -n +2 "$f" >> stats.csv; done
./myscore stats.csv
```

Esta herramienta permite comparar el rendimiento de diferentes algoritmos, configuraciones o mapas de manera sencilla y automatizada.




---

## Compilación

### Compilar todas las herramientas
```bash
cd tools/
make
```

### Compilar herramientas individuales
```bash
make generate
make viewmap
make validate
```

### Limpiar
```bash
make clean
```

---

## Casos de Uso Comunes

### Para Participantes

**1. Validar mi código antes de entregar:**
```bash
cd mi_proyecto/
./tools/validate . --output validacion.txt
cat validacion.txt  # Revisar resultados
```

**2. Crear mapas de prueba:**
```bash
cd tools/
./generate
./viewmap ../maps/noobs.pgm
./viewmap ../maps/random1.pgm
```

**3. Probar con mi mapa:**
```bash
cd mi_proyecto/
cp ../maps/mi_mapa.pgm map.pgm
./roomba
```

---

### Para Organizadores

**1. Validar todas las entregas:**
```bash
mkdir -p reports/
for team in competition/teams/team*/; do
    team_name=$(basename "$team")
    echo "Validating $team_name..."
    ./tools/validate.sh "$team" --strict --output "reports/${team_name}.txt"
done
```

**2. Generar mapas de competición (usar los mapas generados por defecto):**
```bash
cd tools/
./generate
# Verificar visualmente
./viewmap ../maps/noobs.pgm
./viewmap ../maps/random1.pgm
```

**3. Validación rápida de un equipo específico:**
```bash
./tools/validate.sh competition/teams/team07
```

---

## Estructura de Directorios

```
roomba/
├── tools/              # Este directorio
│   ├── generate        # Binario compilado
│   ├── viewmap        # Binario compilado
│   ├── validate.      # Binario compilado
│   ├── generate.c     # Fuente del generador
│   ├── viewmap.c      # Fuente del visualizador
│   ├── validata.c.    # Fuente del validador 
│   ├── Makefile       # Construye el sistema
│   └── README.md      # Esta documentación
└── maps/              # Mapas PGM (solo datos)
    ├── map0.pgm
    ├── map1.pgm
    └── ...
 ```

---

## Requisitos

- **gcc** con soporte para C99
- **make** para compilación
- **bash** para scripts de validación
- **timeout** command (incluido en GNU coreutils)

---

## Referencias

- Ver `../samples/` para ejemplos de uso del simulador
- Ver `../competition/` para sistema de competición completo
- Ver `../docs/` para documentación técnica detallada

---

## Tips

- **Para participantes:** Ejecuta `./tools/validate .` frecuentemente durante desarrollo
- **Para organizadores:** Usa `--strict` para forzar compilación sin warnings
- **Para competiciones:** Combina con `competition/runner` para ejecución completa
- **Debugging:** Usa `viewmap` para inspeccionar mapas problemáticos rápidamente
