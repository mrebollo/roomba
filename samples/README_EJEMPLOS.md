# Ejemplos del Manual de Usuario

Esta carpeta contiene los ejemplos prácticos del **Manual de Usuario** del Simulador Roomba. Cada archivo corresponde a un ejemplo específico explicado en el manual.

Se asume que el simulador se encuentra precompilado en un fichero simula.o para incluirlo con cada ejemplo sin necesidad de recompilar los fuentes completos cada vez.

## Lista de Ejemplos

### Ejemplo 1: Robot Inmóvil (`ejemplo1_inmovil.c`)
El programa más simple para entender la estructura básica. El robot solo despierta y se queda quieto.

**Conceptos:**
- Estructura básica de un programa
- `rmb_awake()` - Activación del robot
- `configure()` y `run()` - Configuración y ejecución
- `visualize()` - Visualización del resultado

**Compilar y ejecutar:**
```bash
gcc ejemplo1_inmovil.c simula.o -lm -o ejemplo1
./ejemplo1
```

---

### Ejemplo 2: Robot que Avanza (`ejemplo2_avanza.c`)
Robot que avanza en línea recta hasta chocar con una pared.

**Conceptos:**
- `rmb_forward()` - Movimiento básico
- Comportamiento en bucle

**Compilar y ejecutar:**
```bash
gcc ejemplo2_avanza.c simula.o -lm -o ejemplo2
./ejemplo2
```

---

### Ejemplo 3: Robot que Explora en Cuadrado (`ejemplo3_cuadrado.c`)
Robot que dibuja un patrón cuadrado usando giros de 90 grados.

**Conceptos:**
- `rmb_turn()` - Rotación del robot
- Uso de `M_PI` para ángulos
- Variables globales para mantener estado
- Control de pasos

**Compilar y ejecutar:**
```bash
gcc ejemplo3_cuadrado.c simula.o -lm -o ejemplo3
./ejemplo3
```

**Ejercicio:** Modifica el programa para que haga un triángulo (giros de 120°).

---

### Ejemplo 4: Robot que Rebota (`ejemplo4_rebote.c`)
Comportamiento reactivo: cuando detecta un obstáculo, gira aleatoriamente y continúa.

**Conceptos:**
- `rmb_bumper()` - Detección de colisiones
- Generación de números aleatorios con `rand()`
- Giros aleatorios para exploración

**Compilar y ejecutar:**
```bash
gcc ejemplo4_rebote.c simula.o -lm -o ejemplo4
./ejemplo4
```

---

### Ejemplo 5: Robot que Sigue Paredes (`ejemplo5_sigue_paredes.c`)
Estrategia más inteligente usando máquina de estados para seguir las paredes del entorno.

**Conceptos:**
- Máquina de estados (`enum`)
- Navegación guiada por paredes
- Estados: AVANZANDO, GIRANDO, AJUSTANDO

**Compilar y ejecutar:**
```bash
gcc ejemplo5_sigue_paredes.c simula.o -lm -o ejemplo5
./ejemplo5
```

---

### Ejemplo 6: Robot que Limpia al Detectar Suciedad (`ejemplo6_limpieza_basica.c`)
Robot que detecta suciedad con el sensor infrarrojo y limpia cuando la encuentra.

**Conceptos:**
- `rmb_ifr()` - Sensor de suciedad
- `rmb_clean()` - Limpieza básica
- Priorización de tareas (limpiar antes que moverse)

**Compilar y ejecutar:**
```bash
gcc ejemplo6_limpieza_basica.c simula.o -lm -o ejemplo6
./ejemplo6
```

---

### Ejemplo 7: Limpieza Completa de una Celda (`ejemplo7_limpieza_completa.c`)
Robot que limpia completamente cada celda sucia (repitiendo `rmb_clean()` hasta nivel 0).

**Conceptos:**
- Bucle `while` para limpieza completa
- Gestión de niveles de suciedad (1-5)
- Consumo de batería en limpieza

**Compilar y ejecutar:**
```bash
gcc ejemplo7_limpieza_completa.c simula.o -lm -o ejemplo7
./ejemplo7
```

---

### Ejemplo 8: Seguimiento de Estadísticas (`ejemplo8_estadisticas.c`)
Usa variables `static` para mantener estadísticas durante la ejecución y las muestra al finalizar.

**Conceptos:**
- Variables `static` vs globales
- Función `finalizar()` para mostrar resultados
- Seguimiento de métricas (pasos, colisiones, limpiezas)

**Compilar y ejecutar:**
```bash
gcc ejemplo8_estadisticas.c simula.o -lm -o ejemplo8
./ejemplo8
```

---

### Ejemplo 9: Robot Autónomo Completo (`ejemplo9_autonomo.c`)
Implementación completa de un robot autónomo con exploración, limpieza, regreso a base y recarga.

**Conceptos:**
- Máquina de estados compleja (4 estados)
- Gestión de batería
- `rmb_load()` - Recarga en base
- `rmb_state()` - Lectura completa de sensores
- Navegación dirigida con `atan2()`
- Algoritmo de regreso a punto conocido

**Compilar y ejecutar:**
```bash
gcc ejemplo9_autonomo.c simula.o -lm -o ejemplo9
./ejemplo9
```

---

## Orden de Aprendizaje Recomendado

1. **Ejemplo 1** - Entender la estructura básica
2. **Ejemplo 2** - Movimiento simple
3. **Ejemplo 3** - Giros y control de pasos
4. **Ejemplo 4** - Detección de obstáculos
5. **Ejemplo 6** - Limpieza básica
6. **Ejemplo 7** - Limpieza completa
7. **Ejemplo 5** - Máquina de estados
8. **Ejemplo 8** - Estadísticas
9. **Ejemplo 9** - Robot completo (proyecto final)

---

## Modificaciones Sugeridas

### Para Ejemplo 3 (Cuadrado)
- Cambiar a triángulo (giros de 120°)
- Hacer un hexágono (giros de 60°)
- Variar el tamaño de los lados

### Para Ejemplo 4 (Rebote)
- Cambiar el rango de ángulos aleatorios
- Añadir limpieza cuando detecta suciedad
- Implementar preferencia por una dirección

### Para Ejemplo 5 (Sigue Paredes)
- Cambiar la distancia de ajuste
- Alternar entre seguir pared derecha/izquierda
- Añadir limpieza durante el seguimiento

### Para Ejemplo 9 (Autónomo)
- Cambiar el umbral de batería para regresar
- Modificar la estrategia de exploración
- Optimizar el algoritmo de regreso a base
- Añadir memoria de celdas visitadas

---

## Archivos Relacionados

- **Manual de Usuario:** `docs/usuario/manual_usuario.pdf`
- **Manual de Usuario (LaTeX):** `docs/usuario/manual_usuario.tex`
- **Manual del Desarrollador:** `docs/developer/manual_desarrollador.pdf`
- **API del Simulador:** `simula.h`
- **Código fuente:** `simula.c`, `sim_robot.c`, etc.

---

## Contacto

Para dudas o sugerencias sobre los ejemplos, consulta el manual de usuario completo en `docs/usuario/`.
