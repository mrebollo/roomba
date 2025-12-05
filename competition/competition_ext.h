/**
 * @file competition_ext.h
 * @brief Competition extensions for Roomba simulator
 * 
 * This module extends the base simulator with competition-specific features:
 * - Multi-map execution according to config.txt
 * - Team identification from current directory
 * - Centralized statistics with team/map information
 * - Automatic map rotation system
 * 
 * Architecture:
 * - simula.c: Core simulator (unchanged)
 * - competition_ext.c: Competition-specific logic
 * - simula_comp.c: Function overrides (run, save_stats)
 * 
 * Usage:
 *   gcc main.c simula.c competition_ext.c simula_comp.c -lm -o roomba
 * 
 * @author IPR-GIIROB-ETSINF-UPV
 * @date 2025
 */

#ifndef COMPETITION_EXT_H
#define COMPETITION_EXT_H

#include "../simula.h"

/* ============================================================================
 * COMPETITION CONFIGURATION
 * ============================================================================ */

#define COMP_MAPS_COUNT 4           ///< Number of different maps
#define COMP_REPS_PER_MAP 5         ///< Repetitions per map
#define COMP_STATS_FILE "../stats.csv"  ///< Centralized stats file

/* ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================ */

/**
 * @brief Initialize competition environment
 * 
 * Optional initialization function to configure paths.
 * If not called, uses default paths.
 * 
 * @param maps_dir Path to maps directory (NULL for default "../maps")
 * @param stats_file Path to stats file (NULL for default "../stats.csv")
 */
void competition_init(const char* maps_dir, const char* stats_file);

/**
 * @brief Competition version of run()
 * 
 * Executes robot behavior on maps sequentially according to config.txt.
 * Automatically manages map rotation and repetitions.
 * 
 * Flow:
 * 1. Read config.txt (rep, map_id)
 * 2. Load map[map_id]
 * 3. Execute robot behavior
 * 4. Update config.txt for next run
 * 
 * This function replaces the standard run() in competition mode.
 */
void run_competition(void);

/**
 * @brief Save statistics with team ID and map type
 * 
 * Enhanced version of save_stats() that includes:
 * - Team identification (from current directory name)
 * - Map type identifier
 * - Appends to centralized stats file (instead of overwriting)
 * 
 * Format: team,map_type,cell_total,cell_visited,...
 * 
 * This function replaces the standard save_stats() in competition mode.
 */
void save_stats_competition(void);

/**
 * @brief Get current team identifier
 * 
 * Extracts team name from current working directory.
 * Example: /path/to/competition/teams/team01 → "team01"
 * 
 * @return Team identifier string (static buffer)
 */
const char* competition_get_team_id(void);

/**
 * @brief Execute a single map in competition mode
 * 
 * Internal function used by run_competition().
 * Loads map, executes behavior, manages config rotation.
 * 
 * @param filename Path to map file
 * @param map_type Map type identifier (0-3)
 */
void competition_execute_map(const char* filename, int map_type);

#endif /* COMPETITION_EXT_H */
