# Makefile for Roomba Student Projects
# 
# This Makefile compiles the student's robot behavior (main.c)
# with the simulation library (simula.c) for development and testing.
#
# Usage:
#   make          - Build the student project
#   make run      - Build and run the simulation
#   make clean    - Remove generated files
#   make debug    - Build with debug symbols

CC = gcc
CFLAGS = -Wall -Wextra -lm
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2

# Source files
SOURCES = main.c simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c
LIBSOURCES = simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c sim_world_api.c
LIBOBJECTS = simula.o
TARGET = roomba
GENERATE = maps/generate
VIEWMAP = maps/viewmap


# Default target
all: $(TARGET)

# Build student project
$(TARGET): $(SOURCES) simula.h
	$(CC) $(SOURCES) $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET)
	@echo "Student project compiled successfully"

# Build with debug symbols
debug: $(SOURCES) simula.h
	$(CC) $(SOURCES) $(CFLAGS) $(DEBUG_FLAGS) -o $(TARGET)
	@echo "Debug build completed"


# Build library object file for distribution to students
lib: $(LIBOBJECTS)

$(LIBOBJECTS): $(LIBSOURCES) simula.h simula_internal.h sim_world_api.h
	$(CC) -c $(LIBSOURCES) -Wall -Wextra -O2
	ld -r simula.o sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o sim_world_api.o -o simula_combined.o
	mv simula_combined.o simula.o
	rm -f sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o sim_world_api.o
	@echo "Library object file created: simula.o"

# Build library for competition mode (single object file)
lib-competition:
	@echo "Building competition library..."
	$(CC) -c simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c -Wall -Wextra -O2
	$(CC) -r simula.o sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o -o competition/lib/simula.o -nostdlib
	rm -f simula.o sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o
	@echo "Competition library created: competition/lib/simula.o"

# Map generator (development mode - from sources)
mapgen-dev: maps/generate.c sim_world_api.c
	$(CC) maps/generate.c sim_world_api.c $(LIBSOURCES:sim_world_api.c=) $(CFLAGS) $(RELEASE_FLAGS) -o $(GENERATE)
	@echo "Map generator compiled (development mode)"

# Map generator (distribution mode - with simula.o)
mapgen: $(LIBOBJECTS) maps/generate.c sim_world_api.h
	$(CC) maps/generate.c simula.o $(CFLAGS) $(RELEASE_FLAGS) -o $(GENERATE)
	@echo "Map generator compiled (distribution mode)"

# Map viewer (development mode - from sources)
viewmap-dev: maps/viewmap.c sim_world_api.c
	$(CC) maps/viewmap.c sim_world_api.c $(LIBSOURCES:sim_world_api.c=) $(CFLAGS) $(RELEASE_FLAGS) -o $(VIEWMAP)
	@echo "Map viewer compiled (development mode)"

# Map viewer (distribution mode - with simula.o)
viewmap: $(LIBOBJECTS) maps/viewmap.c sim_world_api.h
	$(CC) maps/viewmap.c simula.o $(CFLAGS) $(RELEASE_FLAGS) -o $(VIEWMAP)
	@echo "Map viewer compiled (distribution mode)"


# Run the simulation
run: $(TARGET)
	./$(TARGET)

# Run with a specific map
run-map: $(TARGET)
	./$(TARGET) $(MAP)


# Clean generated files
clean:
	rm -f $(TARGET) $(GENERATE) $(VIEWMAP)
	rm -f *.o *.csv *.pgm
	rm -f log.csv stats.csv map.pgm
	@echo "Cleaned build artifacts"

# Documentation targets
doc: doc-api doc-user doc-developer
	@echo "All documentation generated successfully"

doc-api:
	@echo "Generating API documentation (Doxygen)..."
	@if [ -f docs/Doxyfile ]; then \
		cd docs && doxygen Doxyfile > /dev/null 2>&1; \
		echo "API documentation generated in docs/html/"; \
	else \
		echo "Warning: Doxyfile not found in docs/"; \
	fi

doc-user:
	@echo "Generating User Manual PDF..."
	@if [ -f docs/usuario/manual_usuario.tex ]; then \
		cd docs/usuario && pdflatex manual_usuario.tex > /dev/null 2>&1 && \
		pdflatex manual_usuario.tex > /dev/null 2>&1 && \
		rm -f manual_usuario.aux manual_usuario.log manual_usuario.out && \
		echo "User manual generated: docs/usuario/manual_usuario.pdf"; \
	else \
		echo "Warning: User manual .tex file not found"; \
	fi

doc-developer:
	@echo "Generating Developer Manual PDF..."
	@if [ -f docs/developer/manual_desarrollador.tex ]; then \
		cd docs/developer && pdflatex manual_desarrollador.tex > /dev/null 2>&1 && \
		pdflatex manual_desarrollador.tex > /dev/null 2>&1 && \
		rm -f manual_desarrollador.aux manual_desarrollador.log manual_desarrollador.out && \
		echo "Developer manual generated: docs/developer/manual_desarrollador.pdf"; \
	else \
		echo "Warning: Developer manual .tex file not found"; \
	fi

doc-clean:
	@echo "Cleaning documentation artifacts..."
	rm -rf docs/html/
	rm -f docs/usuario/*.aux docs/usuario/*.log docs/usuario/*.out docs/usuario/*.toc
	rm -f docs/usuario/*.fdb_latexmk docs/usuario/*.fls docs/usuario/*.synctex.gz
	rm -f docs/usuario/*.bbl docs/usuario/*.blg docs/usuario/*.idx docs/usuario/*.ilg docs/usuario/*.ind
	rm -f docs/usuario/*.lof docs/usuario/*.lot
	rm -f docs/developer/*.aux docs/developer/*.log docs/developer/*.out docs/developer/*.toc
	rm -f docs/developer/*.fdb_latexmk docs/developer/*.fls docs/developer/*.synctex.gz
	rm -f docs/developer/*.bbl docs/developer/*.blg docs/developer/*.idx docs/developer/*.ilg docs/developer/*.ind
	rm -f docs/developer/*.lof docs/developer/*.lot
	rm -f docs/usuario/manual_usuario.pdf
	rm -f docs/developer/manual_desarrollador.pdf
	@echo "Documentation artifacts cleaned"

# Distribution target - Create student package
dist: lib
	@echo "Creating student distribution package..."
	@mkdir -p dist/maps
	@cp simula.o dist/
	@cp simula.h dist/
	@cp maps/*.pgm dist/maps/ 2>/dev/null || true
	@echo ""
	@echo "=========================================="
	@echo "  Student Package Created"
	@echo "=========================================="
	@echo "Location: dist/"
	@echo ""
	@echo "Contents:"
	@ls -lh dist/ | tail -n +2
	@echo ""
	@echo "To create distributable archive:"
	@echo "  tar -czf roomba-student.tar.gz dist/"
	@echo "  zip -r roomba-student.zip dist/"
	@echo ""

dist-clean:
	@echo "Cleaning distribution package..."
	rm -rf dist/maps/*.pgm dist/simula.o dist/simula.h
	@echo "Distribution cleaned (keeping template files)"

# Help
help:
	@echo "Roomba Student Project - Available targets:"
	@echo ""
	@echo "Build targets:"
	@echo "  make         - Build the project"
	@echo "  make debug   - Build with debug symbols"
	@echo "  make clean   - Remove generated files"
	@echo "  make lib     - Create simula.o library for distribution"
	@echo ""
	@echo "Map generator:"
	@echo "  make mapgen-dev  - Build map generator (development, from sources)"
	@echo "  make mapgen      - Build map generator (with simula.o library)"
	@echo ""
	@echo "Map viewer:"
	@echo "  make viewmap-dev - Build map viewer (development, from sources)"
	@echo "  make viewmap     - Build map viewer (with simula.o library)"
	@echo ""
	@echo "Run targets:"
	@echo "  make run     - Build and run the simulation"
	@echo "  make run-map MAP=path/to/map.pgm - Run with specific map"
	@echo ""
	@echo "Documentation targets:"
	@echo "  make doc           - Generate all documentation (API + manuals)"
	@echo "  make doc-api       - Generate API documentation (Doxygen)"
	@echo "  make doc-user      - Generate user manual PDF"
	@echo "  make doc-developer - Generate developer manual PDF"
	@echo "  make doc-clean     - Remove all generated documentation"
	@echo ""
	@echo "Distribution targets:"
	@echo "  make dist       - Create student package in dist/ (includes simula.o)"
	@echo "  make dist-clean - Clean distribution files (keep templates)"

.PHONY: all debug run run-map clean lib mapgen mapgen-dev viewmap viewmap-dev doc doc-api doc-user doc-developer doc-clean dist dist-clean help
