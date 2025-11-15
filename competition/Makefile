# Makefile for Roomba Competition
#
# This Makefile compiles and runs the competition framework.
# It does NOT compile individual team projects - the runner.c does that.
#
# Usage:
#   make runner   - Build the competition runner
#   make test     - Test compile a single team (use TEAM=name)
#   make run      - Run the full competition
#   make clean    - Remove generated files

CC = gcc
CFLAGS = -Wall -Wextra -lm
RUNNER = runner
TEAMS_DIR = ../teams

# Default target
all: $(RUNNER)

# Build competition runner
$(RUNNER): runner.c
	$(CC) runner.c $(CFLAGS) -o $(RUNNER)
	@echo "✓ Competition runner compiled successfully"

# Test compile a single team
test:
	@if [ -z "$(TEAM)" ]; then \
		echo "Error: Please specify TEAM=name"; \
		echo "Example: make test TEAM=team1"; \
		exit 1; \
	fi
	@echo "Testing compilation for team: $(TEAM)"
	@cd $(TEAMS_DIR)/$(TEAM) && \
		gcc -DCOMPETITION_MODE -Wall -Wno-unused-function \
		main.c ../../competition/simula_comp.c -lm -o roomba
	@echo "✓ Team $(TEAM) compiled successfully"

# Run the competition
run: $(RUNNER)
	@echo "Starting competition..."
	./$(RUNNER) $(TEAMS_DIR)

# Initialize competition environment
init:
	@mkdir -p $(TEAMS_DIR)
	@echo "0 0" > config.txt
	@echo "✓ Competition environment initialized"

# Clean competition files
clean:
	rm -f $(RUNNER)
	rm -f stats.csv ranking.txt
	rm -f config.txt
	@echo "✓ Cleaned competition artifacts"

# Clean all team builds
clean-teams:
	@for team in $(TEAMS_DIR)/*; do \
		if [ -d "$$team" ]; then \
			rm -f $$team/roomba $$team/*.csv $$team/config.txt; \
		fi \
	done
	@echo "✓ Cleaned all team builds"

# Full clean
clean-all: clean clean-teams

# Help
help:
	@echo "Roomba Competition - Available targets:"
	@echo "  make runner      - Build the competition runner"
	@echo "  make test TEAM=name - Test compile a specific team"
	@echo "  make run         - Run the full competition"
	@echo "  make init        - Initialize competition environment"
	@echo "  make clean       - Remove competition files"
	@echo "  make clean-teams - Remove all team builds"
	@echo "  make clean-all   - Full cleanup"

.PHONY: all test run init clean clean-teams clean-all help
