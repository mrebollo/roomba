/**
 * @file myscore.c
 * @brief Personal scoring tool for competition participants
 * 
 * Allows students to calculate their score based on local stats.csv file.
 * Uses the same scoring library as the organizer's tool for consistency.
 * 
 * Usage:
 *   ./myscore [stats.csv]
 * 
 * If no file is specified, uses "stats.csv" in current directory.
 * 
 * @author IPR-GIIROB-ETSINF-UPV
 * @date 2025
 */

#include "libscore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESULTS 100

/**
 * @brief Print usage information
 */
void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] [stats_file]\n", program_name);
    printf("\n");
    printf("Calculate your competition score based on local statistics.\n");
    printf("\n");
    printf("Options:\n");
    printf("  --config FILE    Use custom scoring config (default: ../scoring.conf)\n");
    printf("  --help           Show this help message\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  stats_file       Path to stats.csv (default: stats.csv)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s                    # Use stats.csv in current directory\n", program_name);
    printf("  %s my_stats.csv       # Use specific file\n", program_name);
    printf("  %s --config my.conf   # Use custom config\n", program_name);
    printf("\n");
}

/**
 * @brief Main function
 */
int main(int argc, char *argv[]) {
    const char *config_file = "../scoring.conf";
    const char *stats_file = "stats.csv";
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            config_file = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            // Assume it's the stats file
            stats_file = argv[i];
        }
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    ROOMBA COMPETITION - MY SCORE                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Load configuration
    scoring_config_t config;
    if (!scoring_load_config(config_file, &config)) {
        fprintf(stderr, "Warning: Using default configuration\n");
    }
    
    if (config.verbose) {
        printf("[INFO] Configuration loaded from %s\n", config_file);
        printf("[INFO] Weights: Coverage=%.0f%% DirtEff=%.0f%% Battery=%.0f%% Movement=%.0f%%\n",
               config.weight_coverage, config.weight_dirt_efficiency,
               config.weight_battery_conservation, config.weight_movement_quality);
    }
    
    // Load stats
    map_result_t results[MAX_RESULTS];
    int result_count = 0;
    
    if (!scoring_load_stats(stats_file, results, &result_count, MAX_RESULTS)) {
        fprintf(stderr, "Error: Cannot load statistics from %s\n", stats_file);
        fprintf(stderr, "\n");
        fprintf(stderr, "Make sure:\n");
        fprintf(stderr, "  1. You have run your robot at least once\n");
        fprintf(stderr, "  2. The stats.csv file exists in your team directory\n");
        fprintf(stderr, "  3. The file format is correct (run ./validate to check)\n");
        fprintf(stderr, "\n");
        return 1;
    }
    
    if (result_count == 0) {
        fprintf(stderr, "Error: No results found in %s\n", stats_file);
        fprintf(stderr, "The file is empty or has incorrect format.\n");
        return 1;
    }
    
    printf("[OK] Loaded %d execution(s) from %s\n", result_count, stats_file);
    printf("\n");
    
    // Calculate scores for each map
    for (int i = 0; i < result_count; i++) {
        scoring_calculate_map(&results[i], &config);
    }
    
    // Aggregate into team score
    team_score_t team_score;
    scoring_aggregate_team(results, result_count, &team_score, &config);
    
    // Display results
    scoring_display_team(&team_score, &config);
    
    // Additional tips based on scores
    printf("💡 Tips for improvement:\n");
    printf("\n");
    
    if (team_score.avg_coverage < 50.0f) {
        printf("  📍 Coverage is low (%.1f%%). Try:\n", team_score.avg_coverage);
        printf("     - Implement a systematic exploration strategy\n");
        printf("     - Use memory to avoid revisiting areas\n");
        printf("     - Consider spiral or zigzag patterns\n");
        printf("\n");
    }
    
    if (team_score.avg_dirt_efficiency < 1.0f) {
        printf("  🧹 Dirt efficiency is low (%.1f%%). Try:\n", team_score.avg_dirt_efficiency);
        printf("     - Clean more aggressively when dirt detected\n");
        printf("     - Balance exploration vs cleaning\n");
        printf("     - Prioritize dirty areas\n");
        printf("\n");
    }
    
    if (team_score.avg_battery_conservation < 40.0f) {
        printf("  🔋 Battery conservation is low (%.1f%%). Try:\n", team_score.avg_battery_conservation);
        printf("     - Reduce unnecessary movements\n");
        printf("     - Optimize pathfinding\n");
        printf("     - Return to base when battery is low\n");
        printf("\n");
    }
    
    if (team_score.avg_movement_quality < 80.0f) {
        printf("  🎯 Movement quality is low (%.1f%%). Try:\n", team_score.avg_movement_quality);
        printf("     - Implement obstacle memory\n");
        printf("     - Use sensor data to avoid known obstacles\n");
        printf("     - Plan paths before moving\n");
        printf("\n");
    }
    
    if (team_score.num_crashes > 0) {
        printf("  ⚠️  CRASHES DETECTED (%d)!\n", team_score.num_crashes);
        printf("     - Check for segmentation faults\n");
        printf("     - Verify array bounds\n");
        printf("     - Test with ./validate before running\n");
        printf("\n");
    }
    
    // Success criteria
    printf("🎯 Competition thresholds:\n");
    printf("   • Good score:      > 40 points\n");
    printf("   • Excellent score: > 60 points\n");
    printf("   • Top performer:   > 70 points\n");
    printf("\n");
    
    if (team_score.total_score >= 70.0f) {
        printf("🏆 Outstanding! You're on track for top rankings!\n");
    } else if (team_score.total_score >= 60.0f) {
        printf("⭐ Excellent work! Keep refining your strategy.\n");
    } else if (team_score.total_score >= 40.0f) {
        printf("✓ Good progress! Focus on the tips above.\n");
    } else {
        printf("💪 Keep working! Review the examples and tips.\n");
    }
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("Next steps:\n");
    printf("  1. Review the tips above\n");
    printf("  2. Modify your code in main.c\n");
    printf("  3. Test with: make run\n");
    printf("  4. Check score again: ./myscore\n");
    printf("  5. Iterate until you reach your target score!\n");
    printf("\n");
    
    return 0;
}
