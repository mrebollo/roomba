# Reglas Oficiales de Competición Roomba

Sistema de evaluación para competiciones de programación de robots Roomba.

**Versión:** 1.0  
**Fecha:** Diciembre 2025  
**Institución:** IPR-GIIROB-ETSINF-UPV

---

## 1. Objetivo de la Competición

Los participantes deben programar un robot Roomba para que:
- **Explore** el máximo número de celdas del mapa
- **Limpie** la mayor cantidad de suciedad posible
- **Gestione** eficientemente su batería
- **Navegue** minimizando colisiones

La evaluación se realiza de forma automática en múltiples mapas con diferentes niveles de dificultad.

---

## 2. Escenarios de Evaluación

### 2.1 Mapas Oficiales

Cada equipo se evalúa en **4 mapas diferentes**:

| Mapa | Archivo | Obstáculos | Dificultad |
|------|---------|------------|------------|
| Tipo 0 | `noobs.pgm` | 0% | Principiantes |
| Tipo 1 | `random1.pgm` | 1% | Baja |
| Tipo 2 | `random3.pgm` | 3% | Media |
| Tipo 3 | `random5.pgm` | 5% | Alta |

**Características:**
- Tamaño: 50×50 celdas totales (48×48 navegables + paredes exteriores = 2304 celdas útiles)
- Suciedad: Distribuida aleatoriamente
- Base de recarga: Siempre en posición (1,1)

### 2.2 Sistema de Repeticiones

- **5 ejecuciones** por cada mapa
- **20 ejecuciones totales** por equipo
- Límite de tiempo: **50 segundos** por ejecución
- La puntuación final es el **promedio** de todas las ejecuciones

---

## 3. Criterios de Puntuación

El sistema evalúa **4 métricas principales** que se ponderan para obtener una puntuación final de 0 a 100 puntos.

### 3.1 Cobertura (30%)

**Definición:** Porcentaje de celdas del mapa visitadas por el robot.

**Fórmula:**
```
coverage = (cell_visited / cell_total) × 100
```

**Ejemplo:**
- Mapa: 2304 celdas
- Visitadas: 2056 celdas
- Cobertura: (2056 / 2304) × 100 = **89.2%**

**Puntuación:**
```
coverage_score = coverage × 0.30
```
Si coverage = 89.2%: **26.76 puntos**

**Interpretación:**
- < 30%: Exploración deficiente
- 30-60%: Exploración básica
- 60-85%: Buena exploración
- > 85%: Excelente cobertura

---

### 3.2 Eficiencia de Limpieza (35%)

**Definición:** Suciedad limpiada por unidad de batería consumida.

**Fórmula:**
```
dirt_efficiency = (dirt_cleaned / bat_total) × 100
```

**Ejemplo:**
- Suciedad limpiada: 198 unidades
- Batería consumida: 8076 unidades
- Eficiencia: (198 / 8076) × 100 = **2.45%**

**Puntuación:**
```
dirt_efficiency_score = dirt_efficiency × 0.35
```
Si dirt_efficiency = 2.45%: **0.86 puntos**

**Interpretación:**
- < 0.5%: Muy ineficiente
- 0.5-1.5%: Eficiencia básica
- 1.5-3.0%: Buena eficiencia
- > 3.0%: Excelente eficiencia

---

### 3.3 Conservación de Batería (20%)

**Definición:** Batería media mantenida durante la ejecución.

**Fórmula:**
```
battery_conservation = (bat_mean / 1000) × 100
```

**Ejemplo:**
- Batería media: 456.2 unidades
- Máxima: 1000 unidades
- Conservación: (456.2 / 1000) × 100 = **45.6%**

**Puntuación:**
```
battery_conservation_score = battery_conservation × 0.20
```
Si battery_conservation = 45.6%: **9.12 puntos**

**Interpretación:**
- < 20%: Gestión pobre
- 20-40%: Gestión básica
- 40-60%: Buena gestión
- > 60%: Excelente gestión

---

### 3.4 Calidad de Movimiento (15%)

**Definición:** Ratio de movimientos exitosos sin colisiones.

**Fórmula:**
```
movement_quality = (1 - bumps / forward) × 100
```

**Ejemplo:**
- Movimientos hacia adelante: 1534
- Colisiones: 118
- Calidad: (1 - 118/1534) × 100 = **92.3%**

**Puntuación:**
```
movement_quality_score = movement_quality × 0.15
```
Si movement_quality = 92.3%: **13.85 puntos**

**Interpretación:**
- < 70%: Navegación muy torpe
- 70-85%: Navegación aceptable
- 85-95%: Buena navegación
- > 95%: Navegación excelente

---

### 3.5 Puntuación Base

La suma de las 4 métricas ponderadas:

```
base_score = coverage_score + 
             dirt_efficiency_score + 
             battery_conservation_score + 
             movement_quality_score
```

**Ejemplo completo:**
```
base_score = 26.76 + 0.86 + 9.12 + 13.85 = 50.59 puntos
```

---

## 4. Bonificaciones

### 4.1 Bonificación por Completitud (+5 puntos)

Se otorga si se cumplen **todos** estos criterios:
- **Cobertura ≥ 95%** del mapa
- **Limpieza ≥ 90%** de la suciedad
- **Sin crashes** del programa

**Ejemplo:**
```
coverage = 96.2%          ✓
dirt_cleaned = 220/230    ✓ (95.7%)
crashes = 0               ✓
→ Bonificación: +5 puntos
```

---

### 4.2 Bonificación por Navegación Limpia (+3 puntos)

Se otorga si el número de colisiones es **menor que 5** en la ejecución completa.

**Ejemplo:**
```
bumps = 3 colisiones
→ Bonificación: +3 puntos
```

---

## 5. Penalizaciones

### 5.1 Penalización por Crash (-10 puntos)

Si el programa del equipo **termina inesperadamente** (segmentation fault, abort, etc.):

```
penalty = -10 puntos por cada crash
```

**Comportamientos que causan crash:**
- Segmentation fault
- Abort signal
- Error de compilación (si ocurre durante la competición)
- Timeout excedido (>50 segundos)

**Nota:** Un crash cuenta como ejecución fallida con puntuación de 0 para esa ejecución.

---

## 6. Cálculo de Puntuación Final

### 6.1 Fórmula Completa

```
final_score = base_score + bonuses - penalties

donde:
  base_score = suma de 4 métricas ponderadas (0-100)
  bonuses = completion_bonus + low_bumps_bonus (0-8)
  penalties = crash_penalty × número_de_crashes
```

### 6.2 Ejemplo Completo

**Datos de ejecución:**
- Cobertura: 89.2% → 26.76 puntos
- Eficiencia limpieza: 2.45% → 0.86 puntos
- Conservación batería: 45.6% → 9.12 puntos
- Calidad movimiento: 92.3% → 13.85 puntos
- Colisiones: 3 (< 5) → +3 puntos
- Sin completar mapa → +0 puntos
- Sin crashes → -0 puntos

**Cálculo:**
```
base_score = 26.76 + 0.86 + 9.12 + 13.85 = 50.59
bonuses = 0 + 3 = 3
penalties = 0

final_score = 50.59 + 3 - 0 = 53.59 puntos
```

### 6.3 Puntuación por Equipo (Promedio)

La puntuación oficial de cada equipo es el **promedio** de sus 20 ejecuciones (5 repeticiones × 4 mapas):

```
team_score = (sum(scores_20_executions)) / 20
```

---

## 7. Configuración de Pesos

Los pesos de las métricas están definidos en `scoring.conf`:

```ini
# Pesos de métricas (deben sumar 100)
coverage_weight=30
dirt_efficiency_weight=35
battery_conservation_weight=20
movement_quality_weight=15

# Bonificaciones
completion_bonus=5
low_bumps_bonus=3
bumps_threshold=5

# Penalizaciones
crash_penalty=10
```

**Nota:** Los organizadores pueden ajustar estos valores antes de la competición, pero no durante.

---

## 8. Criterios de Desempate

Si dos equipos tienen la misma puntuación final (redondeada a 2 decimales), se aplican estos criterios en orden:

1. **Mayor cobertura promedio** (cell_visited/cell_total)
2. **Mayor eficiencia de limpieza promedio** (dirt_cleaned/bat_total)
3. **Menor número de crashes totales**
4. **Menor número de colisiones totales** (bumps)
5. **Mayor conservación de batería promedio** (bat_mean)

Si persiste el empate, ambos equipos comparten la posición.

---

## 9. Restricciones Técnicas

### 9.1 Código Fuente

- **Archivo único:** `main.c` (sin subdirectorios)
- **Tamaño máximo:** 100 KB
- **Encoding:** UTF-8
- **Dependencias:** Solo biblioteca estándar de C y `simula.h`

### 9.2 APIs Permitidas

**Funciones del simulador (`simula.h`):**
```c
// Inicialización
rmb_awake(int *x, int *y)

// Movimiento
rmb_forward()
rmb_turn(float angle)

// Sensores
rmb_state()         // Retorna sensor_t
rmb_battery()       // Retorna float
rmb_ifr()           // Retorna int
rmb_bumper()        // Retorna int
rmb_at_base()       // Retorna int

// Acciones
rmb_clean()
rmb_load()
```

**Biblioteca estándar de C:**
- `<stdio.h>`, `<stdlib.h>`, `<math.h>`, `<string.h>`, `<time.h>`

### 9.3 APIs Prohibidas

- **NO** se permite usar `system()`, `exec()`, `fork()`
- **NO** se permite lectura/escritura de archivos externos
- **NO** se permite comunicación por red
- **NO** se permite uso de librerías externas (excepto estándar de C)

**Violación de estas restricciones:** Descalificación automática

---

## 10. Límites de Ejecución

| Parámetro | Valor | Acción si se excede |
|-----------|-------|---------------------|
| Tiempo de ejecución | 50 segundos | Timeout → crash (-10 puntos) |
| Memoria | 256 MB | Crash por OOM (-10 puntos) |
| Ciclos de simulación | 2500 ciclos | Terminación automática |

---

## 11. Formato de Entrega

### 11.1 Estructura Requerida

La entrega consiste en **un único archivo:**

```
main.c
```

**El archivo debe:**
- Compilar correctamente con el paquete de distribución proporcionado
- Usar solo la API estándar de `simula.h`
- Ser autocontenido (sin dependencias externas)

**Prohibido incluir:**
- Archivos compilados (`.o`, ejecutables)
- Mapas (`.pgm`)
- Configuraciones personalizadas
- Otros archivos `.c` o `.h`
- Subdirectorios

### 11.2 Validación Previa

Antes de entregar, los participantes deben:

1. **Compilar exitosamente:**
```bash
gcc main.c simula.o -lm -o roomba
```

2. **Probar la ejecución:**
```bash
./roomba map.pgm
```

3. **Verificar autoevaluación:**
```bash
./myscore stats.csv
```

La herramienta de validación del organizador (`validate`) verificará:
- Existencia de `main.c`
- Compilación exitosa sin errores
- Ejecución sin crashes
- Uso correcto de la API

---

## 12. Proceso de Evaluación

### 12.1 Flujo Automático

```
1. runner compila cada equipo con la biblioteca estándar
2. runner ejecuta 20 veces cada equipo (4 mapas × 5 repeticiones)
3. runner guarda estadísticas en results/teamXX/mapX_stats.csv
4. score calcula puntuaciones usando libscore
5. score genera ranking.txt y scores.csv
```

### 12.2 Transparencia

- Todos los equipos usan la **misma biblioteca** precompilada
- Todos los equipos se ejecutan en el **mismo entorno**
- El código de puntuación (`libscore`) es **público y auditable**
- Los logs completos están disponibles en `logs/`

---

## 13. Interpretación de Resultados

### 13.1 Archivo `ranking.txt`

```
=== RANKING FINAL ===
Pos | Equipo  | Puntuación | Coverage | DirtEff | BatCons | Movement
  1 | team15  |      67.24 |    89.2% |   2.45% |   45.6% |    92.3%
  2 | team08  |      64.18 |    86.1% |   2.31% |   48.2% |    88.7%
  3 | team22  |      61.45 |    94.5% |   1.82% |   38.9% |    85.2%
```

**Columnas:**
- **Pos:** Posición en el ranking
- **Equipo:** Identificador del equipo
- **Puntuación:** Puntuación final promediada (0-108)
- **Coverage:** Cobertura promedio (%)
- **DirtEff:** Eficiencia de limpieza promedio (%)
- **BatCons:** Conservación de batería promedio (%)
- **Movement:** Calidad de movimiento promedio (%)

### 13.2 Archivo `scores.csv`

```csv
team,score
team15,67.24
team08,64.18
team22,61.45
```

Formato simple para análisis estadístico y gráficos.

---

## 14. Estrategias Recomendadas

### 14.1 Para Maximizar Cobertura
- Implementar algoritmos de exploración sistemática (wall-following, espiral)
- Evitar repetir celdas visitadas
- Gestionar bien las recargas para no quedarse sin batería

### 14.2 Para Maximizar Eficiencia de Limpieza
- Detectar y limpiar toda la suciedad en cada celda visitada
- Optimizar rutas para pasar por zonas con más suciedad
- Evitar movimientos innecesarios

### 14.3 Para Conservar Batería
- Minimizar giros innecesarios (consumen más que avanzar)
- Recargar estratégicamente (no esperar a batería crítica)
- Evitar colisiones (detectar obstáculos antes de chocar)

### 14.4 Para Mejorar Calidad de Movimiento
- Implementar detección de obstáculos proactiva
- Usar algoritmos de path planning
- Evitar movimientos aleatorios excesivos

---

## 15. Preguntas Frecuentes

### P1: ¿Puedo usar código de ejemplo proporcionado?
**R:** Sí, los ejemplos en `samples/` son de referencia libre. Pero se evalúa originalidad e implementación.

### P2: ¿Qué pasa si mi código no compila?
**R:** No se ejecutará y recibirás 0 puntos. Usa `validate` antes de entregar.

### P3: ¿Puedo ver los mapas antes de la competición?
**R:** No. Los mapas oficiales solo se revelan durante la evaluación. Practica con mapas similares.

### P4: ¿Qué pasa si mi robot crashea en solo 1 de 20 ejecuciones?
**R:** Esa ejecución recibe 0 puntos. Las otras 19 se promedian normalmente. Penalización: -10 puntos adicionales.

### P5: ¿Puedo depurar mi código durante la competición?
**R:** No. Solo se permite una entrega. Usa `myscore` localmente para depurar antes.

### P6: ¿Cómo sé si estoy mejorando?
**R:** Usa la herramienta `myscore` en tu directorio local para autoevaluarte.

---

## 16. Ética y Conducta

### 16.1 Código Original
- El código debe ser **original** del equipo
- Se permite consultar documentación y ejemplos públicos
- **NO** se permite copiar código de otros equipos

### 16.2 Fair Play
- **NO** se permite intentar interferir con otros equipos
- **NO** se permite explotar vulnerabilidades del sistema
- **NO** se permite modificar archivos de configuración externos

**Violación:** Descalificación inmediata

---

## 17. Contacto y Soporte

Para consultas sobre las reglas:
- **Participantes:** Consulta `STUDENT_GUIDE.md` primero
- **Organizadores:** Consulta `ORGANIZER_GUIDE.md`
- **Técnicas:** Consulta manual del desarrollador

---

## 18. Cambios en las Reglas

**Versión 1.0:** Diciembre 2025 (versión inicial)

Los organizadores se reservan el derecho de modificar estas reglas antes del inicio de la competición. Los cambios se comunicarán con al menos **1 semana** de antelación.

---

**Sistema de competición IPR-GIIROB-ETSINF-UPV | Diciembre 2025**
