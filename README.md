
# Simulador Roomba - IPR-GIIROB-ETSINF-UPV

## Inicio rápido
- Compila y ejecuta el proyecto:
   - `make clean`
   - `make`
   - `./roomba`

- Ejecuta con un mapa específico:
   - `make run-map MAP=maps/sample_map.pgm`

- Archivos generados:
   - `log.csv`: trayectoria del robot
   - `stats.csv`: estadísticas de ejecución
   - `map.pgm`: imagen del mundo (PGM)


**Modo competición:**
- Para compilar en modo competición (sin visualización y con tiempo fijo para todos los equipos):
   - `make comp`
   - O manualmente: `gcc -DCOMPETITION_MODE=1 ...`
   - El número de iteraciones (exec_time) será el mismo para todos los equipos y la visualización estará desactivada.

Consejo: Pulsa Ctrl-C durante la visualización para interrumpirla y salir.

## Herramientas de mapas

### Generar mapas de prueba:
```bash
make mapgen       # Compila el generador de mapas
./maps/generate   # Genera 8 mapas de prueba
```

### Visualizar mapas (sin simular):
```bash
make viewmap                    # Compila el visor de mapas
./maps/viewmap maps/walls2.pgm  # Visualiza un mapa concreto
```

El visor muestra mapas en formato ASCII:
- `#` = Muros/obstáculos
- `B` = Base del robot
- `1`-`5` = Nivel de suciedad
- Espacios = Celdas vacías

Consulta `maps/README.md` para más detalles.

## For Teachers: Creating Student Distribution Package

### Generate student package
```bash
make lib        # Compile simulator library (simula.o)
make dist       # Create distribution package in dist/
```

The `dist/` directory contains everything students need:
- **`simula.o`** - Precompiled simulator library
- **`simula.h`** - Public API documentation
- **`main.c`** - Clean template with guidance comments
- **`maps/`** - Test maps (noobs, random1-5, walls1-4)
- **`Makefile`** - Simplified build system for students
- **`README.md`** - Quick start guide for students

### Distribute to students
```bash
# Create compressed archive
tar -czf roomba-student.tar.gz dist/
# Or create zip file
zip -r roomba-student.zip dist/

# Upload to Moodle/Virtual Campus
```

Students simply extract and start coding:
```bash
tar -xzf roomba-student.tar.gz
cd dist/
make
./roomba
```

### Clean distribution
```bash
make dist-clean  # Remove generated files, keep templates
```

---

## Competition System

The project includes a complete competition system for evaluating student submissions automatically.

### For Participants

**Getting Started:**
1. Receive the distribution package (`dist/`) from your instructor
2. Read `competition/PARTICIPANT_GUIDE.md` for complete instructions
3. Use `myscore` tool for self-assessment before submission

**Quick Self-Assessment:**
```bash
# After running your code
./myscore stats.csv
```

See `competition/PARTICIPANT_GUIDE.md` for detailed optimization strategies.

### For Organizers

**Complete Competition Workflow:**

1. **Setup:**
   ```bash
   cd competition
   make all  # Compile runner, score, myscore, validate
   ```

2. **Collect Submissions:**
   - Place student submissions in `competition/teams/teamXX/main.c`
   - See `competition/ORGANIZER_GUIDE.md` for details

3. **Validate Submissions:**
   ```bash
   ./validate teams/team01  # Validate individual team
   ```

4. **Run Competition:**
   ```bash
   ./runner  # Execute all teams on all maps (4 maps × 5 repetitions)
   ```

5. **Generate Rankings:**
   ```bash
   ./score  # Generate ranking.txt and scores.csv
   ```

**Documentation:**
- `competition/README.md` - Overview and quick start
- `competition/RULES.md` - Official competition rules and scoring
- `competition/PARTICIPANT_GUIDE.md` - Complete guide for participants
- `competition/ORGANIZER_GUIDE.md` - Complete guide for organizers

---

## Documentation

### User Manuals
- **User Manual (MD):** `docs/user/MANUAL_USUARIO.md` - Step-by-step programming guide
- **User Manual (LaTeX):** `docs/user/manual_usuario.tex` - Comprehensive PDF documentation
- **Examples:** `samples/README.md` - 9 solved examples + 9 exercises
- **Exercises:** `samples/README_EJERCICIOS.md` - Guided exercises with solutions

### Technical Documentation
- **Developer Manual:** `docs/developer/manual_desarrollador.tex` - System architecture and internals
- **API Reference:** `simula.h` - Complete function documentation
- **Competition System:** `competition/README.md` - Evaluation system architecture

### Tools Documentation
- **Map Tools:** `maps/README.md` - Map generation and visualization
- **Utility Tools:** `tools/README.md` - Additional development tools
- **Competition Tools:** `competition/ORGANIZER_GUIDE.md` - Runner, score, validate, myscore

---

## Project Structure

```
roomba/
├── simula.c/h              # Main simulator
├── sim_*.c                 # Simulator modules (robot, world, visual, io, stats)
├── main.c                  # Default main program
├── Makefile                # Build system
├── competition/            # Competition evaluation system
│   ├── runner              # Competition orchestrator
│   ├── score               # Ranking calculator
│   ├── myscore             # Self-assessment tool
│   ├── validate            # Code validator
│   ├── libscore.c/h        # Scoring library
│   ├── README.md           # Competition overview
│   ├── RULES.md            # Official rules
│   ├── PARTICIPANT_GUIDE.md # Participant documentation
│   ├── ORGANIZER_GUIDE.md  # Organizer documentation
│   ├── teams/              # Student submissions
│   ├── maps/               # Competition maps
│   ├── results/            # Execution results
│   └── logs/               # Execution logs
├── dist/                   # Student distribution package
│   ├── simula.o            # Precompiled library
│   ├── simula.h            # API header
│   ├── myscore             # Self-assessment tool
│   ├── main.c              # Template
│   └── maps/               # Test maps
├── samples/                # Example code and exercises
│   ├── ejemplo*.c          # 9 solved examples
│   ├── ejercicio*.c        # 9 guided exercises
│   └── solutions/          # Reference solutions
├── maps/                   # Map generation tools
│   ├── generate.c          # Map generator
│   └── *.pgm               # Test maps
├── tools/                  # Development utilities
│   ├── generate_map        # Map generator
│   ├── viewmap             # Map visualizer
│   └── validate.sh         # Code validator
└── docs/                   # Documentation
   ├── user/               # User manuals (MD + LaTeX)
    ├── developer/          # Developer manual (LaTeX)
    └── README.md           # Documentation index
```

---

## License

See LICENSE file for details.

---

**Roomba Simulator | IPR-GIIROB-ETSINF-UPV | 2025**
