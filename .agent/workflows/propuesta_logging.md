# Propuesta de Mejora: Sistema de Logging para el Simulador Roomba

## Motivación
Actualmente, el simulador solo genera log.csv (traza de posiciones y sensores para el visualizador), pero no existe un sistema de logging para eventos, advertencias o errores. Un sistema de log configurable facilitaría la depuración, el soporte y la auditoría en distintos escenarios de uso.

## Escenarios de uso
- **Usuario de la API (implementador de robots):**
  - Necesita entender el comportamiento de su robot y depurar problemas.
- **Organizador de competición:**
  - Quiere auditar el comportamiento de los equipos y detectar usos anómalos.
- **Desarrollador del simulador:**
  - Requiere trazas detalladas para depuración y desarrollo.

## Propuesta de diseño
### Niveles de log
- ERROR: Fallos graves, abortos, condiciones inesperadas.
- WARNING: Situaciones anómalas pero no críticas.
- INFO: Eventos relevantes del flujo normal.
- DEBUG: Detalles internos para desarrollo.

### Destino del log
- Por defecto, a stderr o a un archivo (sim.log).
- Configurable por macro, variable de entorno o parámetro.

### API sugerida
```c
void log_message(int level, const char* fmt, ...);
#define LOG_ERROR(...) log_message(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOG_LEVEL_WARN,  __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOG_LEVEL_INFO,  __VA_ARGS__)
#define LOG_DEBUG(...) log_message(LOG_LEVEL_DEBUG, __VA_ARGS__)
```

### Ejemplo de uso
```c
LOG_INFO("Robot loaded map %s", map.name);
LOG_WARN("Battery low: %.2f", rob->battery);
LOG_ERROR("Null pointer in rmb_awake");
LOG_DEBUG("Step: x=%d y=%d heading=%.2f", rob->x, rob->y, rob->heading);
```

### Configuración
- El nivel de log se puede fijar por macro, variable de entorno o parámetro de configuración.
- El log puede escribirse a archivo, stderr o ambos.

### Ventajas
- Un sistema común, flexible y configurable cubre todos los perfiles.
- No es necesario mantener varios sistemas de log, solo ajustar el nivel según el usuario/escenario.

## Siguiente paso
- Evaluar la prioridad de esta mejora para la próxima versión.
- Definir el API y puntos de integración en el simulador.
- Documentar el uso y buenas prácticas en los workflows del repositorio.
