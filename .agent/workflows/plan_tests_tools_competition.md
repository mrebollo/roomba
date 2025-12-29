# Plan de tests unitarios y de integración para tools/ y competition/

## Objetivo
Definir y priorizar los tests a implementar en las carpetas tools/ y competition/ para mejorar la robustez y trazabilidad del simulador y la infraestructura de competición.

---

## tools/
### 1. Tests unitarios
- Para funciones reutilizables (por ejemplo, en libscore.c, libscore.h):
  - Cálculo de puntuaciones.
  - Validaciones de entradas y salidas.
  - Comprobación de manejo de errores y casos límite.

### 2. Tests de integración
- Scripts y utilidades (compare_scoring.py, validate.c, viewmap.c, etc.):
  - Comprobar la salida esperada para entradas conocidas.
  - Verificar detección de errores en mapas/resultados.
  - Validar que los scripts funcionan correctamente con archivos de ejemplo y casos de error.

### 3. Smoke tests
- Ejecutar cada herramienta con parámetros mínimos y comprobar que no falla.
- Mantener y ampliar los tests de sanitizer y smoke existentes.

---

## competition/
### 1. Tests unitarios
- Para lógica en C (score.c, simula_comp.c, etc.):
  - Cálculo de puntuaciones y reglas de competición.
  - Validación de entradas y salidas.
  - Comprobación de manejo de errores y casos límite.

### 2. Tests de integración
- Simular una competición completa con varios equipos y mapas:
  - Comprobar que los resultados y logs son coherentes.
  - Verificar que los scripts de automatización (Makefile, runner.c, scripts/) funcionan correctamente en escenarios típicos y de error.
  - Validar la generación de informes y estadísticas.

### 3. Sanitizer/smoke
- Mantener los tests de sanitizer y smoke para asegurar que no hay errores graves ni crashes en ejecuciones básicas.

---

## Prioridad
1. Tests de scoring y validación de resultados (críticos para la competición).
2. Tests de integración para scripts y automatizaciones.
3. Tests unitarios de funciones internas complejas.
4. Ampliar smoke tests y sanitizer según se detecten nuevos casos.

---

## CI
- Integrar en el CI los tests más críticos (scoring, validación, smoke) lo antes posible.
- Añadir los demás progresivamente según se vayan completando.

---

## Siguiente paso inmediato
- Integrar los tests unitarios del API en el CI (ya completos y estables).
- Revisar y priorizar la implementación de los tests aquí listados en futuras iteraciones.

---

## Notas
- Documentar en README o workflows cómo ejecutar y mantener los tests.
- Revisar periódicamente la cobertura y actualizar prioridades según necesidades del proyecto.
