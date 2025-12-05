# Competition Maps

Mapas oficiales para la competición de Roombas.

## Mapas disponibles

Los mapas se rotan automáticamente durante la competición (4 mapas × 5 repeticiones = 20 rondas por equipo).

### Configuración actual

El sistema está configurado para usar 4 tipos de mapas diferentes. Estos se especifican en `config.txt` de cada equipo mediante el campo `map_file` (valores 0-3).

## Generación de mapas

Para generar nuevos mapas oficiales:

```bash
# Desde el directorio raíz del proyecto
cd maps
make  # O: gcc generate.c -o generate -lm
./generate

# Esto genera 8 mapas diferentes:
# - noobs.pgm (sin obstáculos)
# - random1.pgm, random3.pgm, random5.pgm (obstáculos dispersos)
# - walls1.pgm, walls2.pgm, walls3.pgm, walls4.pgm (muros)
```

## Seleccionar mapas para competición

Copia los mapas deseados a este directorio:

```bash
# Ejemplo: Usar 4 mapas con diferentes dificultades
cp ../maps/noobs.pgm map0.pgm
cp ../maps/random3.pgm map1.pgm
cp ../maps/walls2.pgm map2.pgm
cp ../maps/walls4.pgm map3.pgm
```

## Formato de mapas

Los mapas usan formato PGM (Portable Gray Map):

```
P2
50 50
255
# ... valores de píxeles ...
```

**Valores especiales:**
- `255` = Pared
- `0` = Espacio vacío
- `1-254` = Cantidad de suciedad en la celda

## Visualizar mapas

Para ver un mapa antes de usarlo:

```bash
# Convertir a PNG (requiere ImageMagick)
convert map0.pgm map0.png
open map0.png

# O usar el visor del proyecto
cd ../maps
make viewmap
./viewmap ../competition/maps/map0.pgm
```

## Configuración en competition_ext.h

El número de mapas debe coincidir con la definición:

```c
#define COMP_MAPS_COUNT 4  // Número de mapas en este directorio
```

Si cambias el número de mapas, actualiza esta constante en `competition_ext.h`.

## Validación de mapas

Para asegurar que un mapa es válido:

1. **Debe tener base del robot** (marcada con 'B' o 'o')
2. **Debe tener bordes de paredes** completos
3. **Debe tener suciedad** (celdas con valores 1-254)
4. **Tamaño recomendado**: 50×50 celdas

## Mapas personalizados

Puedes crear mapas manualmente editando archivos PGM o usando el generador:

```c
// Ver maps/generate.c para el código de generación
// Personaliza densidad de obstáculos, cantidad de suciedad, etc.
```

---

**Nota**: Los equipos NO deben tener acceso a estos mapas antes de la competición para evitar sobre-optimización.
