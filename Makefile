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
SOURCES = main.c simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c
TARGET = roomba


# Default target
all: $(TARGET)

# Build student project
$(TARGET): $(SOURCES) simula.h
	$(CC) $(SOURCES) $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET)
	@echo "✓ Student project compiled successfully"

# Build with debug symbols
debug: $(SOURCES) simula.h
	$(CC) $(SOURCES) $(CFLAGS) $(DEBUG_FLAGS) -o $(TARGET)
	@echo "✓ Debug build completed"


# Run the simulation
run: $(TARGET)
	./$(TARGET)

# Run with a specific map
run-map: $(TARGET)
	./$(TARGET) $(MAP)


# Clean generated files
clean:
	rm -f $(TARGET)
	rm -f *.csv *.pgm
	rm -f log.csv stats.csv map.pgm
	@echo "✓ Cleaned build artifacts"

# Help
help:
	@echo "Roomba Student Project - Available targets:"
	@echo "  make         - Build the project"
	@echo "  make run     - Build and run the simulation"
	@echo "  make debug   - Build with debug symbols"
	@echo "  make clean   - Remove generated files"
	@echo "  make run-map MAP=path/to/map.pgm - Run with specific map"

.PHONY: all debug run run-map clean help
