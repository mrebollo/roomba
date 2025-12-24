# Simulador Roomba - IPR-GIIROB-ETSINF-UPV

## Inicio rápido
- Compila y ejecuta el proyecto:
   - `make`
   - `./roomba`

- Ejecuta con un mapa específico:
   - `make run-map MAP=maps/sample_map.pgm`

- Archivos generados:
   - `log.csv`: trayectoria del robot
   - `stats.csv`: estadísticas de ejecución
   - `map.pgm`: imagen del mundo (PGM)

Consejo: Pulsa Ctrl-C durante la visualización para interrumpirla y salir.

**Modo competición:**
- Para compilar en modo competición (sin visualización y con tiempo fijo):
   - `make single`
   - O manualmente: `gcc -DCOMPETITION_MODE=1 ...`
   - El número de iteraciones (exec_time) será fijo y la visualización estará desactivada.


## Herramientas de mapas

### Generar mapas de prueba:
```bash
make tools        # Compila todas las herramientas (incluyendo mapas)
./tools/generate  # Genera 8 mapas de prueba en maps/
```

### Visualizar mapas (sin simular):
```bash
./tools/viewmap maps/walls2.pgm  # Visualiza un mapa concreto
```

El visor muestra mapas en formato ASCII:
- `#` = Muros/obstáculos
- `B` = Base del robot
- `1`-`5` = Nivel de suciedad
- Espacios = Celdas vacías

Consulta `tools/README.md` para más detalles.

## Para Organizadores: Crear Paquete de Distribución Standalone

### Generar paquete standalone
```bash
make lib        # Compila la librería del simulador (simula.o)
make dist       # Crea el paquete de distribución en dist/
```

El directorio `dist/` contiene todo lo necesario para el desarrollo en modo Standalone:
- **`simula.o`** - Librería precompilada del simulador
- **`simula.h`** - Documentación de la API pública
- **`main.c`** - Plantilla limpia con comentarios guía
- **`maps/`** - Mapas de prueba (noobs, random1-5, walls1-4)
- **`Makefile`** - Sistema de compilación simplificado
- **`README.md`** - Guía de inicio rápido

### Distribuir a los participantes
```bash
# Crear archivo comprimido
tar -czf roomba-standalone.tar.gz dist/
# O crear archivo zip
zip -r roomba-standalone.zip dist/

# Subir a la plataforma de enseñanza (Moodle/Aula Virtual)
```

Los participantes simplemente extraen y comienzan a programar:
```bash
tar -xzf roomba-standalone.tar.gz
cd dist/
make
./roomba
```

### Limpiar distribución
```bash
make dist-clean  # Elimina archivos generados, mantiene plantillas
```

---

## Sistema para Organizadores (Arena)

El proyecto incluye un sistema de competición completo para evaluar entregas automáticamente.

### Para Participantes (Modo Standalone)

**Primeros Pasos:**
1. Recibe el paquete de distribución (`dist/`)
2. Lee `competition/PARTICIPANT_GUIDE.md` para instrucciones completas
3. Usa la herramienta `myscore` para autoevaluación antes de la entrega

**Autoevaluación Rápida:**
```bash
# Después de ejecutar tu código
./tools/myscore stats.csv
```

Consulta `competition/PARTICIPANT_GUIDE.md` para estrategias detalladas de optimización.

### Para Organizadores (Modo Arena)

**Flujo de Trabajo de Competición:**

1. **Configuración:**
   ```bash
   make arena  # Compila runner, score, myscore, validate
   ```

2. **Recolección de Entregas:**
   - Coloca las entregas en `competition/teams/teamXX/main.c`
   - Consulta `competition/ORGANIZER_GUIDE.md` para más detalles

3. **Validación de Entregas:**
   ```bash
   ./tools/validate competition/teams/team01  # Valida un equipo individual
   ```

4. **Ejecutar Competición:**
   ```bash
   cd competition && ./runner  # Ejecuta todos los equipos
   ```

5. **Generar Rankings:**
   ```bash
   cd competition && ./score  # Genera ranking.txt y scores.csv
   ```

**Documentación:**
- `competition/README.md` - Resumen e inicio rápido
- `competition/RULES.md` - Reglas oficiales y puntuación
- `competition/PARTICIPANT_GUIDE.md` - Guía completa para participantes
- `competition/ORGANIZER_GUIDE.md` - Guía completa para organizadores

---

## Documentación

### Manuales de Usuario
- **Manual de Usuario (MD):** `docs/user/MANUAL_USUARIO.md` - Guía paso a paso
- **Manual de Usuario (LaTeX):** `docs/user/manual_usuario.tex` - Documentación completa en PDF
- **Ejemplos:** `samples/README.md` - 9 ejemplos resueltos + 9 ejercicios
- **Ejercicios:** `samples/README_EJERCICIOS.md` - Ejercicios guiados con soluciones

### Documentación Técnica
- **Manual del Desarrollador:** `docs/developer/manual_desarrollador.tex` - Arquitectura interna
- **Referencia API:** `simula.h` - Documentación completa de funciones
- **Sistema de Competición:** `competition/README.md` - Arquitectura de evaluación

### Documentación de Herramientas
- **Herramientas:** `tools/README.md` - Generación de mapas, visualización, validación
- **Guía del Organizador:** `competition/ORGANIZER_GUIDE.md` - Runner, score, validate

---

## Estructura del Proyecto

```
roomba/
├── simula.c/h              # Main simulator
├── sim_*.c                 # Simulator modules (robot, world, visual, io, stats)
├── main.c                  # Default main program
├── Makefile                # Build system
├── competition/            # Arena evaluation system
│   ├── runner              # Competition orchestrator
│   ├── score               # Ranking calculator
│   ├── libscore.c/h        # Scoring library
│   ├── README.md           # Competition overview
│   ├── RULES.md            # Official rules
│   ├── PARTICIPANT_GUIDE.md # Participant documentation
│   ├── ORGANIZER_GUIDE.md  # Organizer documentation
│   ├── teams/              # Participant submissions
│   ├── maps/               # Competition maps
│   ├── results/            # Execution results
│   └── logs/               # Execution logs
├── dist/                   # Standalone distribution package
│   ├── simula.o            # Precompiled library
│   ├── simula.h            # API header
│   ├── main.c              # Template
│   └── maps/               # Test maps
├── samples/                # Example code and exercises
├── tools/                  # Development utilities
│   ├── generate            # Map generator (binary)
│   ├── generate.c          # Map generator (source)
│   ├── viewmap             # Map visualizer
│   ├── validate.c          # Code validator
│   ├── myscore.c           # Self-assessment tool
│   └── Makefile            # Tools build system
├── maps/                   # Map storage
│   └── *.pgm               # Test maps
└── docs/                   # Documentation
   ├── user/               # User manuals (MD + LaTeX)
    ├── developer/          # Developer manual (LaTeX)
    ├── README.md           # Documentation index
    └── Makefile            # Docs build system
```

---

## License

See LICENSE file for details.

---

**Roomba Simulator | IPR-GIIROB-ETSINF-UPV | 2025**
