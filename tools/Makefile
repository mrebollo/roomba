# Makefile for Roomba Tools
#
# Compiles utility programs for map generation, visualization, and validation

CC = gcc
CFLAGS = -Wall -Wextra -O2 -Wno-unused-result
LDFLAGS = -lm


# Fuentes del mundo del simulador
SIM_WORLD = ../sim_world.c ../sim_world_api.c


# Ejecutables: uno por cada .c
GENERATE = generate
VIEWMAP = viewmap
VALIDATE = validate
MYSCORE = myscore


.PHONY: all clean help

# Default target: build all tools
all: $(GENERATE) $(VIEWMAP) $(VALIDATE) $(MYSCORE)
	@echo ""
	@echo "=== Tools compiled successfully ==="
	@echo ""
	@echo "Available tools:"
	@echo "  ./$(GENERATE) <options>"
	@echo "  ./$(VIEWMAP) <map.pgm>"
	@echo "  ./$(VALIDATE) [<team_dir>] [--output report.txt] [--strict]"
	@echo "  ./$(MYSCORE) [stats.csv]"
	@echo ""



# Generador de mapas
$(GENERATE): generate.c $(SIM_WORLD)
	$(CC) $(CFLAGS) -I.. -o $@ $^ $(LDFLAGS)
	@echo "✓ Map generator compiled: $(GENERATE)"


# Visualizador de mapas
$(VIEWMAP): viewmap.c $(SIM_WORLD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Map visualizer compiled: $(VIEWMAP)"

# Validador de código
$(VALIDATE): validate.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✓ Code validator compiled: $(VALIDATE)"

# Mi sistema de puntuacion
$(MYSCORE): myscore.c libscore.c libscore.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Custom score system compiled: $(MYSCORE)"

# Clean compiled binaries

clean:
	rm -f $(GENERATE) $(VIEWMAP) $(VALIDATE) $(MYSCORE)	
	@echo "✓ Tools cleaned"

# Help
help:
	@echo "Roomba Tools Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make         - Compile all tools (generate, viewmap, validate)"
	@echo "  make clean   - Remove compiled binaries"
	@echo "  make help    - Show this help"
	@echo "  make <tool>  - Compile only the specified tool (e.g. 'make generate')"
	@echo ""
	@echo "Tools:"
	@echo "  generate     - Generate random maps with walls and dirt"
	@echo "  viewmap      - Visualize PGM maps in terminal"
	@echo "  validate     - Validate team code before competition"
	@echo "  myscore      - Custom scoring system for competition results"
