# Makefile for Roomba Project
#
# This Makefile compiles the standalone robot behavior (main.c)
# combined with the simulation library (simula.c) for development and testing.
#
# Usage:
#   make          - Compiles the standalone project (normal mode)
#   make run      - Compiles and runs the simulation
#   make clean    - Removes generated files
#   make debug    - Compiles with debug symbols
#   make single   - Compiles in single competition mode (no GUI, fixed time)
#   make arena    - Compiles organizer tools (runner, score, etc.)


CC = gcc
CFLAGS = -Wall -Wextra -lm -Wno-unused-result
DEBUG_FLAGS = -g -O0
COMP_FLAGS = -DCOMPETITION_MODE=1

# Source files
SOURCES = main.c simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c
LIBSOURCES = simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c sim_world_api.c
LIBOBJECTS = simula.o
TARGET = roomba



# Default target
all: $(TARGET)

# Modo "Single": versión individual de competición (sin visualización)
single: $(SOURCES) simula.h
	$(CC) $(SOURCES) $(CFLAGS) $(COMP_FLAGS) -o $(TARGET)_comp
	@echo "Single-mode compiled: $(TARGET)_comp (Competition mode, no GUI)"

# Modo "Arena": herramientas del organizador (runner, score, etc.)
arena:
	@$(MAKE) -C competition
	@echo "Arena system compiled in competition/"

# Build standalone project
$(TARGET): $(SOURCES) simula.h
	$(CC) $(SOURCES) $(CFLAGS) -o $(TARGET)
	@echo "Roomba standalone project compiled successfully"

# Build with debug symbols
debug: $(SOURCES) simula.h
	$(CC) $(SOURCES) $(CFLAGS) $(DEBUG_FLAGS) -o $(TARGET)
	@echo "Debug build completed"


# Build library object file for distribution
lib: $(LIBOBJECTS)

$(LIBOBJECTS): $(LIBSOURCES) simula.h simula_internal.h sim_world_api.h
	$(CC) -c $(LIBSOURCES) $(CFLAGS)
	ld -r simula.o sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o sim_world_api.o -o simula_combined.o
	mv simula_combined.o simula.o
	rm -f sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o sim_world_api.o
	@echo "Library object file created: simula.o"

# Build library for competition mode (single object file)
lib-competition:
	@echo "Building competition library..."
	$(CC) -c simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c $(CFLAGS)
	$(CC) -r simula.o sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o -o competition/lib/simula.o -nostdlib
	rm -f simula.o sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o
	@echo "Competition library created: competition/lib/simula.o"

# Tools compilation
tools:
	@$(MAKE) -C tools

# Clean generated files
clean:
	rm -f $(TARGET) $(TARGET)_comp
	rm -f *.o *.csv *.pgm
	rm -f log.csv stats.csv map.pgm
	@$(MAKE) -C tools clean
	@$(MAKE) -C competition clean
	@echo "Cleaned build artifacts"

# Clean everything including generated maps
clean-all: clean
	rm -f maps/*.pgm
	@echo "Cleaned all generated content including maps"

# Documentation targets
doc:
	@$(MAKE) -C docs

doc-clean:
	@$(MAKE) -C docs clean

# Distribution target - Create standalone package
dist: lib
	@echo "Creating standalone distribution package..."
	@mkdir -p dist/maps
	@cp simula.o dist/
	@cp simula.h dist/
	@cp maps/*.pgm dist/maps/ 2>/dev/null || true
	@echo ""
	@echo "=========================================="
	@echo "  Standalone Package Created"
	@echo "=========================================="
	@echo "Location: dist/"
	@echo ""
	@echo "Contents:"
	@ls -lh dist/ | tail -n +2
	@echo ""
	@echo "To create distributable archive:"
	@echo "  tar -czf roomba-standalone.tar.gz dist/"
	@echo "  zip -r roomba-standalone.zip dist/"
	@echo ""
	@echo "To clean distribution package:"
	@echo "  make dist-clean"

dist-clean:
	@echo "Cleaning distribution package..."
	rm -rf dist/maps/*.pgm dist/simula.o dist/simula.h
	@echo "Distribution cleaned (keeping template files)"

# Help
help:
	@echo "Roomba Project - Available targets:"
	@echo ""
	@echo "Main targets:"
	@echo "  make         - Build the interactive simulation (default)"
	@echo "  make single  - Build standalone competition version (no GUI, fast)"
	@echo "  make arena   - Build organizer tools (runner, score, etc.)"
	@echo "  make tools   - Build utilities (generate, viewmap, validate)"
	@echo "  make clean   - Remove all generated files (binaries, logs, docs)"
	@echo "  make clean-all - Remove EVERYTHING (including generated maps)"
	@echo ""
	@echo "Run targets:"
	@echo "  make run     - Build and run simulation interactively"
	@echo "  make run-map MAP=path/to/map.pgm - Run with specific map"
	@echo ""
	@echo "Distribution (for organizers):"
	@echo "  make dist       - Create standalone package in dist/"
	@echo "  make dist-clean - Clean distribution package"
	@echo "  make lib        - Rebuild simula.o library"
	@echo ""
	@echo "Documentation:"
	@echo "  make doc        - Generate all documentation (API, User & Dev Manuals)"
	@echo "  make doc-clean  - Remove documentation artifacts"

.PHONY: all debug run run-map clean lib tools doc doc-api doc-user doc-developer doc-clean dist dist-clean help
