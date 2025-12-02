# Makefile for Roomba Student Projects
# 
# This Makefile compiles the student's robot behavior (main.c)
# with the simulation library (simula.c and helpers) for development and testing.
# It now builds a single simula.o that contains the full simulator and links it
# with the student's main.

CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-result -lm
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2

# Source files
SIM_SRCS = simula.c sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c
# We'll compile simula.c separately into simula_src.o to avoid a circular dependency
SIM_HELPER_SRCS = sim_robot.c sim_visual.c sim_io.c sim_world.c sim_stats.c
SIM_HELPER_OBJS = $(SIM_HELPER_SRCS:.c=.o)
SIMULA_SRC_OBJ = simula_src.o
SIM_OBJS = $(SIM_HELPER_OBJS) $(SIMULA_SRC_OBJ)
STUDENT_SRCS = main.c
TARGET = roomba


# Default target
all: $(TARGET)

# Link final executable: only main.o + simula.o
$(TARGET): main.o simula.o
	$(CC) main.o simula.o $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET)
	@echo "✓ Student project compiled successfully"

# Compile main with header dependency
main.o: main.c simula.h
	$(CC) -c main.c $(CFLAGS) $(RELEASE_FLAGS) -o main.o

# Build a single relocatable object simula.o that contains the whole simulator
# Compile simula.c into simula_src.o, compile helpers into their .o files, then merge
$(SIMULA_SRC_OBJ): simula.c simula.h
	$(CC) -c simula.c $(CFLAGS) $(RELEASE_FLAGS) -o $(SIMULA_SRC_OBJ)

simula.o: $(SIM_OBJS)
	$(CC) -r $(SIM_OBJS) -o simula.o
	@echo "✓ simula.o created (contains: $(SIM_SRCS))"

# Target to only build the combined simulator object
simula: simula.o
	@echo "✓ simula.o is up to date (generated)"

# Generic rule to compile .c -> .o
%.o: %.c simula.h
	$(CC) -c $< $(CFLAGS) $(RELEASE_FLAGS) -o $@

# Build with debug symbols
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)
	@echo "✓ Debug build completed"

# Run the simulation
run: $(TARGET)
	./$(TARGET)

# Run with a specific map
run-map: $(TARGET)
	./$(TARGET) $(MAP)


# Clean generated files
clean:
	rm -f $(TARGET) *.csv *.pgm log.csv stats.csv map.pgm
	rm -f *.o simula.o
	@echo "✓ Cleaned build artifacts"

# Help
help:
	@echo "Roomba Student Project - Available targets:"
	@echo "  make         - Build the project"
	@echo "  make run     - Build and run the simulation"
	@echo "  make debug   - Build with debug symbols"
	@echo "  make clean   - Remove generated files"
	@echo "  make run-map MAP=path/to/map.pgm - Run with specific map"

.PHONY: all debug run run-map clean help simula
