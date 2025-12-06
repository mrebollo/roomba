# Ejercicios Propuestos del Manual de Usuario

Esta carpeta contiene los **ejercicios propuestos** del Manual de Usuario del Simulador Roomba. Cada archivo es una plantilla con el esqueleto básico y comentarios guía para que implementes tu propia solución.

Se asume que el simulador se encuentra precompilado en un fichero `simula.o` para incluirlo con cada ejercicio sin necesidad de recompilar los fuentes completos cada vez.

## Estructura de los Ejercicios

Cada ejercicio tiene:
- **Descripción del objetivo** en los comentarios del encabezado
- **Nivel de dificultad** (Básico, Intermedio, Avanzado)
- **Pistas y sugerencias** para guiar tu implementación
- **Esqueleto de código** con las funciones principales
- **Sección TODO** donde debes implementar tu solución

## Lista de Ejercicios

### Nivel Básico

#### Ejercicio 1: Cuadrado Perfecto (`ejercicio1_cuadrado_perfecto.c`)
**Objetivo:** Haz que el robot dibuje un cuadrado de 10×10 celdas sin salirse del trazado.

**Conceptos a practicar:**
- Control preciso de pasos
- Giros de 90 grados
- Variables de estado (contador de pasos, lados)

**Compilar y ejecutar:**
```bash
gcc ejercicio1_cuadrado_perfecto.c simula.o -lm -o ejercicio1
./ejercicio1
```

**Criterio de éxito:** El robot debe dibujar un cuadrado perfecto de exactamente 10 celdas por lado.

---

#### Ejercicio 2: Limpiador Simple (`ejercicio2_limpiador_simple.c`)
**Objetivo:** Limpia al menos el 50% de la suciedad del mapa.

**Conceptos a practicar:**
- Detección de suciedad con `rmb_ifr()`
- Limpieza con `rmb_clean()`
- Exploración eficiente

**Compilar y ejecutar:**
```bash
gcc ejercicio2_limpiador_simple.c simula.o -lm -o ejercicio2
./ejercicio2
```

**Criterio de éxito:** El archivo `stats.csv` debe mostrar `dirt_cleaned` ≥ 50% de `dirt_total`.

---

#### Ejercicio 3: Explorador (`ejercicio3_explorador.c`)
**Objetivo:** Visita al menos el 30% de las celdas del mapa.

**Conceptos a practicar:**
- Estrategias de exploración
- Rebote aleatorio o seguimiento de paredes
- Maximizar cobertura

**Compilar y ejecutar:**
```bash
gcc ejercicio3_explorador.c simula.o -lm -o ejercicio3
./ejercicio3
```

**Criterio de éxito:** El archivo `stats.csv` debe mostrar `cell_visited` ≥ 30% de `cell_total`.

---

### Nivel Intermedio

#### Ejercicio 4: Gestor de Batería (`ejercicio4_gestor_bateria.c`)
**Objetivo:** Implementa recarga automática cuando la batería caiga por debajo del 20%.

**Conceptos a practicar:**
- Monitoreo de batería con `rmb_battery()`
- Navegación dirigida con `atan2()`
- Estados: EXPLORANDO, REGRESANDO, RECARGANDO
- Uso de `rmb_load()` en la base

**Compilar y ejecutar:**
```bash
gcc ejercicio4_gestor_bateria.c simula.o -lm -o ejercicio4
./ejercicio4
```

**Criterio de éxito:** El robot nunca debe llegar a batería = 0. Debe recargar automáticamente.

---

#### Ejercicio 5: Espiral Completa (`ejercicio5_espiral.c`)
**Objetivo:** Cubre el área en espiral sin repetir celdas.

**Conceptos a practicar:**
- Patrón de espiral (1, 1, 2, 2, 3, 3, 4, 4...)
- Control incremental de distancias
- Manejo de colisiones con paredes

**Compilar y ejecutar:**
```bash
gcc ejercicio5_espiral.c simula.o -lm -o ejercicio5
./ejercicio5
```

**Criterio de éxito:** El patrón visible en `visualize()` debe ser claramente una espiral.

---

#### Ejercicio 6: Limpieza Profunda (`ejercicio6_limpieza_profunda.c`)
**Objetivo:** Limpia el 80% de la suciedad del mapa.

**Conceptos a practicar:**
- Limpieza completa de cada celda (while loop)
- Exploración sistemática
- Gestión de batería para misiones largas

**Compilar y ejecutar:**
```bash
gcc ejercicio6_limpieza_profunda.c simula.o -lm -o ejercicio6
./ejercicio6
```

**Criterio de éxito:** `dirt_cleaned` ≥ 80% de `dirt_total` en `stats.csv`.

---

### Nivel Avanzado

#### Ejercicio 7: Cobertura Completa (`ejercicio7_cobertura_completa.c`)
**Objetivo:** Visita el 95% de las celdas accesibles del mapa.

**Conceptos a practicar:**
- Algoritmos de exploración avanzados
- Seguimiento sistemático de paredes
- Memoria de celdas visitadas (opcional: array 2D)
- Detección de callejones sin salida

**Compilar y ejecutar:**
```bash
gcc ejercicio7_cobertura_completa.c simula.o -lm -o ejercicio7
./ejercicio7
```

**Criterio de éxito:** `cell_visited` ≥ 95% de `cell_total` en `stats.csv`.

---

#### Ejercicio 8: Eficiencia Energética (`ejercicio8_eficiencia.c`)
**Objetivo:** Limpia 50+ unidades de suciedad con menos de 500 unidades de batería.

**Conceptos a practicar:**
- Optimización de movimientos
- Evitar colisiones (cada una cuesta 0.5)
- Minimizar movimientos diagonales (1.4 vs 1.0)
- Planificación de rutas eficientes

**Compilar y ejecutar:**
```bash
gcc ejercicio8_eficiencia.c simula.o -lm -o ejercicio8
./ejercicio8
```

**Criterio de éxito:** 
- Limpiar ≥ 50 unidades de suciedad
- Consumir < 500 unidades de batería (1000 inicial - batería final < 500)

---

#### Ejercicio 9: Robot Inteligente (`ejercicio9_robot_inteligente.c`)
**Objetivo:** Implementa una estrategia que adapte su comportamiento según el nivel de batería y suciedad detectada.

**Conceptos a practicar:**
- Máquina de estados compleja (5 modos)
- Comportamiento adaptativo
- Priorización dinámica de tareas
- Gestión inteligente de recursos

**Modos sugeridos:**
- `EXPLORACION_AGRESIVA` (batería > 50%)
- `LIMPIEZA_PROFUNDA` (batería > 50% + suciedad detectada)
- `CONSERVACION_ENERGIA` (batería 20-50%)
- `REGRESO_BASE` (batería < 20%)
- `RECARGANDO` (en base)

**Compilar y ejecutar:**
```bash
gcc ejercicio9_robot_inteligente.c simula.o -lm -o ejercicio9
./ejercicio9
```

**Criterio de éxito:** 
- El robot debe cambiar de modo según las condiciones
- Alta cobertura (>60%) y limpieza (>60%)
- Nunca quedarse sin batería

---

## Orden de Aprendizaje Recomendado

1. **Ejercicio 1** - Cuadrado Perfecto (control básico)
2. **Ejercicio 2** - Limpiador Simple (sensores básicos)
3. **Ejercicio 3** - Explorador (estrategias de navegación)
4. **Ejercicio 4** - Gestor de Batería (estados y navegación dirigida)
5. **Ejercicio 6** - Limpieza Profunda (combinación de habilidades)
6. **Ejercicio 5** - Espiral (algoritmo específico)
7. **Ejercicio 7** - Cobertura Completa (exploración avanzada)
8. **Ejercicio 8** - Eficiencia Energética (optimización)
9. **Ejercicio 9** - Robot Inteligente (proyecto integrador)

---

## Verificación de Resultados

Después de ejecutar cada ejercicio, revisa:

### Archivo `stats.csv`
```
cell_total,cell_visited,dirt_total,dirt_cleaned,bat_total,bat_mean,forward,turn,bumps,clean,load
```

**Métricas importantes:**
- **Cobertura:** `(cell_visited / cell_total) * 100`
- **Limpieza:** `(dirt_cleaned / dirt_total) * 100`
- **Batería consumida:** `1000 - bat_mean` (aprox)
- **Eficiencia:** `dirt_cleaned / bat_total`
- **Tasa de colisiones:** `bumps / forward`

### Visualización
Ejecuta `visualize()` en `finalizar()` para ver:
- Trayectoria del robot (puntos `.`)
- Áreas exploradas
- Suciedad restante (números 1-5)
- Posición final del robot

---

## Consejos Generales

### Para Todos los Ejercicios
- **Comienza simple:** Primero haz que funcione, luego optimiza
- **Usa printf():** Imprime información de depuración
- **Lee stats.csv:** Analiza métricas después de cada ejecución
- **Experimenta:** Prueba diferentes valores y estrategias

### Depuración
```c
// Añade prints para entender qué hace tu robot
sensor_t s = rmb_state();
printf("Pos: (%d,%d) Bat: %.1f IFR: %d\n", 
       s.x, s.y, s.battery, s.infrared);
```

### Variables Útiles
```c
// Variables estáticas mantienen su valor entre llamadas
static int contador = 0;
static float bateria_anterior = 1000.0;

// Variables globales (declarar antes de inicializar())
int base_x, base_y;
int celdas_visitadas = 0;
```

### Funciones Matemáticas
```c
#include <math.h>

// Distancia euclidiana
float distancia = sqrt(dx*dx + dy*dy);

// Ángulo hacia un punto
float angulo = atan2(dy, dx);

// Normalizar ángulos
while (angulo < 0) angulo += 2*M_PI;
while (angulo >= 2*M_PI) angulo -= 2*M_PI;
```

---

## Desafíos Adicionales

Una vez completados todos los ejercicios, intenta:

1. **Optimizar tiempo:** ¿Puedes completar el ejercicio 6 en < 2000 ticks?
2. **Minimizar colisiones:** ¿Puedes explorar 80% con < 10 colisiones?
3. **Máxima eficiencia:** Ejercicio 8 con < 300 unidades de batería
4. **Cobertura perfecta:** 100% de celdas visitadas
5. **Limpieza perfecta:** 100% de suciedad limpiada

---

## Recursos Relacionados

- **Ejemplos resueltos:** Ver `ejemplo1_inmovil.c` a `ejemplo9_autonomo.c`
- **Manual de Usuario:** `docs/usuario/manual_usuario.pdf`
- **README de Ejemplos:** `README_EJEMPLOS.md`
- **API Completa:** `simula.h`

---

## Compartir Soluciones

Guarda tus soluciones con nombres descriptivos:
```bash
cp ejercicio4_gestor_bateria.c mi_solucion_ejercicio4.c
```

Compara diferentes estrategias viendo las estadísticas y visualizaciones.

---

## Checklist de Validación

Antes de considerar un ejercicio completo, verifica:

- [ ] El código compila sin errores
- [ ] Cumple el objetivo numérico (porcentajes, umbrales)
- [ ] No hay errores en tiempo de ejecución
- [ ] La visualización muestra comportamiento correcto
- [ ] Las estadísticas confirman el éxito
- [ ] El código está comentado y es legible

---

¡Buena suerte con los ejercicios!
