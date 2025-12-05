# Makefile for Roomba Competition System
#
# This Makefile manages the competition infrastructure, separate from
# the main development Makefile in the project root.

CC = gcc
CFLAGS = -Wall -Wextra -O2 -lm
RUNNER = runner
TEAMS_DIR = teams
LIB_DIR = lib
RESULTS_DIR = results

# Default target: build everything
all: runner check-lib

# Build competition runner
$(RUNNER): runner.c competition_ext.c simula_comp.c
	$(CC) runner.c $(CFLAGS) -o $(RUNNER)
	@echo "✓ Competition runner compiled successfully"

# Check if library exists, warn if not
check-lib:
	@if [ ! -f $(LIB_DIR)/simula.o ]; then \
		echo "⚠️  Warning: $(LIB_DIR)/simula.o not found"; \
		echo "   Run 'make lib' to build it"; \
	else \
		echo "✓ Competition library exists"; \
	fi

# Build the competition library from project root
lib:
	@echo "Building competition library from project root..."
	@cd .. && $(MAKE) lib-competition
	@echo "✓ Library built: $(LIB_DIR)/simula.o"

# Sync headers from project root
sync-headers:
	@echo "Syncing headers from project root..."
	@cp ../simula.h $(LIB_DIR)/
	@cp ../simula_internal.h $(LIB_DIR)/
	@echo "✓ Headers synced"

# Test compile a single team
test:
	@if [ -z "$(TEAM)" ]; then \
		echo "Error: Please specify TEAM=name"; \
		echo "Example: make test TEAM=team01"; \
		exit 1; \
	fi
	@echo "Testing compilation for team: $(TEAM)"
	@cd $(TEAMS_DIR)/$(TEAM) && \
		gcc -I../../lib main.c ../../lib/simula.o \
		../../competition_ext.c ../../simula_comp.c -lm -o roomba
	@echo "✓ Team $(TEAM) compiled successfully"

# Run the competition
run: $(RUNNER) check-lib
	@echo "Starting competition..."
	@mkdir -p $(RESULTS_DIR)
	./$(RUNNER) $(TEAMS_DIR)
	@echo "✓ Competition completed"

# Run competition and archive results with timestamp
run-archived: run
	@TIMESTAMP=$$(date +%Y%m%d_%H%M%S); \
	ARCHIVE_DIR=$(RESULTS_DIR)/run_$$TIMESTAMP; \
	mkdir -p $$ARCHIVE_DIR; \
	mv stats.csv $$ARCHIVE_DIR/ 2>/dev/null || true; \
	mv ranking.txt $$ARCHIVE_DIR/ 2>/dev/null || true; \
	ln -sf run_$$TIMESTAMP $(RESULTS_DIR)/latest; \
	echo "✓ Results archived to $$ARCHIVE_DIR"

# Initialize competition environment
init:
	@mkdir -p $(TEAMS_DIR) $(LIB_DIR) $(RESULTS_DIR) maps scripts
	@echo "✓ Competition environment initialized"

# Clean competition files
clean:
	rm -f $(RUNNER)
	rm -f $(TEAMS_DIR)/*/roomba
	rm -f $(TEAMS_DIR)/*/config.txt
	rm -f stats.csv ranking.txt
	@echo "✓ Cleaned competition artifacts"

# Deep clean: also remove library and results
clean-all: clean
	rm -f $(LIB_DIR)/simula.o
	rm -rf $(RESULTS_DIR)/*
	@echo "✓ Deep clean completed"

# Help
help:
	@echo "Roomba Competition System - Available targets:"
	@echo ""
	@echo "  make              - Build runner and check library"
	@echo "  make runner       - Build competition runner"
	@echo "  make lib          - Build competition library from root"
	@echo "  make sync-headers - Sync headers from project root"
	@echo "  make run          - Run full competition"
	@echo "  make run-archived - Run and archive results with timestamp"
	@echo "  make test TEAM=teamXX - Test compile a single team"
	@echo "  make init         - Initialize directory structure"
	@echo "  make clean        - Remove generated files"
	@echo "  make clean-all    - Deep clean (includes library)"
	@echo ""
	@echo "Directory structure:"
	@echo "  teams/     - Team source code (main.c)"
	@echo "  lib/       - Competition library (simula.o)"
	@echo "  maps/      - Official competition maps"
	@echo "  results/   - Competition results by timestamp"
	@echo "  scripts/   - Auxiliary scripts"

.PHONY: all runner check-lib lib sync-headers test run run-archived init clean clean-all help
