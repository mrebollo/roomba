# roomba
roomba simulator for IPR-GIIROB-ETSINF-UPV

## Quick Start
- Build and run the project:
	- `make clean`
	- `make`
	- `./roomba`

- Run with a specific map:
	- `make run-map MAP=maps/sample_map.pgm`

- Generated files:
	- `log.csv`: robot trajectory
	- `stats.csv`: execution statistics
	- `map.pgm`: world snapshot (PGM image)

Tip: Press Ctrl-C during visualization to exit early.

## Map Tools

### Generate test maps:
```bash
make mapgen       # Compile map generator
./maps/generate   # Generate 8 test maps
```

### Visualize maps (without running simulation):
```bash
make viewmap                    # Compile map viewer
./maps/viewmap maps/walls2.pgm  # View a specific map
```

The viewer displays maps in ASCII format:
- `#` = Walls/obstacles
- `B` = Robot base
- `1`-`5` = Dirt level
- Spaces = Empty cells

See `maps/README.md` for more details.

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
