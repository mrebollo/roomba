# Roomba Tools

Herramientas de utilidad para el simulador Roomba. Independientes y reutilizables tanto para desarrollo, práctica de alumnos, como para gestión de competiciones.

## 🛠️ Herramientas Disponibles

### 1. **generate_map** - Generador de Mapas

Crea mapas de prueba en formato PGM con diferentes configuraciones de obstáculos y suciedad.

**Uso:**
```bash
# Compilar
make generate_map

# Generar mapa personalizado
./generate_map <filas> <columnas> <pared%> <suciedad%> <salida.pgm>

# Ejemplos
./generate_map 50 50 10 50 ../maps/test1.pgm
./generate_map 80 80 5 30 ../maps/test2.pgm
```

**Parámetros:**
- `filas`: Altura del mapa (recomendado: 50-100)
- `columnas`: Ancho del mapa (recomendado: 50-100)
- `pared%`: Porcentaje de obstáculos (0-20, recomendado: 5-15)
- `suciedad%`: Porcentaje de celdas con suciedad (0-100, recomendado: 30-70)
- `salida.pgm`: Nombre del archivo de salida

**Características:**
- Base del robot en posición aleatoria en las paredes
- Orientación perpendicular a la pared
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
./viewmap ../maps/walls2.pgm
./viewmap ../maps/random3.pgm
```

**Leyenda:**
- `#` = Obstáculos/muros
- `B` = Base del robot
- `1`-`5` = Nivel de suciedad
- ` ` (espacio) = Celdas vacías

**Ideal para:**
- Verificar mapas generados antes de usarlos
- Debugging rápido sin ejecutar el simulador completo
- Inspección visual de configuraciones de mapas

---

### 3. **validate.sh** - Validador de Código

Valida código de equipos antes de la competición. Detecta problemas de compilación, crashes, y generación de estadísticas.

**Uso:**
```bash
# Auto-validación para estudiantes
cd mi_proyecto/
../tools/validate.sh .

# Validación por profesor con reporte
./validate.sh ../competition/teams/team01 --output report.txt

# Modo estricto (falla con warnings)
./validate.sh ../competition/teams/team02 --strict
```

**Opciones:**
```
--maps <dir>      Path to maps directory (default: ../maps)
--lib <file>      Path to simula library (default: ../simula.o)
--output <file>   Save validation report to file
--strict          Fail on warnings
--timeout <sec>   Execution timeout per test (default: 10s)
```

**Validaciones realizadas:**
- ✅ Estructura del directorio correcta
- ✅ Compilación exitosa (con/sin warnings)
- ✅ Ejecución sin crashes en múltiples mapas
- ✅ Generación de `stats.csv` válido
- ✅ Límites de tiempo de ejecución
- 📊 Reporte detallado con métricas

**Casos de uso:**
1. **Estudiante:** Verificar código antes de enviar
   ```bash
   cd mi_roomba/
   ../tools/validate.sh . --output mi_validacion.txt
   ```

2. **Profesor:** Validar entregas
   ```bash
   for team in teams/*/; do
       ./tools/validate.sh "$team" --output "reports/$(basename $team).txt"
   done
   ```

3. **Competición:** Pre-validación automática
   ```bash
   ./tools/validate.sh teams/team15 --strict --timeout 5
   ```

---

## 📦 Compilación

### Compilar todas las herramientas
```bash
cd tools/
make
```

### Compilar herramientas individuales
```bash
make generate_map
make viewmap
make validate    # Solo hace el script ejecutable
```

### Limpiar
```bash
make clean
```

---

## 🎯 Casos de Uso Comunes

### Para Estudiantes

**1. Validar mi código antes de entregar:**
```bash
cd mi_proyecto/
../tools/validate.sh . --output validacion.txt
cat validacion.txt  # Revisar resultados
```

**2. Crear un mapa de prueba:**
```bash
cd tools/
./generate_map 60 60 8 40 ../maps/mi_mapa.pgm
./viewmap ../maps/mi_mapa.pgm
```

**3. Probar con mi mapa:**
```bash
cd mi_proyecto/
cp ../maps/mi_mapa.pgm map.pgm
./roomba
```

---

### Para Profesores

**1. Validar todas las entregas:**
```bash
mkdir -p reports/
for team in competition/teams/team*/; do
    team_name=$(basename "$team")
    echo "Validating $team_name..."
    ./tools/validate.sh "$team" --strict --output "reports/${team_name}.txt"
done
```

**2. Generar mapas de competición:**
```bash
cd tools/
./generate_map 80 80 12 50 ../maps/comp_map1.pgm
./generate_map 70 90 8 60 ../maps/comp_map2.pgm
./generate_map 100 60 15 40 ../maps/comp_map3.pgm

# Verificar visualmente
./viewmap ../maps/comp_map1.pgm
```

**3. Validación rápida de un equipo específico:**
```bash
./tools/validate.sh competition/teams/team07
```

---

## 📝 Estructura de Directorios

```
roomba/
├── tools/              # Este directorio
│   ├── generate_map    # Binario compilado
│   ├── viewmap        # Binario compilado
│   ├── validate.sh    # Script de validación
│   ├── generate.c     # Fuente del generador
│   ├── viewmap.c      # Fuente del visualizador
│   ├── Makefile       # Build system
│   └── README.md      # Esta documentación
├── maps/              # Mapas PGM (solo datos)
│   ├── map0.pgm
│   ├── map1.pgm
│   └── ...
└── competition/       # Sistema de competición
    └── scripts/
        └── validate_all.sh  # Wrapper para validación masiva
```

---

## 🔧 Requisitos

- **gcc** con soporte para C99
- **make** para compilación
- **bash** para scripts de validación
- **timeout** command (incluido en GNU coreutils)

---

## 📚 Referencias

- Ver `../samples/` para ejemplos de uso del simulador
- Ver `../competition/` para sistema de competición completo
- Ver `../docs/` para documentación técnica detallada

---

## ⚡ Tips

- **Para alumnos:** Ejecuta `./validate.sh .` frecuentemente durante desarrollo
- **Para profesores:** Usa `--strict` para forzar compilación sin warnings
- **Para competiciones:** Combina con `competition/runner` para ejecución completa
- **Debugging:** Usa `viewmap` para inspeccionar mapas problemáticos rápidamente
