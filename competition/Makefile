# Makefile for Roomba Competition System
#
# This Makefile manages the competition infrastructure, separate from
# the main development Makefile in the project root.

CC = gcc
CFLAGS = -Wall -Wextra -lm -Wno-unused-result -I../tools
RUNNER = runner
TEAMS_DIR = teams
LIB_DIR = lib
RESULTS_DIR = results

# Default target: build everything
all: runner score myscore check-lib

# Build competition runner
$(RUNNER): runner.c competition_ext.c simula_comp.c
	$(CC) runner.c $(CFLAGS) -o $(RUNNER)
	@echo "[OK] Competition runner compiled successfully"

# Build scoring calculator (organizer tool)
score: score.c ../tools/libscore.o
	$(CC) score.c ../tools/libscore.o $(CFLAGS) -o score
	@echo "[OK] Scoring calculator compiled successfully"

# Build participant scoring tool (from tools/)
myscore:
	@$(MAKE) -C ../tools myscore
	@cp ../tools/myscore .
	@echo "[OK] Participant scoring tool copied from tools/"

# Build scoring library (in tools/)
# Explicit dependency on source files ensures 'make' checks timestamps correcty
../tools/libscore.o: ../tools/libscore.c ../tools/libscore.h
	@$(MAKE) -C ../tools libscore.o
	@echo "[OK] Scoring library verified in tools/"

# Check if library exists, warn if not
check-lib:
	@if [ ! -f $(LIB_DIR)/simula.o ]; then \
		echo "[OK] Warning: $(LIB_DIR)/simula.o not found"; \
		echo "   Run 'make lib' to build it"; \
	else \
		echo "[OK] Competition library exists"; \
	fi

# Build the competition library from project root
lib:
	@echo "Building competition library from project root..."
	@cd .. && $(MAKE) lib-competition
	@echo "[OK] Library built: $(LIB_DIR)/simula.o"
	@echo "Copying participant tools to lib/..."
	@cp ../tools/myscore $(LIB_DIR)/ 2>/dev/null || echo "Run 'make myscore' first"
	@cp ../tools/scoring.conf $(LIB_DIR)/ 2>/dev/null || echo "scoring.conf not found"
	@echo "[OK] Participant tools ready"

# Sync headers from project root
sync-headers:
	@echo "Syncing headers from project root..."
	@cp ../simula.h $(LIB_DIR)/
	@cp ../simula_internal.h $(LIB_DIR)/
	@echo "[OK] Headers synced"

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
	@echo "[OK] Team $(TEAM) compiled successfully"

# Run the competition
run: $(RUNNER) check-lib
	@echo "Starting competition..."
	@mkdir -p $(RESULTS_DIR)
	./$(RUNNER) $(TEAMS_DIR)
	@echo "[OK] Competition completed"

# Run competition and archive results with timestamp
run-archived: run
	@TIMESTAMP=$$(date +%Y%m%d_%H%M%S); \
	ARCHIVE_DIR=$(RESULTS_DIR)/run_$$TIMESTAMP; \
	mkdir -p $$ARCHIVE_DIR; \
	mv stats.csv $$ARCHIVE_DIR/ 2>/dev/null || true; \
	mv ranking.txt $$ARCHIVE_DIR/ 2>/dev/null || true; \
	ln -sf run_$$TIMESTAMP $(RESULTS_DIR)/latest; \
	echo "[OK] Results archived to $$ARCHIVE_DIR"

# Initialize competition environment
init:
	@mkdir -p $(TEAMS_DIR) $(LIB_DIR) $(RESULTS_DIR) maps scripts
	@$(MAKE) lib
	@$(MAKE) sync-headers
	@echo "Copying default maps from ../maps/..."
	@cp ../maps/*.pgm maps/ 2>/dev/null || echo "Warning: No maps found in ../maps/"
	@echo "[OK] Competition environment initialized"

# Clean competition files
clean:
	rm -f $(RUNNER) score myscore libscore.o
	rm -f $(TEAMS_DIR)/*/roomba
	rm -f $(TEAMS_DIR)/*/config.txt
	rm -f stats.csv ranking.txt
	@echo "[OK] Cleaned competition artifacts"

# Deep clean: also remove library and results
clean-all: clean
	rm -f $(LIB_DIR)/simula.o
	rm -rf $(RESULTS_DIR)/*
	@echo "[OK] Deep clean completed"

# Help
help:
	@echo "Roomba Competition System - Available targets:"
	@echo ""
	@echo "  make              - Build runner, score calculator and check library"
	@echo "  make runner       - Build competition runner"
	@echo "  make score        - Build scoring calculator (organizer)"
	@echo "  make myscore      - Build scoring tool (participants)"
	@echo "  make lib          - Build competition library from root"
	@echo "  make sync-headers - Sync headers from project root"
	@echo "  make run          - Run full competition"
	@echo "  make run-archived - Run and archive results with timestamp"
	@echo "  make test TEAM=teamXX - Test compile a single team"
	@echo "  make init         - Initialize directory structure"
	@echo "  make clean        - Remove generated files"
	@echo "  make clean-all    - Deep clean (includes library)"
	@echo ""
	@echo "Organizer workflow:"
	@echo "  1. make lib       - Build competition library (once)"
	@echo "  2. make           - Build runner and score calculator"
	@echo "  3. ./runner       - Run competition, generates stats.csv"
	@echo "  4. ./score        - Calculate rankings from stats.csv"
	@echo ""
	@echo "Participant workflow:"
	@echo "  1. Edit main.c    - Modify robot behavior"
	@echo "  2. make run       - Test your robot"
	@echo "  3. ./myscore      - Check your score"
	@echo "  4. Iterate!"
	@echo ""
	@echo "Directory structure:"
	@echo "  teams/     - Participant source code (main.c + stats.csv)"
	@echo "  lib/       - Competition library (simula.o)"
	@echo "  maps/      - Official competition maps"
	@echo "  results/   - Generated rankings and scores"
	@echo "  logs/      - Execution logs per team/map/run"
	@echo "  results/   - Competition results by timestamp"
	@echo "  scripts/   - Auxiliary scripts"

.PHONY: all runner check-lib lib sync-headers test run run-archived init clean clean-all help
