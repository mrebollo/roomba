#!/bin/bash
# setup.sh - Configure competition environment
#
# This script prepares the competition directory for running.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMP_DIR="$(dirname "$SCRIPT_DIR")"

echo "Setting up Roomba Competition environment..."

# Check if we're in the right directory
if [ ! -f "$COMP_DIR/runner.c" ]; then
    echo "Error: Must be run from competition/scripts/ directory"
    exit 1
fi

# Create necessary directories
echo "Creating directory structure..."
mkdir -p "$COMP_DIR/teams"
mkdir -p "$COMP_DIR/lib"
mkdir -p "$COMP_DIR/maps"
mkdir -p "$COMP_DIR/results"
mkdir -p "$COMP_DIR/scripts"

# Build competition library if it doesn't exist
if [ ! -f "$COMP_DIR/lib/simula.o" ]; then
    echo "Building competition library..."
    cd "$COMP_DIR/.."
    make lib-competition
    cd "$COMP_DIR"
else
    echo "Competition library already exists"
fi

# Sync headers
echo "Syncing headers from project root..."
cp "$COMP_DIR/../simula.h" "$COMP_DIR/lib/"
cp "$COMP_DIR/../simula_internal.h" "$COMP_DIR/lib/"

# Build runner
echo "Building competition runner..."
cd "$COMP_DIR"
make runner

# Validate teams
echo "Validating team directories..."
TEAM_COUNT=0
for team_dir in "$COMP_DIR/teams"/*; do
    if [ -d "$team_dir" ]; then
        TEAM_NAME=$(basename "$team_dir")
        if [ -f "$team_dir/main.c" ] || ls "$team_dir"/*.c >/dev/null 2>&1; then
            TEAM_COUNT=$((TEAM_COUNT + 1))
        else
            echo "  [!]  $TEAM_NAME: No .c file found"
        fi
    fi
done

echo ""
echo "[OK] Setup complete!"
echo "  Teams found: $TEAM_COUNT"
echo "  Library: lib/simula.o"
echo "  Runner: runner"
echo ""
echo "To run competition:"
echo "  cd $COMP_DIR"
echo "  make run"
