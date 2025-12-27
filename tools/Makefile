# Makefile for Roomba Tools
#
# Compiles utility programs for map generation, visualization, and validation

CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-result
LDFLAGS = -lm


# Simulator world sources
SIM_WORLD = ../sim_world.c ../sim_world_api.c


# Executables: one per .c file

GENERATE = generate
VIEWMAP = viewmap
VALIDATE = validate
MYSCORE = myscore
VISUALIZE = visualize



.PHONY: all clean help visualize

all: $(GENERATE) $(VIEWMAP) $(VALIDATE) $(MYSCORE) $(VISUALIZE)
	@echo ""
	@echo "=== Tools compiled successfully ==="
	@echo ""
	@echo "Available tools:"
	@echo "  ./$(GENERATE) <options>"
	@echo "  ./$(VIEWMAP) <map.pgm>"
	@echo "  ./$(VALIDATE) [<team_dir>] [--output report.txt] [--strict]"
	@echo "  ./$(MYSCORE) [stats.csv]"
	@echo "  ./$(VISUALIZE) [logs.csv]"
	@echo ""



# Map generator
$(GENERATE): generate.c $(SIM_WORLD)
	$(CC) $(CFLAGS) -I.. -o $@ $^ $(LDFLAGS)
	@echo "Map generator compiled: $(GENERATE)"


# Map visualizer
$(VIEWMAP): viewmap.c $(SIM_WORLD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Map visualizer compiled: $(VIEWMAP)"

# Code validator
$(VALIDATE): validate.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Code validator compiled: $(VALIDATE)"

# Custom scoring system
$(MYSCORE): myscore.c libscore.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Custom score system compiled: $(MYSCORE)"

# Library object for shared use (e.g. by competition system)
libscore.o: libscore.c libscore.h
	$(CC) -c libscore.c $(CFLAGS)
	@echo "Scoring library object compiled: libscore.o"

# Log visualizer
$(VISUALIZE): visualize.c ../sim_visual.c

# Rule to generate local simula.o in tools/
SIMULA_SRC = ../simula.c ../sim_robot.c ../sim_visual.c ../sim_io.c ../sim_world.c ../sim_stats.c ../sim_world_api.c
SIMULA_OBJ = simula.o

$(SIMULA_OBJ): $(SIMULA_SRC)
	$(CC) -c $(SIMULA_SRC) $(CFLAGS)
	ld -r simula.o sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o sim_world_api.o -o simula_combined.o
	mv simula_combined.o simula.o
	rm -f sim_robot.o sim_visual.o sim_io.o sim_world.o sim_stats.o sim_world_api.o
	@echo "Local simula.o created in tools/"

$(VISUALIZE): visualize.c simula.o
	$(CC) $(CFLAGS) -I.. -o $@ visualize.c simula.o $(LDFLAGS)
	@echo "Log visualizer compiled: $(VISUALIZE)"

# Clean compiled binaries

clean:
	rm -f $(GENERATE) $(VIEWMAP) $(VALIDATE) $(MYSCORE) $(VISUALIZE) simula.o libscore.o
	@echo "Tools cleaned"

# Debug build for Tools with AddressSanitizer
tools-asan: CFLAGS += -fsanitize=address -g -O1 -fno-omit-frame-pointer
tools-asan: clean all

# Help
help:
	@echo "Roomba Tools Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make         - Compile all tools (generate, viewmap, validate, visualize)"
	@echo "  make clean   - Remove compiled binaries"
	@echo "  make help    - Show this help"
	@echo "  make <tool>  - Compile only the specified tool (e.g. 'make generate')"
	@echo ""
	@echo "Tools:"
	@echo "  generate     - Generate random maps with walls and dirt"
	@echo "  viewmap      - Visualize PGM maps in terminal"
	@echo "  validate     - Validate team code before competition"
	@echo "  myscore      - Custom scoring system for competition results"
	@echo "  visualize    - Visualize logs from logs.csv or other file"
