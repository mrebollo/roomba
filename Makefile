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
SOURCES = main.c simula.c
TARGET = roomba

# Refactor preview
REFACT_SOURCES = main.c simula_refactor.c
REFACT_TARGET = roomba_refactor

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

# Build refactor preview
$(REFACT_TARGET): $(REFACT_SOURCES) simula.h
	$(CC) $(REFACT_SOURCES) $(CFLAGS) $(RELEASE_FLAGS) -o $(REFACT_TARGET)
	@echo "✓ Refactor preview compiled successfully"

# Run the simulation
run: $(TARGET)
	./$(TARGET)

# Run with a specific map
run-map: $(TARGET)
	./$(TARGET) $(MAP)

# Run refactor preview
run-refactor: $(REFACT_TARGET)
	./$(REFACT_TARGET)

# Clean generated files
clean:
	rm -f $(TARGET)
	rm -f $(REFACT_TARGET)
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

.PHONY: all debug run run-map run-refactor clean help
