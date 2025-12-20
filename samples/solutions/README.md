# Soluciones de los Ejercicios Propuestos

Esta carpeta contiene **soluciones completas** de los 9 ejercicios propuestos en el Manual de Usuario del Simulador Roomba.

**IMPORTANTE:** Estas son **soluciones de referencia**. Se recomienda intentar resolver los ejercicios por tu cuenta antes de revisar estas implementaciones.

## Contenido

### Nivel Básico

#### `ejercicio1_cuadrado_perfecto.c`
**Estrategia:** Control preciso de pasos y giros de 90 grados.
- Variables: `pasos_en_lado`, `lados_completados`
- Lógica: Avanzar 10 pasos → Girar 90° → Repetir 4 veces

#### `ejercicio2_limpiador_simple.c`
**Estrategia:** Rebote aleatorio con prioridad de limpieza.
- Siempre limpia primero si detecta suciedad
- Navegación: Rebote aleatorio (45-135 grados)
- Objetivo: >50% de suciedad limpiada

#### `ejercicio3_explorador.c`
**Estrategia:** Seguimiento de pared derecha.
- Estados: AVANZANDO, GIRANDO_IZQ, AJUSTANDO_DER
- Algoritmo: Wall-following para máxima cobertura
- Objetivo: >30% de celdas visitadas

---

### Nivel Intermedio

#### `ejercicio4_gestor_bateria.c`
**Estrategia:** Máquina de estados con navegación dirigida.
- Estados: EXPLORANDO, REGRESANDO, RECARGANDO
- Umbral: Batería < 200 (20%) → Regresar
- Navegación: Usa `atan2()` para calcular ángulo hacia la base
- Recarga hasta 900 unidades

#### `ejercicio5_espiral.c`
**Estrategia:** Patrón de espiral cuadrada creciente.
- Secuencia: 1-1-2-2-3-3-4-4... pasos
- Lógica: Cada 2 lados, incrementar longitud
- Detiene al chocar con paredes

#### `ejercicio6_limpieza_profunda.c`
**Estrategia:** Seguimiento de paredes + limpieza completa + gestión de batería.
- Estados: EXPLORANDO, LIMPIANDO, REGRESANDO_BASE, RECARGANDO
- Limpia cada celda completamente (while `rmb_ifr() > 0`)
- Seguimiento de pared derecha para cobertura
- Recarga cuando batería < 150
- Objetivo: >80% de suciedad limpiada

---

### Nivel Avanzado

#### `ejercicio7_cobertura_completa.c`
**Estrategia:** Seguimiento sistemático de pared izquierda + detección de ciclos.
- Memoria: Compara posición actual vs anterior
- Anti-ciclos: Giro aleatorio si detecta bucle
- Pared izquierda: Gira a la izquierda cada 2 pasos
- Múltiples ciclos de recarga para alcanzar 95%
- Objetivo: >95% de celdas visitadas

#### `ejercicio8_eficiencia.c`
**Estrategia:** Optimización ultra-eficiente de movimientos.
- Evita colisiones (cada colisión = -0.5 batería)
- Giros mínimos (60° en lugar de 90°)
- Prioriza líneas rectas (evita diagonales 1.4 vs 1.0)
- Detiene automáticamente al alcanzar 50 limpiezas
- Monitorea consumo constantemente
- Objetivo: 50+ limpiezas con <500 batería

#### `ejercicio9_robot_inteligente.c`
**Estrategia:** Comportamiento adaptativo con 5 modos.

**Modos:**
1. **EXPLORACION_AGRESIVA** (batería >50%)
   - Movimientos rápidos, permite diagonales
   - Giros aleatorios amplios
   - Explora áreas nuevas

2. **LIMPIEZA_PROFUNDA** (detecta suciedad en modo agresivo)
   - Limpia completamente la celda
   - Vuelve a exploración después

3. **CONSERVACION_ENERGIA** (batería 15-40%)
   - Solo movimientos rectos
   - Giros de 90° exactos
   - Solo limpia suciedad alta (nivel 3+)

4. **REGRESO_BASE** (batería <15%)
   - Navegación dirigida eficiente
   - No limpia ni explora

5. **RECARGANDO** (en base)
   - Recarga hasta 850 unidades
   - Vuelve a modo agresivo

**Transiciones inteligentes:**
- Batería < 15% → REGRESO_BASE (crítico)
- Batería < 40% → CONSERVACION_ENERGIA
- Batería > 50% → EXPLORACION_AGRESIVA
- Detecta suciedad en agresivo → LIMPIEZA_PROFUNDA

---

## Compilar y Ejecutar

### Requisitos
Necesitas `simula.o` en la carpeta padre o en el mismo directorio.

### Compilación
```bash
cd samples/solutions
gcc ejercicio1_cuadrado_perfecto.c ../simula.o -lm -o ejercicio1
./ejercicio1
```

O desde la carpeta `samples`:
```bash
gcc solutions/ejercicio1_cuadrado_perfecto.c simula.o -lm -o ejercicio1
./ejercicio1
```

### Compilar todos
```bash
cd samples/solutions
for i in {1..9}; do
    gcc ejercicio${i}_*.c ../simula.o -lm -o ejercicio${i}
done
```

---

## Verificar Resultados

### Estadísticas (`stats.csv`)
```
cell_total,cell_visited,dirt_total,dirt_cleaned,bat_total,bat_mean,forward,turn,bumps,clean,load
```

**Criterios de éxito por ejercicio:**

| Ejercicio | Criterio Principal | Valor Objetivo |
|-----------|-------------------|----------------|
| 1 | Visual (cuadrado perfecto) | 10×10 celdas |
| 2 | `dirt_cleaned / dirt_total` | ≥ 50% |
| 3 | `cell_visited / cell_total` | ≥ 30% |
| 4 | `bat_mean` nunca llega a 0 | Recarga automática |
| 5 | Visual (patrón espiral) | Espiral clara |
| 6 | `dirt_cleaned / dirt_total` | ≥ 80% |
| 7 | `cell_visited / cell_total` | ≥ 95% |
| 8 | `dirt_cleaned` ≥ 50 Y `bat_total` < 500 | Ambos |
| 9 | Cobertura >60% Y Limpieza >60% | Adaptativo |

### Visualización
Todas las soluciones incluyen `visualize()` para ver la animación del recorrido.

---

## Puntos Clave de Cada Solución

### Ejercicio 1
- Uso de contadores simples
- Control exacto de bucles

### Ejercicio 2
- Priorización con `if-else`
- Números aleatorios con `rand()`

### Ejercicio 3
- Máquina de estados básica
- Algoritmo wall-following

### Ejercicio 4
- Navegación dirigida con `atan2()`
- Normalización de ángulos
- Estados con recarga

### Ejercicio 5
- Patrón matemático (espiral)
- Incremento controlado de variables

### Ejercicio 6
- 4 estados complejos
- Limpieza completa con `while`
- Gestión completa de batería

### Ejercicio 7
- Detección de ciclos
- Memoria de posición anterior
- Múltiples recargas para objetivo alto

### Ejercicio 8
- Optimización de cada acción
- Monitoreo constante de batería
- Detención condicional temprana

### Ejercicio 9
- 5 modos adaptativos
- Transiciones basadas en múltiples sensores
- Comportamiento emergente complejo

---

## Cómo Usar Estas Soluciones

### 1. Para Aprender
- **Compara** tu solución con la de referencia
- **Identifica** diferencias en la estrategia
- **Prueba** modificar parámetros para ver efectos

### 2. Para Depurar
- **Compila** ambas versiones (tuya y referencia)
- **Compara** `stats.csv` de ambas
- **Analiza** dónde difieren las métricas

### 3. Para Mejorar
- **Combina** técnicas de diferentes soluciones
- **Optimiza** parámetros (ángulos, umbrales)
- **Experimenta** con variaciones

---

## Modificaciones Sugeridas

### Ejercicio 1
- Cambiar tamaño del cuadrado (15×15)
- Hacer rectángulo (10×15)
- Hacer múltiples cuadrados concéntricos

### Ejercicio 2-3
- Cambiar estrategia de navegación
- Probar wall-following vs rebote
- Comparar eficiencia

### Ejercicio 4-6
- Ajustar umbrales de batería
- Cambiar algoritmo de navegación a base
- Optimizar cobertura vs limpieza

### Ejercicio 7
- Probar pared derecha vs izquierda
- Implementar memoria completa (array 2D)
- Algoritmos de exploración avanzados

### Ejercicio 8
- Reducir umbral a <400 batería
- Aumentar objetivo a 70+ limpiezas
- Implementar predicción de rutas

### Ejercicio 9
- Añadir más modos (ej: PATRULLAJE)
- Ajustar umbrales de transición
- Implementar aprendizaje simple

---

## Comparativa de Estrategias

| Estrategia | Cobertura | Limpieza | Eficiencia | Complejidad |
|------------|-----------|----------|------------|-------------|
| Rebote aleatorio | Media | Media | Baja | Baja |
| Wall-following | Alta | Media | Media | Media |
| Espiral | Media-Alta | Baja | Media | Media |
| Adaptativa | Alta | Alta | Alta | Alta |

---

## Mejores Prácticas Observadas

### Gestión de Batería
```c
// Siempre verificar antes de acciones costosas
if (rmb_battery() < umbral) {
    modo = REGRESO_BASE;
}
```

### Limpieza Completa
```c
// Limpiar hasta vaciar completamente
while (rmb_ifr() > 0) {
    rmb_clean();
}
```

### Navegación Dirigida
```c
// Normalizar diferencia de ángulos
while (dif > M_PI) dif -= 2 * M_PI;
while (dif < -M_PI) dif += 2 * M_PI;
```

### Detección de Ciclos
```c
// Comparar posición actual vs anterior
if (pos_actual.x == pos_anterior.x && 
    pos_actual.y == pos_anterior.y) {
    // Estamos atrapados
}
```

---

## Recursos Relacionados

- **Ejercicios sin resolver:** `../ejercicio1_*.c` a `../ejercicio9_*.c`
- **Ejemplos del manual:** `../ejemplo1_*.c` a `../ejemplo9_*.c`
- **README de ejercicios:** `../README_EJERCICIOS.md`
- **README de ejemplos:** `../README_EJEMPLOS.md`
- **Manual de Usuario:** `../../docs/usuario/manual_usuario.pdf`

---

## Notas Importantes

1. **No copies directamente:** Estas soluciones son para aprender, no para copiar.
2. **Experimenta:** Modifica parámetros y observa los cambios.
3. **Compara métricas:** Usa `stats.csv` para análisis cuantitativo.
4. **Múltiples soluciones:** Puede haber muchas formas válidas de resolver cada ejercicio.
5. **Optimización:** Estas soluciones priorizan claridad sobre máxima optimización.

---

¡Úsalas sabiamente para mejorar tu comprensión del simulador!
