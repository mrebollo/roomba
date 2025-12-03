# Samples - Ejemplos y Ejercicios del Simulador Roomba

Esta carpeta contiene **ejemplos resueltos** y **ejercicios propuestos** del Manual de Usuario del Simulador Roomba.

## 📂 Contenido

### 📘 Ejemplos Resueltos (9 archivos)
Implementaciones completas que demuestran conceptos del manual:

- `ejemplo1_inmovil.c` - Robot inmóvil (estructura básica)
- `ejemplo2_avanza.c` - Robot que avanza
- `ejemplo3_cuadrado.c` - Robot dibujando cuadrado
- `ejemplo4_rebote.c` - Robot con rebote aleatorio
- `ejemplo5_sigue_paredes.c` - Seguimiento de paredes
- `ejemplo6_limpieza_basica.c` - Limpieza al detectar suciedad
- `ejemplo7_limpieza_completa.c` - Limpieza completa de celdas
- `ejemplo8_estadisticas.c` - Seguimiento de estadísticas
- `ejemplo9_autonomo.c` - Robot autónomo completo

**Ver detalles:** [`README_EJEMPLOS.md`](README_EJEMPLOS.md)

### 📝 Ejercicios Propuestos (9 archivos)
Plantillas con objetivos específicos para que implementes tu solución:

**Nivel Básico:**
- `ejercicio1_cuadrado_perfecto.c` - Cuadrado de 10×10 celdas
- `ejercicio2_limpiador_simple.c` - Limpiar 50% de suciedad
- `ejercicio3_explorador.c` - Visitar 30% de celdas

**Nivel Intermedio:**
- `ejercicio4_gestor_bateria.c` - Recarga automática
- `ejercicio5_espiral.c` - Patrón en espiral
- `ejercicio6_limpieza_profunda.c` - Limpiar 80% de suciedad

**Nivel Avanzado:**
- `ejercicio7_cobertura_completa.c` - Visitar 95% de celdas
- `ejercicio8_eficiencia.c` - Limpiar 50+ con < 500 batería
- `ejercicio9_robot_inteligente.c` - Comportamiento adaptativo

**Ver detalles:** [`README_EJERCICIOS.md`](README_EJERCICIOS.md)

## 🚀 Inicio Rápido

### Requisitos
Necesitas el simulador precompilado (`simula.o`) en esta carpeta o en una ruta accesible.

### Compilar y ejecutar un ejemplo
```bash
gcc ejemplo1_inmovil.c simula.o -lm -o ejemplo1
./ejemplo1
```

### Compilar y ejecutar un ejercicio
```bash
gcc ejercicio1_cuadrado_perfecto.c simula.o -lm -o ejercicio1
./ejercicio1
```

### Alternativa: Usar el Makefile del proyecto
```bash
# Copiar un ejemplo al directorio principal
cp samples/ejemplo1_inmovil.c 2025/main.c
cd 2025
make
./roomba
```

## 📊 Verificar Resultados

Después de ejecutar, revisa:

### Visualización
La función `visualize()` muestra una animación ASCII del recorrido del robot.

### Estadísticas (`stats.csv`)
```
cell_total,cell_visited,dirt_total,dirt_cleaned,bat_total,bat_mean,forward,turn,bumps,clean,load
```

**Métricas útiles:**
- **Cobertura:** `(cell_visited / cell_total) × 100`
- **Limpieza:** `(dirt_cleaned / dirt_total) × 100`
- **Eficiencia:** `dirt_cleaned / bat_total`

### Log de ejecución (`log.csv`)
Registro tick a tick del estado del robot.

## 🎓 Ruta de Aprendizaje Recomendada

### Para Principiantes
1. Estudia `ejemplo1_inmovil.c` a `ejemplo3_cuadrado.c`
2. Intenta `ejercicio1_cuadrado_perfecto.c`
3. Estudia `ejemplo4_rebote.c`
4. Intenta `ejercicio2_limpiador_simple.c` y `ejercicio3_explorador.c`

### Para Nivel Intermedio
1. Estudia `ejemplo5_sigue_paredes.c` a `ejemplo7_limpieza_completa.c`
2. Intenta `ejercicio4_gestor_bateria.c` a `ejercicio6_limpieza_profunda.c`

### Para Nivel Avanzado
1. Estudia `ejemplo9_autonomo.c`
2. Intenta `ejercicio7_cobertura_completa.c` a `ejercicio9_robot_inteligente.c`

## 🛠️ Estructura de un Programa Típico

```c
#include "simula.h"
#include <math.h>

// Variables globales (opcional)
int mi_variable = 0;

void inicializar() {
    int x, y;
    rmb_awake(&x, &y);
    // Inicialización adicional
}

void comportamiento() {
    // Lógica del robot (se ejecuta cada tick)
    
    // Variables static mantienen valor entre llamadas
    static int contador = 0;
    contador++;
    
    // Tu código aquí
}

void finalizar() {
    // Mostrar resultados (opcional)
    printf("Resultado final\n");
    
    // Visualización (obligatorio)
    visualize();
}

int main() {
    configure(inicializar, comportamiento, finalizar, 1000);
    run();
    return 0;
}
```

## 📚 Referencia de API

### Acciones del Robot
- `rmb_awake(&x, &y)` - Activar robot (devuelve posición inicial)
- `rmb_forward()` - Avanzar un paso
- `rmb_turn(angulo)` - Girar (radianes, + = izq, - = der)
- `rmb_clean()` - Limpiar celda actual (-1 nivel)
- `rmb_load()` - Recargar batería (+10, solo en base)

### Sensores
- `rmb_bumper()` - Detectar colisión (1 = choque, 0 = libre)
- `rmb_ifr()` - Nivel de suciedad (0-5)
- `rmb_battery()` - Batería restante (0-1000)
- `rmb_state()` - Todos los sensores (devuelve `sensor_t`)

### Configuración
- `configure(init, loop, fin, timeout)` - Configurar simulación
- `run()` - Ejecutar simulación
- `visualize()` - Mostrar animación (en `finalizar()`)

## 💡 Consejos Útiles

### Depuración
```c
#include <stdio.h>

sensor_t s = rmb_state();
printf("Pos: (%d,%d) Bat: %.1f Suciedad: %d\n", 
       s.x, s.y, s.battery, s.infrared);
```

### Ángulos Comunes
```c
M_PI / 2    // 90 grados (izquierda)
-M_PI / 2   // 90 grados (derecha)
M_PI        // 180 grados
M_PI / 4    // 45 grados
```

### Máquina de Estados
```c
typedef enum { ESTADO1, ESTADO2, ESTADO3 } Estado;
Estado estado_actual = ESTADO1;

void comportamiento() {
    switch(estado_actual) {
        case ESTADO1:
            // código
            estado_actual = ESTADO2;
            break;
        // ...
    }
}
```

## 🔍 Archivos Adicionales

- `main_template.c` - Plantilla básica vacía
- `eses.c`, `main.c`, `random.c`, `rob.c`, `roomba.c` - Ejemplos legacy

## 📖 Documentación Completa

- **Manual de Usuario:** `../docs/usuario/manual_usuario.pdf`
- **Manual del Desarrollador:** `../docs/developer/manual_desarrollador.pdf`
- **API Completa:** `../simula.h`
- **README Principal:** `../README.md`

## 🤝 Contribuir

Si creas una solución interesante:
```bash
cp mi_solucion.c mi_solucion_ejercicio4.c
```

Comparte tus estrategias comparando `stats.csv` con otros estudiantes.

---

**¿Dudas?** Consulta los manuales en `docs/` o revisa los ejemplos resueltos como referencia.

¡Feliz programación! 🤖🧹✨
