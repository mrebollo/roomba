# Competition Library

Esta biblioteca contiene la versión compilada del simulador Roomba para competición.

## Archivos

- **simula.o** - Biblioteca precompilada con todos los módulos del simulador (21KB)
- **simula.h** - API pública del simulador (copia sincronizada desde raíz)
- **simula_internal.h** - Estructuras internas (copia sincronizada desde raíz)

## Versión

Generada desde el proyecto principal mediante:
```bash
cd ..  # Directorio raíz del proyecto
make lib-competition
```

Este comando compila todos los módulos con optimización `-O2` y los combina en un único objeto relocatable usando `gcc -r -nostdlib`.

## Módulos incluidos

El archivo `simula.o` contiene:
- `simula.c` - Core del simulador
- `sim_robot.c` - Control del robot
- `sim_visual.c` - Visualización
- `sim_io.c` - Entrada/salida (mapas, stats, logs)
- `sim_world.c` - Generación de mundos
- `sim_stats.c` - Estadísticas

## Sincronización

**IMPORTANTE**: Esta biblioteca debe regenerarse cada vez que se modifique el código fuente del simulador en el directorio raíz:

```bash
# Después de modificar simula.c, sim_*.c, etc.
cd /path/to/roomba
make lib-competition

# Los headers también deben copiarse si cambian:
cp simula.h competition/lib/
cp simula_internal.h competition/lib/
```

## Uso en compilación de equipos

Los equipos se compilan enlazando con esta biblioteca:

```bash
cd teams/team01
gcc -I../../lib main.c ../../lib/simula.o \
    ../../competition_ext.c ../../simula_comp.c -lm -o roomba
```

El runner (`runner.c`) gestiona automáticamente estas compilaciones.

## Verificación

Para verificar que la biblioteca está actualizada:

```bash
# Fecha de modificación
ls -lh simula.o

# Tamaño (debe ser ~21KB)
du -h simula.o

# Símbolos exportados
nm simula.o | grep " T " | head -10
```

## Depuración

Si hay problemas de compilación o enlazado:

1. **Regenerar biblioteca**: `make lib-competition` desde raíz
2. **Verificar headers**: Comparar con versiones en raíz
3. **Limpiar y recompilar**: `rm simula.o && make lib-competition`

---

Última actualización: Diciembre 2025
