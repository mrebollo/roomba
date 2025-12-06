# Makefile for Roomba Tools
#
# Compiles utility programs for map generation, visualization, and validation

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

# Paths
SIM_WORLD = ../sim_world.c

# Tools
GENERATE = generate_map
VIEWMAP = viewmap
VALIDATE = validate

.PHONY: all clean help

# Default target: build all tools
all: $(VALIDATE)
	@echo ""
	@echo "=== Tools compiled successfully ==="
	@echo ""
	@echo "Available tools:"
	@echo "  ./$(VALIDATE) <team_dir> [--output report.txt] [--strict]"
	@echo ""
	@echo "Note: Map generator and visualizer need simulator sources"
	@echo "      Use 'make generate_map' or 'make viewmap' if needed"
	@echo ""

# Code validator (standalone)
$(VALIDATE): validate.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✓ Code validator compiled: $(VALIDATE)"

# Validation script (just make it executable)
validate-sh:
	@if [ -f validate.sh ]; then \
		chmod +x validate.sh; \
		echo "✓ Validation script ready: validate.sh"; \
	else \
		echo "⚠ validate.sh not found"; \
	fi

# Clean compiled binaries
clean:
	rm -f $(GENERATE) $(VIEWMAP) $(VALIDATE)
	@echo "✓ Tools cleaned"

# Help
help:
	@echo "Roomba Tools Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Compile all tools"
	@echo "  make clean    - Remove compiled binaries"
	@echo "  make validate - Make validation script executable"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Tools:"
	@echo "  generate_map  - Generate random maps with walls and dirt"
	@echo "  viewmap       - Visualize PGM maps in terminal"
	@echo "  validate.sh   - Validate team code before competition"
