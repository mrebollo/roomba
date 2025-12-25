# Guía para Participantes - Competición Roomba

Guía completa para preparar, desarrollar y entregar tu código para la competición.

**Versión:** 1.0  
**Fecha:** Diciembre 2025

---

## 1. Introducción

Esta guía te ayudará a:
- Entender qué se espera de tu entrega
- Usar herramientas de autoevaluación
- Optimizar tu código para la competición
- Validar antes de entregar
- Resolver problemas comunes

**Antes de empezar, lee:** `RULES.md` para entender los criterios de evaluación.

---

## 2. Preparación del Código

### 2.1 Estructura de Entrega

Tu entrega debe consistir en un único archivo:

```
main.c
```

**Recibirás un paquete de distribución (`dist/`) que incluye:**
- `simula.h` - API del simulador
- `simula.o` - Biblioteca precompilada
- `myscore` - Herramienta de autoevaluación
- `map.pgm` - Mapa de ejemplo para pruebas
- Instrucciones de compilación

**Tu entrega solo debe contener:**
- **Un único archivo:** `main.c`
- **Sin subdirectorios**
- **Sin archivos adicionales** (ejecutables, objetos, mapas, etc.)
- **Codificación:** UTF-8
- **Tamaño máximo:** 100 KB

### 2.2 Plantilla Básica

Tu `main.c` debe seguir esta estructura:

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simula.h"

// Variables globales (si las necesitas)
struct {
    int x, y;
} base;

// Función de inicialización
void on_start() {
    rmb_awake(&base.x, &base.y);
    
    // Tu código de inicialización aquí
}

// Función de comportamiento cíclico
void cyclic_behav() {
    // Tu lógica principal aquí
    
    // Ejemplo: limpiar si hay suciedad
    while (rmb_ifr() > 0) {
        rmb_clean();
    }
    
    // Ejemplo: moverse
    rmb_forward();
}

// Función de finalización
void on_stop() {
    printf("\n");
    visualize();
}

// Función principal
int main(int argc, char *argv[]) {
    if (argc > 1)
        load_map(argv[1]);
    
    configure(on_start, cyclic_behav, on_stop, 50);
    run();
    
    return 0;
}
```

### 2.3 APIs Disponibles

**Funciones del simulador:**

```c
// Inicialización
int rmb_awake(int *x, int *y);  // Retorna posición inicial de la base

// Movimiento
void rmb_forward();              // Avanzar una celda
void rmb_turn(float angle);      // Girar (radianes)

// Sensores
sensor_t rmb_state();            // Estado completo del robot
float rmb_battery();             // Nivel de batería (0-1000)
int rmb_ifr();                   // Nivel de suciedad en celda actual
int rmb_bumper();                // 1 si hay colisión, 0 si no
int rmb_at_base();               // 1 si está en la base, 0 si no

// Acciones
void rmb_clean();                // Limpiar una unidad de suciedad
void rmb_load();                 // Recargar batería (solo en base)
```

**Estructura sensor_t:**

```c
typedef struct {
    int x, y;          // Posición actual
    float heading;     // Orientación (radianes)
    float battery;     // Batería restante
} sensor_t;
```

**Biblioteca estándar permitida:**
- `<stdio.h>`, `<stdlib.h>`, `<math.h>`, `<string.h>`, `<time.h>`

**Funciones PROHIBIDAS:**
- `system()`, `exec()`, `fork()` (manipulación de procesos)
- `fopen()`, `fwrite()`, `fread()` (archivos externos)
- Cualquier operación de red
- Librerías externas no estándar

---

## 3. Autoevaluación con myscore

### 3.1 ¿Qué es myscore?

`myscore` es una herramienta que te permite evaluar tu código **localmente** usando el mismo sistema de puntuación que se usará en la competición oficial.

### 3.2 Cómo Usar myscore

**Paso 0: Compilar herramientas (una sola vez)**

```bash
make -C tools
# O cd tools && make
```

**Paso 1: Compilar tu código**

```bash
# En el directorio raíz de tu proyecto
make
```

**Paso 2: Ejecutar tu robot**

```bash
./roomba map.pgm
```

Esto genera `stats.csv` en el mismo directorio.

**Paso 3: Evaluar con myscore**

```bash
./tools/myscore stats.csv
```

### 3.3 Interpretación de Resultados

`myscore` muestra:

```
=== AUTOEVALUACIÓN ROOMBA ===

Equipo: team15

--- Métricas Individuales ---
Cobertura:             89.2% → 26.76 puntos (peso: 30%)
Eficiencia limpieza:    2.45% →  0.86 puntos (peso: 35%)
Conservación batería:  45.6% →  9.12 puntos (peso: 20%)
Calidad movimiento:    92.3% → 13.85 puntos (peso: 15%)

--- Bonificaciones ---
Completitud: NO alcanzada (falta 4.8% cobertura)
Navegación limpia: SÍ (+3 puntos, solo 3 colisiones)

--- Puntuación Final ---
Base: 50.59 puntos
Bonos: +3.00 puntos
Total: 53.59 puntos

--- Evaluación ---
★★★☆☆ Rendimiento ACEPTABLE

Sugerencias de mejora:
- Cobertura está bien pero puedes mejorar
- Eficiencia de limpieza es baja: limpia más suciedad por batería
- Conservación de batería es buena
- Navegación excelente: pocas colisiones
```

### 3.4 Umbrales de Referencia

| Puntuación | Evaluación | Descripción |
|------------|------------|-------------|
| < 40 | Bajo | Necesitas mejorar varias métricas |
| 40-55 | Aceptable | Funcionamiento básico correcto |
| 55-70 | Bueno | Rendimiento competitivo |
| 70-85 | Muy Bueno | Excelente implementación |
| > 85 | Excelente | Top performers |

### 3.5 Consejos Basados en Resultados

**Si tu cobertura es baja (<60%):**
- Implementa exploración sistemática (wall-following, espiral)
- Evita quedar atrapado en ciclos
- Gestiona mejor las recargas de batería

**Si tu eficiencia de limpieza es baja (<1%):**
- Asegúrate de limpiar TODA la suciedad en cada celda
- Usa `while (rmb_ifr() > 0) rmb_clean();`
- Optimiza rutas para pasar por zonas con suciedad

**Si tu conservación de batería es baja (<30%):**
- Reduce giros innecesarios (consumen más que avanzar)
- Recarga estratégicamente (no esperes a batería crítica)
- Evita movimientos redundantes

**Si tu calidad de movimiento es baja (<80%):**
- Implementa detección de obstáculos antes de avanzar
- Evita rebote aleatorio excesivo
- Usa algoritmos más inteligentes de navegación

---

## 4. Validación Antes de Entregar

### 4.1 Herramienta validate

Antes de entregar, **SIEMPRE** valida tu código con:

```bash
./tools/validate .
```

### 4.2 Qué Verifica validate

La herramienta comprueba:

1. **Estructura de archivos:**
   - Existe `main.c`
   - No hay archivos prohibidos
   - No hay subdirectorios

2. **Compilación:**
   - El código compila sin errores
   - Reporta warnings (deberías corregirlos)

3. **Ejecución básica:**
   - El programa no crashea inmediatamente
   - Genera `stats.csv` correctamente

4. **Conformidad de API:**
   - Usa solo funciones permitidas

### 4.3 Ejemplo de Salida Correcta

```
Roomba Code Validator
===================================

Team: team15
========================================

Source file: main.c

Compilation: PASSED
Warnings: 0
Execution tests: 4/4 PASSED
Stats generation: PASSED

========================================
Summary: 5 passed, 0 warnings, 0 failed
========================================

Validation PASSED - Code is ready for competition!
```

### 4.4 Errores Comunes y Soluciones

**Error: "No .c source file found"**
```bash
# Solución: Asegúrate de que el archivo se llama main.c
mv mi_codigo.c main.c
```

**Error: "Compilation failed"**
```bash
# Solución: Revisa los mensajes de error del compilador
./tools/validate .
cat compile.log
```

**Error: "Execution TIMEOUT"**
```bash
# Solución: Tu código tarda demasiado o tiene bucle infinito
# Revisa tu lógica de terminación
```

**Error: "stats.csv not generated"**
```bash
# Solución: Tu código no está llamando correctamente a las funciones
# Asegúrate de seguir la plantilla básica
```

---

## 5. Estrategias de Desarrollo

### 5.1 Desarrollo Iterativo

**Fase 1: Funcionalidad Básica** (Objetivo: 30-40 puntos)
- Movimiento simple (avanzar y girar)
- Limpieza cuando detecta suciedad
- Evitar colisiones básicas

**Fase 2: Exploración Sistemática** (Objetivo: 45-60 puntos)
- Implementar wall-following o algún otro método
- Gestión básica de batería (recarga cuando <20%)
- Reducir colisiones

**Fase 3: Optimización** (Objetivo: 60-75 puntos)
- Exploración completa del mapa
- Limpieza exhaustiva en cada celda
- Gestión inteligente de batería
- Navegación sin colisiones

**Fase 4: Refinamiento** (Objetivo: >75 puntos)
- Algoritmos adaptativos
- Optimización de rutas
- Gestión predictiva de batería
- Minimización de movimientos redundantes

### 5.2 Algoritmos Recomendados

**Exploración:**
- **Wall-following (seguir paredes):** Simple y efectivo
- **Patrón en zigzag:** Cobertura sistemática
- **Aleatorio con memoria:** Para mapas complejos

**Gestión de batería:**
```c
float battery = rmb_battery();
float umbral = 200.0;  // 20% de batería

if (battery < umbral) {
    // Ir a base y recargar
    return_to_base();
    while (rmb_battery() < 900) {
        rmb_load();
    }
}
```

**Navegación inteligente:**
```c
// Calcular ángulo hacia objetivo
int dx = objetivo_x - rob.x;
int dy = objetivo_y - rob.y;
float angulo = atan2(dy, dx);
float diferencia = angulo - rob.heading;

// Normalizar a [-π, π]
while (diferencia > M_PI) diferencia -= 2*M_PI;
while (diferencia < -M_PI) diferencia += 2*M_PI;

rmb_turn(diferencia);
```

### 5.3 Depuración Local

**Usar visualización:**
```c
void on_stop() {
    printf("\n=== Estadísticas ===\n");
    printf("Batería final: %.1f\n", rmb_battery());
    visualize();  // Animación del recorrido
}
```

**Prints de depuración:**
```c
void cyclic_behav() {
    sensor_t rob = rmb_state();
    printf("Pos: (%d,%d) Bat: %.1f\n", rob.x, rob.y, rob.battery);
    
    // Tu lógica aquí
}
```

**Nota:** Elimina los prints antes de entregar (afectan rendimiento).

---

## 6. Optimización de Métricas

### 6.1 Maximizar Cobertura

**Objetivo:** Visitar >90% del mapa

**Técnicas:**
- Exploración sistemática (no aleatoria)
- Memoria de celdas visitadas
- Evitar quedarse atrapado en áreas pequeñas
- Múltiples ciclos de exploración después de recargar

**Ejemplo de memoria simple:**
```c
int visitado[50][50] = {0};

void cyclic_behav() {
    sensor_t rob = rmb_state();
    visitado[rob.x][rob.y] = 1;
    
    // Preferir celdas no visitadas
    // ... tu lógica
}
```

### 6.2 Maximizar Eficiencia de Limpieza

**Objetivo:** >2.5% de eficiencia

**Técnicas:**
- Limpiar COMPLETAMENTE cada celda antes de salir
- No perder tiempo en celdas sin suciedad
- Rutas optimizadas hacia zonas con suciedad

**Código clave:**
```c
// SIEMPRE limpia completamente
while (rmb_ifr() > 0) {
    rmb_clean();
}
```

### 6.3 Conservar Batería

**Objetivo:** Mantener >40% de batería media

**Técnicas:**
- Recargar antes de llegar a batería crítica
- Minimizar giros (cuestan más batería)
- Rutas directas (evitar zigzag innecesario)
- Evitar colisiones (cuestan batería)

**Estrategia de recarga:**
```c
// Calcular batería necesaria para volver a base
int distancia_a_base = abs(rob.x - base.x) + abs(rob.y - base.y);
float bateria_necesaria = distancia_a_base * 2.0 + 100; // Margen

if (rob.battery < bateria_necesaria) {
    // Ir a base ahora
}
```

### 6.4 Mejorar Calidad de Movimiento

**Objetivo:** >90% de movimientos sin colisión

**Técnicas:**
- Memoria de obstáculos detectados (construir mapa interno)
- Algoritmos de path planning en zonas ya exploradas
- Evitar movimientos aleatorios excesivos
- Preferir rutas conocidas sin obstáculos

**Nota:** El robot NO tiene sensores para detectar obstáculos antes de chocar. Solo puedes usar información de colisiones previas guardadas en memoria.

**Gestión de colisiones:**
```c
// Guardar obstáculos detectados
int obstaculos[50][50] = {0};

void cyclic_behav() {
    sensor_t rob = rmb_state();
    
    rmb_forward();
    if (rmb_bumper()) {
        // Registrar obstáculo
        int next_x = rob.x + cos(rob.heading);
        int next_y = rob.y + sin(rob.heading);
        obstaculos[next_x][next_y] = 1;
        
        // Girar para evitar
        rmb_turn(M_PI/2);
    }
}
```

---

## 7. Errores Comunes

### 7.1 Errores de Compilación

**Error:** `undefined reference to 'rmb_awake'`
```bash
# Solución: Falta enlazar con simula.o
gcc main.c simula.o -lm -o roomba
```

**Error:** `simula.h: No such file or directory`
```bash
# Solución: simula.h debe estar en el mismo directorio que main.c
# Verifica que esté incluido en el paquete de distribución
ls -la simula.h
```

### 7.2 Errores de Ejecución

**Segmentation fault:**
- Arrays con índices fuera de rango
- Punteros no inicializados
- Recursión infinita

**Bucle infinito:**
- Condiciones de salida incorrectas en `while`
- Lógica de navegación que atrapa al robot
- Falta de gestión de batería

**Timeout:**
- Código demasiado lento
- Bucles innecesariamente largos
- Cálculos complejos en cada ciclo

### 7.3 Errores de Lógica

**Robot no se mueve:**
```c
// Verifica que estés llamando rmb_forward()
void cyclic_behav() {
    rmb_forward();  // ¡No olvides esto!
}
```

**Robot no limpia:**
```c
// Verifica que estés limpiando cuando detectas suciedad
if (rmb_ifr() > 0) {
    while (rmb_ifr() > 0) {
        rmb_clean();
    }
}
```

**Robot no recarga:**
```c
// Verifica que estés en la base antes de cargar
if (rmb_at_base()) {
    while (rmb_battery() < 900) {
        rmb_load();
    }
}
```

---

## 8. Checklist Pre-Entrega

Antes de entregar, verifica:

- [ ] El código compila sin errores con `gcc main.c simula.o -lm -o roomba`
- [ ] No hay warnings (o están justificados)
- [ ] Has probado con el mapa de ejemplo incluido en `dist/`
- [ ] Has probado con diferentes mapas (si tienes acceso a más)
- [ ] `myscore stats.csv` muestra puntuación >40
- [ ] Has eliminado todos los prints de depuración innecesarios
- [ ] El archivo se llama exactamente `main.c`
- [ ] El archivo es autocontenido (solo necesita simula.h y simula.o)
- [ ] Has verificado que el código funciona en el entorno de distribución
- [ ] Has leído completamente `RULES.md`

---

## 9. FAQ Participantes

### P1: ¿Puedo usar código de los ejemplos?
**R:** Sí, los ejemplos en `samples/` son de referencia libre. Pero debes entenderlos y adaptarlos.

### P2: ¿Cuántas veces puedo ejecutar myscore?
**R:** Las que quieras. Es una herramienta local para ti.

### P3: ¿Qué mapa debo usar para probar?
**R:** Usa mapas similares a los oficiales (48×48, diferentes densidades de obstáculos). No tendrás acceso a los mapas exactos de la competición.

### P4: ¿Puedo trabajar en equipo?
**R:** Sí, si la competición lo permite. Consulta con tu organizador.

### P5: ¿Cuánto tiempo tengo para desarrollar?
**R:** Consulta con tu organizador. Típicamente 2-4 semanas.

### P6: ¿Puedo entregar varias versiones?
**R:** Depende de las reglas de tu competición. Generalmente solo se acepta una entrega final.

### P7: ¿Qué pasa si mi código provoca fallos de ejecución?
**R:** Esa ejecución recibe 0 puntos y -10 de penalización. Las otras ejecuciones cuentan normalmente.

### P8: ¿Puedo ver los resultados de otros equipos antes de entregar?
**R:** No. Los resultados se publican después de la fecha límite de entrega.

### P9: ¿Qué pasa si dos equipos tienen el mismo código?
**R:** Ambos pueden ser descalificados por copia. Asegúrate de que tu código es original.

### P10: ¿Puedo usar inteligencia artificial para generar código?
**R:** Consulta con tu organizador. Generalmente se permite como asistencia, pero debes entender y poder explicar tu código.

---

## 10. Recursos Adicionales

### 10.1 Documentación

- **Manual del Usuario:** `docs/user/MANUAL_USUARIO.md`
- **Ejemplos de código:** `samples/ejemplo*.c`
- **Ejercicios:** `samples/ejercicio*.c`

### 10.2 Herramientas

- **myscore:** Autoevaluación local con puntuación detallada
- **validate:** Pre-validación de entrega antes de enviar
- **visualize():** Animación visual del recorrido del robot
- **stats.csv:** Archivo con todas las métricas de ejecución
- **Generación de mapas:** Usa `tools/generate.c` para crear mapas de prueba personalizados
- **Visualización de mapas:** Los archivos `.pgm` se pueden abrir con GIMP, ImageMagick o visualizadores PGM en modo gráfico, o con el visualizador proporcionado en modo texto.


---

## 11. Contacto

Para consultas:
- **Reglas de la competición:** Lee `RULES.md`
- **Problemas técnicos:** Consulta con tu organizador
- **Errores del sistema:** Informa a los organizadores

---

## 12. Consejos Finales

1. **Empieza simple:** Código básico que funcione es mejor que código complejo que falla
2. **Itera:** Mejora incrementalmente basándote en `myscore`
3. **Prueba exhaustivamente:** Diferentes mapas y escenarios
4. **Valida siempre:** Usa `validate` antes de entregar
5. **Lee las reglas:** Completamente, varias veces
6. **No dejes para el final:** Empieza con tiempo
7. **Depura sistemáticamente:** No hagas cambios aleatorios
8. **Comenta tu código:** Te ayudará a ti y a tu equipo
9. **Guarda copias:** Versiones funcionales anteriores
10. **¡Diviértete!:** Es un ejercicio de aprendizaje

---

**¡Mucha suerte en la competición!**

**Sistema de competición IPR-GIIROB-ETSINF-UPV | Diciembre 2025**
