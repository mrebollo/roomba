# Competition Results

Este directorio contiene los resultados de las ejecuciones de la competición, organizados por timestamp.

## Estructura

Cada ejecución genera un directorio con formato `run_YYYYMMDD_HHMMSS`:

```
results/
├── run_20251205_143000/
│   ├── stats.csv         # Estadísticas detalladas por equipo/mapa
│   └── ranking.txt       # Clasificación final
├── run_20251205_150000/
│   ├── stats.csv
│   └── ranking.txt
└── latest -> run_20251205_150000/  # Symlink a la última ejecución
```

## Uso

### Ejecutar con archivado automático

```bash
cd competition
make run-archived
```

Esto ejecuta la competición y mueve automáticamente `stats.csv` y `ranking.txt` a un directorio timestamped.

### Ver últimos resultados

```bash
# Ver ranking
cat results/latest/ranking.txt

# Ver estadísticas
head results/latest/stats.csv

# O usar el Makefile
make ranking
make stats
```

## Formato de datos

### stats.csv
Cada línea representa una ejecución de un equipo en un mapa específico:

```csv
team,map_type,cell_total,cell_visited,dirt_total,dirt_cleaned,bat_total,bat_mean,forward,turn,bumps,clean,load
team01,0,2400,1523,127,95,100.0,67.3,1245,89,23,95,1
team01,1,2400,1687,134,121,100.0,52.1,1534,102,18,121,2
```

### ranking.txt
Clasificación ordenada por puntuación:

```
ROOMBA COMPETITION RANKING

Rank  Team            Score    Cells    Dirt    Battery
-----------------------------------------------------------
1     team07          8734     2145     189     32.5
2     team13          8521     2087     201     35.2
```

## Análisis

Para análisis estadístico avanzado, considera usar:
- Python + pandas para procesar CSV
- matplotlib/seaborn para visualización
- Jupyter notebooks para análisis interactivo

Ejemplo rápido:
```python
import pandas as pd
df = pd.read_csv('results/latest/stats.csv')
print(df.groupby('team')['dirt_cleaned'].mean().sort_values(ascending=False))
```

## Limpieza

Para limpiar resultados antiguos:

```bash
# Mantener solo últimas 10 ejecuciones
cd results
ls -t | tail -n +11 | xargs rm -rf

# O limpiar todo
cd competition
make clean-all
```

## Backup

Se recomienda hacer backup periódico de los resultados:

```bash
tar -czf competition_results_$(date +%Y%m%d).tar.gz results/
```
