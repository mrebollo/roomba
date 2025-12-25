/**
 * @file libscore.h
 * @brief Competition scoring library - Shared API for scoring calculations
 * 
 * This library provides a common scoring implementation used by both:
 * - Organizers: Full competition scoring across all teams
 * - Participants: Individual score calculation for self-assessment
 * 
 * Benefits:
 * - Single source of truth for scoring formulas
 * - Consistency between testing and competition
 * - Easy configuration via scoring.conf
 * - Transparency for participants
 * 
 * @author IPR-GIIROB-ETSINF-UPV
 * @date 2025
 */

#ifndef LIBSCORE_H
#define LIBSCORE_H

#include <stdio.h>
#include <stdbool.h>

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */

#define MAX_MAPS 100              ///< Maximum maps per team
#define TEAM_NAME_LEN 64          ///< Maximum team name length

/* ============================================================================
 * SCORING CONFIGURATION
 * ============================================================================ */

/**
 * @brief Scoring configuration structure
 * 
 * Loaded from scoring.conf file with weights and bonuses.
 */
typedef struct {
    // Weights (should sum to 100)
    float weight_coverage;              ///< Coverage weight (%)
    float weight_dirt_efficiency;       ///< Dirt efficiency weight (%)
    float weight_battery_conservation;  ///< Battery conservation weight (%)
    float weight_movement_quality;      ///< Movement quality weight (%)
    
    // Bonuses
    float completion_bonus;             ///< Bonus for zero crashes
    float low_bumps_bonus;              ///< Bonus for movement > 90%
    float consistency_bonus;            ///< Bonus for high consistency
    float crash_penalty;                ///< Penalty per crash
    
    // Thresholds
    float movement_quality_threshold;   ///< Threshold for low bumps bonus (default 90%)
    float consistency_threshold;        ///< Threshold for consistency bonus (default 40)
    
    // Scaling factors
    float dirt_scale;                   ///< Scaling factor for dirt efficiency
    float battery_scale;                ///< Scaling factor for battery (1000 = max)
    
    // Output options
    bool generate_csv;                  ///< Generate CSV output
    bool verbose;                       ///< Verbose output
} scoring_config_t;

/* ============================================================================
 * DATA STRUCTURES
 * ============================================================================ */

/**
 * @brief Single map execution result
 */
typedef struct {
    char team_name[TEAM_NAME_LEN];  ///< Team identifier
    int map_type;                   ///< Map type (0-3)
    
    // Raw statistics from stats.csv
    int cell_total;                 ///< Total cells in map
    int cell_visited;               ///< Cells visited
    int dirt_total;                 ///< Total dirt in map
    int dirt_cleaned;               ///< Dirt cleaned
    float bat_total;                ///< Battery consumed
    float bat_mean;                 ///< Average battery remaining
    int forward;                    ///< Forward moves
    int turn;                       ///< Turn moves
    int bumps;                      ///< Bump count
    int clean;                      ///< Clean actions
    int load;                       ///< Load actions
    
    // Calculated scores (0-100 scale)
    float coverage;                 ///< Coverage percentage
    float dirt_efficiency;          ///< Dirt cleaning efficiency
    float battery_conservation;     ///< Battery conservation
    float movement_quality;         ///< Movement quality (low bumps)
} map_result_t;

/**
 * @brief Aggregated team score
 */
typedef struct {
    char name[TEAM_NAME_LEN];       ///< Team name
    
    // Aggregated scores
    float avg_coverage;             ///< Average coverage
    float avg_dirt_efficiency;      ///< Average dirt efficiency
    float avg_battery_conservation; ///< Average battery conservation
    float avg_movement_quality;     ///< Average movement quality
    
    // Final score
    float total_score;              ///< Weighted total score with bonuses
    float consistency_score;        ///< Consistency metric (1/stddev)
    
    // Metadata
    int num_maps;                   ///< Number of maps executed
    int num_crashes;                ///< Number of crashes
} team_score_t;

/* ============================================================================
 * CORE API FUNCTIONS
 * ============================================================================ */

/**
 * @brief Load scoring configuration from file
 * 
 * Reads scoring.conf (or custom file) and populates config structure.
 * Uses sensible defaults if file not found or values missing.
 * 
 * @param filename Configuration file path
 * @param config Output configuration structure
 * @return true on success, false on error
 */
bool scoring_load_config(const char *filename, scoring_config_t *config);

/**
 * @brief Calculate scores for a single map execution
 * 
 * Applies scoring formulas to raw statistics:
 * - Coverage: (visited/total) * 100
 * - Dirt efficiency: (cleaned/consumed) * 100 * scale
 * - Battery conservation: (bat_mean/1000) * 100
 * - Movement quality: (1 - bumps/moves) * 100
 * 
 * @param result Map result with raw stats (in) and calculated scores (out)
 * @param config Scoring configuration
 */
void scoring_calculate_map(map_result_t *result, const scoring_config_t *config);

/**
 * @brief Aggregate multiple map results into team score
 * 
 * Calculates weighted average of all metrics, applies bonuses/penalties:
 * - Completion bonus: +N if zero crashes
 * - Low bumps bonus: +N if movement > threshold
 * - Crash penalty: -N per crash
 * - Consistency: Based on standard deviation
 * 
 * @param results Array of map results for a team
 * @param count Number of results
 * @param team_score Output aggregated score
 * @param config Scoring configuration
 */
void scoring_aggregate_team(const map_result_t *results, int count, 
                            team_score_t *team_score, const scoring_config_t *config);

/**
 * @brief Load stats from CSV file
 * 
 * Parses stats.csv format (with or without team column).
 * Supports both competition format (team,map_type,...) and 
 * participant format (just stats without team).
 * 
 * @param filename CSV file path
 * @param results Output array of results
 * @param count Output count of results
 * @param max_results Maximum results to read
 * @return true on success, false on error
 */
bool scoring_load_stats(const char *filename, map_result_t *results, 
                       int *count, int max_results);

/**
 * @brief Display single team score summary
 * 
 * Pretty-print team score with breakdown of metrics.
 * Useful for participant feedback.
 * 
 * @param team Team score to display
 * @param config Scoring configuration
 */
void scoring_display_team(const team_score_t *team, const scoring_config_t *config);

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

/**
 * @brief Get default scoring configuration
 * 
 * Returns configuration with standard competition weights and bonuses.
 * 
 * @param config Output configuration with defaults
 */
void scoring_default_config(scoring_config_t *config);

/**
 * @brief Calculate standard deviation of scores
 * 
 * Used for consistency metric calculation.
 * 
 * @param values Array of values
 * @param count Number of values
 * @return Standard deviation
 */
float scoring_stddev(const float *values, int count);

#endif /* LIBSCORE_H */
