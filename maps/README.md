# Generador de Mapas para Roomba

Este directorio contiene el generador de mapas de prueba para el simulador Roomba.

## Descripción

El generador crea automáticamente 8 mapas diferentes con diversas configuraciones:

- **noobs.pgm**: Sin obstáculos, solo suciedad
- **random1.pgm**: Obstáculos dispersos (densidad baja: 1%)
- **random3.pgm**: Obstáculos dispersos (densidad media: 3%)
- **random5.pgm**: Obstáculos dispersos (densidad alta: 5%)
- **walls1.pgm**: 1 muro (vertical u horizontal)
- **walls2.pgm**: 2 muros de la misma orientación
- **walls3.pgm**: 3 muros de la misma orientación
- **walls4.pgm**: 4 muros de la misma orientación

Todos los mapas incluyen:
- Base del robot en posición aleatoria en las paredes
- Orientación perpendicular a la pared
- 50 celdas con suciedad distribuidas aleatoriamente
- Garantía de que los muros no tocan los bordes

## Compilación

### Modo desarrollo (desde fuentes):
```bash
make mapgen-dev   # Generador de mapas
make viewmap-dev  # Visualizador de mapas
```

### Modo distribución (con simula.o):
```bash
make lib          # Genera simula.o
make mapgen       # Compila mapgen con simula.o
make viewmap      # Compila viewmap con simula.o
```

## Uso

### Generar mapas

```bash
./maps/generate
```

Los mapas se generarán en el directorio `maps/`.

### Visualizar mapas

```bash
./maps/viewmap maps/walls2.pgm
./maps/viewmap maps/random3.pgm
./maps/viewmap maps/noobs.pgm
```

El visualizador muestra el mapa en formato ASCII:
- `#` = Obstáculos/muros
- `B` = Base del robot
- `1`-`5` = Nivel de suciedad
- Espacios = Celdas vacías

## Probar los mapas generados

```bash
./roomba maps/walls2.pgm
./roomba maps/random3.pgm
```

## API

El generador utiliza la API limpia definida en `sim_world_api.h`:

- `map_create()` - Crea un nuevo mapa
- `map_generate()` - Genera un mapa aleatorio
- `map_save()` - Guarda el mapa en formato PGM
- `map_destroy()` - Libera la memoria del mapa

