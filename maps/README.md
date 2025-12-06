# Mapas de Prueba para Roomba

Este directorio contiene mapas de prueba en formato PGM para el simulador Roomba.

## 📁 Mapas Disponibles

### Mapas sin obstáculos
- **noobs.pgm**: Sin obstáculos, solo suciedad - Ideal para principiantes

### Mapas con obstáculos aleatorios
- **random1.pgm**: Obstáculos dispersos (densidad baja: 1%)
- **random3.pgm**: Obstáculos dispersos (densidad media: 3%)
- **random5.pgm**: Obstáculos dispersos (densidad alta: 5%)

### Mapas con muros
- **walls1.pgm**: 1 muro (vertical u horizontal)
- **walls2.pgm**: 2 muros de la misma orientación
- **walls3.pgm**: 3 muros de la misma orientación
- **walls4.pgm**: 4 muros de la misma orientación

## 📝 Características de los mapas

Todos los mapas incluyen:
- Base del robot en posición aleatoria en las paredes
- Orientación perpendicular a la pared
- Suciedad distribuida aleatoriamente (niveles 1-5)
- Garantía de que los muros no tocan los bordes

## 🛠️ Generar Mapas Personalizados

Para crear tus propios mapas, usa las herramientas en `../tools/`:

```bash
cd ../tools
make generate_map

# Crear mapa personalizado
./generate_map <filas> <columnas> <obstáculos%> <suciedad%> <salida.pgm>

# Ejemplo
./generate_map 60 60 10 50 ../maps/mi_mapa.pgm
```

Ver documentación completa en `../tools/README.md`

## 👁️ Visualizar Mapas

Para visualizar mapas en la terminal:

```bash
cd ../tools
make viewmap
./viewmap ../maps/walls2.pgm
```

Leyenda:
- `#` = Obstáculos/muros
- `B` = Base del robot
- `1`-`5` = Nivel de suciedad
- ` ` = Celdas vacías

## 🎮 Usar Mapas en el Simulador

```bash
# Copiar mapa al directorio de trabajo
cp maps/walls2.pgm .

# O especificar ruta (si el simulador lo permite)
./roomba maps/random3.pgm
```

## 📊 Formato PGM

Los mapas usan formato PGM (Portable Gray Map):
- Valores 0-255 representan diferentes elementos
- Formato de texto simple, fácil de generar/editar
- Compatible con herramientas estándar de procesamiento de imágenes

## 🔗 Referencias

- Generador de mapas: `../tools/generate_map`
- Visualizador: `../tools/viewmap`
- Documentación técnica: `../docs/`


