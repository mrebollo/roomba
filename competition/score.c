/**
 * @file calculate_scores.c
 * @brief Configurable scoring system for Roomba competition
 * 
 * Reads stats.csv and scoring.conf to calculate team rankings
 * with customizable weights and formulas.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_TEAMS 100
#define MAX_MAPS 50
#define MAX_LINE 1024
#define MAX_NAME 64

// Configuration structure
typedef struct {
    float weight_coverage;
    float weight_dirt_efficiency;
    float weight_battery_conservation;
    float weight_movement_quality;
    float completion_bonus;
    float low_bumps_bonus;
    float crash_penalty;
    float low_bumps_threshold;
    float min_battery_threshold;
    int show_map_details;
    int highlight_top;
    int generate_csv;
} config_t;

// Map result for a single execution
typedef struct {
    char team_name[MAX_NAME];
    int map_type;
    int cell_total;
    int cell_visited;
    int dirt_total;
    int dirt_cleaned;
    float bat_total;
    float bat_mean;
    int forward;
    int turn;
    int bumps;
    int clean;
    int load;
    
    // Calculated scores
    float score_coverage;
    float score_dirt_efficiency;
    float score_battery_conservation;
    float score_movement_quality;
} map_result_t;

// Team aggregated results
typedef struct {
    char name[MAX_NAME];
    int num_maps;
    int num_crashes;
    
    // Per-map scores
    float coverage_scores[MAX_MAPS];
    float dirt_efficiency_scores[MAX_MAPS];
    float battery_conservation_scores[MAX_MAPS];
    float movement_quality_scores[MAX_MAPS];
    
    // Aggregated metrics
    float avg_coverage;
    float avg_dirt_efficiency;
    float avg_battery_conservation;
    float avg_movement_quality;
    float consistency_score;
    
    // Movement stats for bonuses
    float avg_bumps;
    float avg_moves;
    
    // Final score
    float total_score;
} team_score_t;

/**
 * @brief Trim whitespace from string
 */
void trim(char *str) {
    char *start = str;
    while(isspace(*start)) start++;
    
    char *end = start + strlen(start) - 1;
    while(end > start && isspace(*end)) end--;
    *(end + 1) = '\0';
    
    memmove(str, start, strlen(start) + 1);
}

/**
 * @brief Parse configuration file
 */
int load_config(const char *filename, config_t *cfg) {
    // Default values
    cfg->weight_coverage = 30.0;
    cfg->weight_dirt_efficiency = 35.0;
    cfg->weight_battery_conservation = 20.0;
    cfg->weight_movement_quality = 15.0;
    cfg->completion_bonus = 5.0;
    cfg->low_bumps_bonus = 3.0;
    cfg->crash_penalty = 10.0;
    cfg->low_bumps_threshold = 0.10;
    cfg->min_battery_threshold = 200.0;
    cfg->show_map_details = 1;
    cfg->highlight_top = 3;
    cfg->generate_csv = 1;
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Warning: Could not open %s, using defaults\n", filename);
        return 0;
    }
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\0' || line[0] == '[') continue;
        
        // Parse key = value
        char *eq = strchr(line, '=');
        if (!eq) continue;
        
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        trim(key);
        trim(value);
        
        // Remove inline comments from value
        char *comment = strchr(value, '#');
        if (comment) {
            *comment = '\0';
            trim(value);
        }
        
        // Parse weights
        if (strcmp(key, "coverage") == 0) cfg->weight_coverage = atof(value);
        else if (strcmp(key, "dirt_efficiency") == 0) cfg->weight_dirt_efficiency = atof(value);
        else if (strcmp(key, "battery_conservation") == 0) cfg->weight_battery_conservation = atof(value);
        else if (strcmp(key, "movement_quality") == 0) cfg->weight_movement_quality = atof(value);
        
        // Parse bonuses
        else if (strcmp(key, "completion_bonus") == 0) cfg->completion_bonus = atof(value);
        else if (strcmp(key, "low_bumps_bonus") == 0) cfg->low_bumps_bonus = atof(value);
        else if (strcmp(key, "crash_penalty") == 0) cfg->crash_penalty = atof(value);
        
        // Parse thresholds
        else if (strcmp(key, "low_bumps_threshold") == 0) cfg->low_bumps_threshold = atof(value);
        else if (strcmp(key, "min_battery_threshold") == 0) cfg->min_battery_threshold = atof(value);
        
        // Parse display options
        else if (strcmp(key, "show_map_details") == 0) cfg->show_map_details = (strcmp(value, "yes") == 0);
        else if (strcmp(key, "highlight_top") == 0) cfg->highlight_top = atoi(value);
        else if (strcmp(key, "generate_csv") == 0) cfg->generate_csv = (strcmp(value, "yes") == 0);
    }
    
    fclose(f);
    return 1;
}

/**
 * @brief Calculate scores for a single map result
 */
void calculate_map_scores(map_result_t *result, config_t *cfg) {
    (void)cfg; // Unused for now, formulas are fixed
    
    // Coverage score (0-100): percentage of cells visited
    result->score_coverage = (result->cell_visited * 100.0) / (result->cell_total > 0 ? result->cell_total : 1);
    
    // Dirt efficiency (0-100): dirt cleaned per battery consumed
    result->score_dirt_efficiency = 0;
    if (result->bat_total > 0.1) {
        result->score_dirt_efficiency = (result->dirt_cleaned * 100.0) / result->bat_total;
        // Cap at 100 for normalization
        if (result->score_dirt_efficiency > 100) result->score_dirt_efficiency = 100;
    }
    
    // Battery conservation (0-100): percentage of battery remaining on average
    result->score_battery_conservation = (result->bat_mean * 100.0) / 1000.0;
    if (result->score_battery_conservation > 100) result->score_battery_conservation = 100;
    
    // Movement quality (0-100): proportion of movements without bumps
    int total_moves = result->forward + result->turn + result->bumps;
    if (total_moves > 0) {
        float bump_ratio = (float)result->bumps / total_moves;
        result->score_movement_quality = (1.0 - bump_ratio) * 100.0;
    } else {
        result->score_movement_quality = 0;
    }
}

/**
 * @brief Calculate standard deviation
 */
float calculate_std_dev(float values[], int count) {
    if (count <= 1) return 0.0;
    
    float sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    float mean = sum / count;
    
    float variance = 0.0;
    for (int i = 0; i < count; i++) {
        float diff = values[i] - mean;
        variance += diff * diff;
    }
    variance /= count;
    
    return sqrt(variance);
}

/**
 * @brief Aggregate team results from all maps
 */
void aggregate_team_scores(team_score_t *team, config_t *cfg) {
    if (team->num_maps == 0) {
        team->total_score = 0;
        return;
    }
    
    // Calculate averages
    float sum_coverage = 0, sum_dirt_eff = 0, sum_battery_cons = 0, sum_movement_qual = 0;
    for (int i = 0; i < team->num_maps; i++) {
        sum_coverage += team->coverage_scores[i];
        sum_dirt_eff += team->dirt_efficiency_scores[i];
        sum_battery_cons += team->battery_conservation_scores[i];
        sum_movement_qual += team->movement_quality_scores[i];
    }
    
    team->avg_coverage = sum_coverage / team->num_maps;
    team->avg_dirt_efficiency = sum_dirt_eff / team->num_maps;
    team->avg_battery_conservation = sum_battery_cons / team->num_maps;
    team->avg_movement_quality = sum_movement_qual / team->num_maps;
    
    // Calculate consistency (inverse of std deviation across all criteria)
    float all_scores[MAX_MAPS * 4];
    int score_count = 0;
    for (int i = 0; i < team->num_maps; i++) {
        all_scores[score_count++] = team->coverage_scores[i];
        all_scores[score_count++] = team->dirt_efficiency_scores[i];
        all_scores[score_count++] = team->battery_conservation_scores[i];
        all_scores[score_count++] = team->movement_quality_scores[i];
    }
    
    float std_dev = calculate_std_dev(all_scores, score_count);
    team->consistency_score = 100.0 - (std_dev > 100.0 ? 100.0 : std_dev);
    
    // Calculate weighted final score (base score without bonuses)
    team->total_score = 
        (team->avg_coverage * cfg->weight_coverage / 100.0) +
        (team->avg_dirt_efficiency * cfg->weight_dirt_efficiency / 100.0) +
        (team->avg_battery_conservation * cfg->weight_battery_conservation / 100.0) +
        (team->avg_movement_quality * cfg->weight_movement_quality / 100.0);
    
    // Apply crash penalty
    team->total_score -= (team->num_crashes * cfg->crash_penalty);
    
    // Apply completion bonus (no crashes)
    if (team->num_crashes == 0) {
        team->total_score += cfg->completion_bonus;
    }
    
    // Apply low bumps bonus
    if (team->avg_moves > 0) {
        float bump_ratio = team->avg_bumps / team->avg_moves;
        if (bump_ratio < cfg->low_bumps_threshold) {
            team->total_score += cfg->low_bumps_bonus;
        }
    }
    
    // Ensure score is not negative
    if (team->total_score < 0) team->total_score = 0;
}

/**
 * @brief Load stats from CSV file
 */
int load_stats(const char *filename, map_result_t results[], int *count, config_t *cfg) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open %s\n", filename);
        return 0;
    }
    
    char line[MAX_LINE];
    
    // Skip header
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return 0;
    }
    
    *count = 0;
    while (fgets(line, sizeof(line), f) && *count < MAX_TEAMS * MAX_MAPS) {
        map_result_t *r = &results[*count];
        
        int parsed = sscanf(line, "%[^,],%d,%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d",
            r->team_name, &r->map_type,
            &r->cell_total, &r->cell_visited,
            &r->dirt_total, &r->dirt_cleaned,
            &r->bat_total, &r->bat_mean,
            &r->forward, &r->turn, &r->bumps, &r->clean, &r->load);
        
        if (parsed >= 8) {  // At least basic fields
            calculate_map_scores(r, cfg);
            (*count)++;
        }
    }
    
    fclose(f);
    return 1;
}

/**
 * @brief Aggregate results by team
 */
int aggregate_by_team(map_result_t results[], int result_count, team_score_t teams[], config_t *cfg) {
    int team_count = 0;
    
    for (int i = 0; i < result_count; i++) {
        map_result_t *r = &results[i];
        
        // Find or create team
        int team_idx = -1;
        for (int j = 0; j < team_count; j++) {
            if (strcmp(teams[j].name, r->team_name) == 0) {
                team_idx = j;
                break;
            }
        }
        
        if (team_idx == -1) {
            team_idx = team_count++;
            strcpy(teams[team_idx].name, r->team_name);
            teams[team_idx].num_maps = 0;
            teams[team_idx].num_crashes = 0;
        }
        
        team_score_t *team = &teams[team_idx];
        int map_idx = team->num_maps;
        
        if (map_idx < MAX_MAPS) {
            team->coverage_scores[map_idx] = r->score_coverage;
            team->dirt_efficiency_scores[map_idx] = r->score_dirt_efficiency;
            team->battery_conservation_scores[map_idx] = r->score_battery_conservation;
            team->movement_quality_scores[map_idx] = r->score_movement_quality;
            team->num_maps++;
            
            // Accumulate movement stats for bonuses
            team->avg_bumps += r->bumps;
            team->avg_moves += (r->forward + r->turn + r->bumps);
            
            // Detect crashes (very low scores combined with very few moves)
            if (r->score_coverage < 1.0 && r->cell_visited < 5) {
                team->num_crashes++;
            }
        }
    }
    
    // Calculate final averages for movement stats
    for (int i = 0; i < team_count; i++) {
        if (teams[i].num_maps > 0) {
            teams[i].avg_bumps /= teams[i].num_maps;
            teams[i].avg_moves /= teams[i].num_maps;
        }
    }
    
    // Calculate final scores
    for (int i = 0; i < team_count; i++) {
        aggregate_team_scores(&teams[i], cfg);
    }
    
    return team_count;
}

/**
 * @brief Comparison function for sorting teams
 */
int compare_teams(const void *a, const void *b) {
    team_score_t *ta = (team_score_t *)a;
    team_score_t *tb = (team_score_t *)b;
    
    if (tb->total_score > ta->total_score) return 1;
    if (tb->total_score < ta->total_score) return -1;
    return 0;
}

/**
 * @brief Display ranking to console
 */
void display_ranking(team_score_t teams[], int team_count, config_t *cfg) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════\n");
    printf("                        ROOMBA COMPETITION RANKING                          \n");
    printf("═══════════════════════════════════════════════════════════════════════════\n");
    printf("Rank  Team            Score    Cover  DirtEff  BatCons  Movement  Maps\n");
    printf("───────────────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < team_count; i++) {
        team_score_t *t = &teams[i];
        
        char marker = ' ';
        if (i < cfg->highlight_top) {
            marker = (i == 0) ? '*' : '+';
        }
        
        printf("%c %-3d %-15s %6.2f   %5.1f  %6.1f   %6.1f   %6.1f      %d\n",
            marker, i + 1, t->name, t->total_score,
            t->avg_coverage, t->avg_dirt_efficiency, t->avg_battery_conservation,
            t->avg_movement_quality, t->num_maps);
        
        if (t->num_crashes > 0) {
            printf("      └─ [!] %d crash(es) detected\n", t->num_crashes);
        }
    }
    
    printf("═══════════════════════════════════════════════════════════════════════════\n");
    printf("\nScoring weights: Coverage=%.0f%% DirtEff=%.0f%% BatCons=%.0f%% Movement=%.0f%%\n",
        cfg->weight_coverage, cfg->weight_dirt_efficiency, 
        cfg->weight_battery_conservation, cfg->weight_movement_quality);
    printf("\n");
}

/**
 * @brief Save ranking to text file
 */
void save_ranking_txt(team_score_t teams[], int team_count, config_t *cfg) {
    FILE *f = fopen("results/ranking.txt", "w");
    if (!f) {
        fprintf(stderr, "Warning: Could not create results/ranking.txt\n");
        return;
    }
    
    fprintf(f, "ROOMBA COMPETITION RANKING\n");
    fprintf(f, "==========================\n\n");
    
    fprintf(f, "Scoring Configuration:\n");
    fprintf(f, "  Coverage Weight:           %.0f%%\n", cfg->weight_coverage);
    fprintf(f, "  Dirt Efficiency Weight:    %.0f%%\n", cfg->weight_dirt_efficiency);
    fprintf(f, "  Battery Conservation:      %.0f%%\n", cfg->weight_battery_conservation);
    fprintf(f, "  Movement Quality Weight:   %.0f%%\n\n", cfg->weight_movement_quality);
    
    fprintf(f, "Rank  Team            Total Score  Cover  DirtEff  BatCons  Movement  Maps\n");
    fprintf(f, "--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < team_count; i++) {
        team_score_t *t = &teams[i];
        fprintf(f, "%-5d %-15s %8.2f     %5.1f  %6.1f   %6.1f   %6.1f      %d\n",
            i + 1, t->name, t->total_score,
            t->avg_coverage, t->avg_dirt_efficiency, t->avg_battery_conservation,
            t->avg_movement_quality, t->num_maps);
        
        if (t->num_crashes > 0) {
            fprintf(f, "      Warning: %d crash(es) detected\n", t->num_crashes);
        }
    }
    
    fclose(f);
    printf("[OK] Ranking saved to results/ranking.txt\n");
}

/**
 * @brief Save detailed scores to CSV
 */
void save_scores_csv(team_score_t teams[], int team_count) {
    FILE *f = fopen("results/scores.csv", "w");
    if (!f) {
        fprintf(stderr, "Warning: Could not create results/scores.csv\n");
        return;
    }
    
    fprintf(f, "rank,team,total_score,coverage,dirt_efficiency,battery_conservation,movement_quality,consistency,maps,crashes\n");
    
    for (int i = 0; i < team_count; i++) {
        team_score_t *t = &teams[i];
        fprintf(f, "%d,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d\n",
            i + 1, t->name, t->total_score,
            t->avg_coverage, t->avg_dirt_efficiency, t->avg_battery_conservation,
            t->avg_movement_quality, t->consistency_score, t->num_maps, t->num_crashes);
    }
    
    fclose(f);
    printf("[OK] Detailed scores saved to results/scores.csv\n");
}

/**
 * @brief Main function
 */
int main(int argc, char *argv[]) {
    const char *config_file = "scoring.conf";
    const char *stats_file = "stats.csv";
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            config_file = argv[++i];
        } else if (strcmp(argv[i], "--stats") == 0 && i + 1 < argc) {
            stats_file = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [OPTIONS]\n", argv[0]);
            printf("Options:\n");
            printf("  --config FILE   Use custom config file (default: scoring.conf)\n");
            printf("  --stats FILE    Use custom stats file (default: stats.csv)\n");
            printf("  --help          Show this help message\n");
            return 0;
        }
    }
    
    // Load configuration
    config_t cfg;
    load_config(config_file, &cfg);
    
    // Load stats
    map_result_t results[MAX_TEAMS * MAX_MAPS];
    int result_count = 0;
    
    if (!load_stats(stats_file, results, &result_count, &cfg)) {
        fprintf(stderr, "Error loading stats from %s\n", stats_file);
        return 1;
    }
    
    if (result_count == 0) {
        fprintf(stderr, "No results found in %s\n", stats_file);
        return 1;
    }
    
    printf("[OK] Loaded %d results from %s\n", result_count, stats_file);
    
    // Aggregate by team
    team_score_t teams[MAX_TEAMS];
    int team_count = aggregate_by_team(results, result_count, teams, &cfg);
    
    printf("[OK] Processed %d teams\n", team_count);
    
    // Sort teams by score
    qsort(teams, team_count, sizeof(team_score_t), compare_teams);
    
    // Display ranking
    display_ranking(teams, team_count, &cfg);
    
    // Save results
    save_ranking_txt(teams, team_count, &cfg);
    
    if (cfg.generate_csv) {
        save_scores_csv(teams, team_count);
    }
    
    return 0;
}
