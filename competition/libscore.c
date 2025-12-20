/**
 * @file libscore.c
 * @brief Competition scoring library implementation
 * This file contains the implementation of scoring logic.
 * Shared scoring logic for both organizers and participants.
 * 
 * @author IPR-GIIROB-ETSINF-UPV
 * @date 2025
 */

#include "libscore.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_LINE 1024 ///< Longitud máxima de línea para buffers de texto

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

/**
 * @brief Trim whitespace from string
 */
static void trim(char *str) {
    char *start = str;
    while(isspace(*start)) start++;
    
    char *end = start + strlen(start) - 1;
    while(end > start && isspace(*end)) end--;
    *(end + 1) = '\0';
    
    memmove(str, start, strlen(start) + 1);
}

/**
 * @brief Parse key=value from config line
 */
static bool parse_config_line(const char *line, char *key, char *value) {
    const char *eq = strchr(line, '=');
    if (!eq) return false;
    
    size_t key_len = eq - line;
    if (key_len >= MAX_LINE) return false;
    
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    trim(key);
    
    strcpy(value, eq + 1);
    trim(value);
    
    return true;
}

float scoring_stddev(const float *values, int count) {
    if (count <= 1) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    float mean = sum / count;
    
    float variance = 0.0f;
    for (int i = 0; i < count; i++) {
        float diff = values[i] - mean;
        variance += diff * diff;
    }
    variance /= count;
    
    return sqrtf(variance);
}

/* ============================================================================
 * CONFIGURATION
 * ============================================================================ */

/**
 * @brief Inicializa la configuración de puntuación con valores por defecto
 * @param config Puntero a la configuración a rellenar
 */
void scoring_default_config(scoring_config_t *config) {
    config->weight_coverage = 30.0f;
    config->weight_dirt_efficiency = 35.0f;
    config->weight_battery_conservation = 20.0f;
    config->weight_movement_quality = 15.0f;
    
    config->completion_bonus = 5.0f;
    config->low_bumps_bonus = 3.0f;
    config->crash_penalty = 10.0f;
    
    config->movement_quality_threshold = 90.0f;
    
    config->dirt_scale = 100.0f;
    config->battery_scale = 1000.0f;
    
    config->generate_csv = 1;
    config->verbose = 0;
}

/**
 * @brief Carga la configuración de puntuación desde un archivo
 * @param filename Ruta al archivo de configuración
 * @param config Puntero a la configuración a rellenar
 * @return true si OK, false si error
 */
bool scoring_load_config(const char *filename, scoring_config_t *config) {
    // Start with defaults
    scoring_default_config(config);
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Warning: Cannot open %s, using defaults\n", filename);
        return true; // Not fatal, use defaults
    }
    
    char line[MAX_LINE];
    char key[MAX_LINE];
    char value[MAX_LINE];
    
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == ';' || line[0] == '\0') continue;
        
        // Skip section headers
        if (line[0] == '[') continue;
        
        // Parse key=value
        if (!parse_config_line(line, key, value)) continue;
        
        // Apply configuration
        if (strcmp(key, "coverage") == 0) {
            config->weight_coverage = atof(value);
        } else if (strcmp(key, "dirt_efficiency") == 0) {
            config->weight_dirt_efficiency = atof(value);
        } else if (strcmp(key, "battery_conservation") == 0) {
            config->weight_battery_conservation = atof(value);
        } else if (strcmp(key, "movement_quality") == 0) {
            config->weight_movement_quality = atof(value);
        } else if (strcmp(key, "completion_bonus") == 0) {
            config->completion_bonus = atof(value);
        } else if (strcmp(key, "low_bumps_bonus") == 0) {
            config->low_bumps_bonus = atof(value);
        } else if (strcmp(key, "crash_penalty") == 0) {
            config->crash_penalty = atof(value);
        } else if (strcmp(key, "movement_quality_threshold") == 0) {
            config->movement_quality_threshold = atof(value);
        } else if (strcmp(key, "generate_csv") == 0) {
            config->generate_csv = atoi(value);
        } else if (strcmp(key, "verbose") == 0) {
            config->verbose = atoi(value);
        }
    }
    
    fclose(f);
    return true;
}

/* ============================================================================
 * SCORING CALCULATIONS
 * ============================================================================ */

/**
 * @brief Calcula las métricas de puntuación para un mapa
 * @param result Puntero a los resultados del mapa
 * @param config Puntero a la configuración de puntuación
 */
void scoring_calculate_map(map_result_t *result, const scoring_config_t *config) {
    // Coverage: percentage of cells visited
    if (result->cell_total > 0) {
        result->coverage = (result->cell_visited * 100.0f) / result->cell_total;
        if (result->coverage > 100.0f) {
            result->coverage = 100.0f;
        }
    } else {
        result->coverage = 0.0f;
    }
    
    // Dirt efficiency: dirt cleaned per battery unit consumed
    if (result->bat_total > 0) {
        result->dirt_efficiency = (result->dirt_cleaned / result->bat_total) * config->dirt_scale;
    } else {
        result->dirt_efficiency = 0.0f;
    }
    
    // Battery conservation: average battery remaining
    result->battery_conservation = (result->bat_mean / config->battery_scale) * 100.0f;
    if (result->battery_conservation > 100.0f) {
        result->battery_conservation = 100.0f;
    }
    
    // Movement quality: ratio of successful moves to bumps
    int total_moves = result->forward + result->turn + result->bumps;
    if (total_moves > 0) {
        float bump_ratio = (float)result->bumps / total_moves;
        result->movement_quality = (1.0f - bump_ratio) * 100.0f;
        if( result->movement_quality < 0.0f) {
            result->movement_quality = 0.0f;
        }
    } else {
        result->movement_quality = 0.0f;
    }
}

/**
 * @brief Calcula la puntuación agregada de un equipo
 * @param results Array de resultados de mapas
 * @param count Número de mapas
 * @param team_score Puntero a la estructura de puntuación del equipo
 * @param config Puntero a la configuración de puntuación
 */
void scoring_aggregate_team(const map_result_t *results, int count, 
                            team_score_t *team_score, const scoring_config_t *config) {
    if (count == 0) {
        memset(team_score, 0, sizeof(team_score_t));
        return;
    }
    
    // Copy team name from first result
    strncpy(team_score->name, results[0].team_name, TEAM_NAME_LEN - 1);
    team_score->name[TEAM_NAME_LEN - 1] = '\0';
    
    team_score->num_maps = count;
    team_score->num_crashes = 0;
    
    // Accumulate scores
    float sum_coverage = 0.0f;
    float sum_dirt_eff = 0.0f;
    float sum_bat_cons = 0.0f;
    float sum_movement = 0.0f;
    
    float coverage_values[MAX_MAPS];
    float dirt_eff_values[MAX_MAPS];
    float bat_cons_values[MAX_MAPS];
    float movement_values[MAX_MAPS];
    
    for (int i = 0; i < count && i < MAX_MAPS; i++) {
        coverage_values[i] = results[i].coverage;
        dirt_eff_values[i] = results[i].dirt_efficiency;
        bat_cons_values[i] = results[i].battery_conservation;
        movement_values[i] = results[i].movement_quality;
        
        sum_coverage += results[i].coverage;
        sum_dirt_eff += results[i].dirt_efficiency;
        sum_bat_cons += results[i].battery_conservation;
        sum_movement += results[i].movement_quality;
        
        // Check for crashes (detect via bat_total = 0 or similar indicator)
        if (results[i].cell_visited == 0 && results[i].bat_total > 0) {
            team_score->num_crashes++;
        }
    }
    
    // Calculate averages
    team_score->avg_coverage = sum_coverage / count;
    team_score->avg_dirt_efficiency = sum_dirt_eff / count;
    team_score->avg_battery_conservation = sum_bat_cons / count;
    team_score->avg_movement_quality = sum_movement / count;
    
    // Calculate consistency (inverse of average stddev)
    float stddev_avg = (
        scoring_stddev(coverage_values, count) +
        scoring_stddev(dirt_eff_values, count) +
        scoring_stddev(bat_cons_values, count) +
        scoring_stddev(movement_values, count)
    ) / 4.0f;
    
    team_score->consistency_score = (stddev_avg > 0) ? (100.0f / (1.0f + stddev_avg)) : 100.0f;
    
    // Calculate weighted score
    team_score->total_score = (
        team_score->avg_coverage * (config->weight_coverage / 100.0f) +
        team_score->avg_dirt_efficiency * (config->weight_dirt_efficiency / 100.0f) +
        team_score->avg_battery_conservation * (config->weight_battery_conservation / 100.0f) +
        team_score->avg_movement_quality * (config->weight_movement_quality / 100.0f)
    );
    
    // Apply bonuses and penalties
    if (team_score->num_crashes == 0) {
        team_score->total_score += config->completion_bonus;
    } else {
        team_score->total_score -= team_score->num_crashes * config->crash_penalty;
    }
    
    // Low bumps bonus
    if (team_score->avg_movement_quality >= config->movement_quality_threshold) {
        team_score->total_score += config->low_bumps_bonus;
    }
    
    // Ensure non-negative
    if (team_score->total_score < 0.0f) {
        team_score->total_score = 0.0f;
    }
}

/* ============================================================================
 * CSV LOADING
 * ============================================================================ */

/**
 * @brief Carga los resultados de stats.csv
 * @param filename Ruta al archivo stats.csv
 * @param results Array de resultados de mapas
 * @param count Puntero a entero para número de resultados
 * @param max_results Máximo de resultados a cargar
 * @return true si OK, false si error
 */
bool scoring_load_stats(const char *filename, map_result_t *results, 
                       int *count, int max_results) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        return false;
    }
    
    char line[MAX_LINE];
    *count = 0;
    
    // Read header
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return false;
    }
    
    // Determine if format has team column (competition) or not (participant)
    bool has_team_column = (strstr(line, "team") != NULL);
    
    // Read data lines
    while (fgets(line, sizeof(line), f) && *count < max_results) {
        map_result_t *r = &results[*count];
        
        if (has_team_column) {
            // Competition format: team,map_type,cell_total,...
            if (sscanf(line, "%63[^,],%d,%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d",
                      r->team_name, &r->map_type,
                      &r->cell_total, &r->cell_visited,
                      &r->dirt_total, &r->dirt_cleaned,
                      &r->bat_total, &r->bat_mean,
                      &r->forward, &r->turn, &r->bumps,
                      &r->clean, &r->load) != 13) {
                continue; // Skip malformed lines
            }
        } else {
            // Participant format: cell_total,cell_visited,...
            strcpy(r->team_name, "me");
            r->map_type = 0;
            
            if (sscanf(line, "%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d",
                      &r->cell_total, &r->cell_visited,
                      &r->dirt_total, &r->dirt_cleaned,
                      &r->bat_total, &r->bat_mean,
                      &r->forward, &r->turn, &r->bumps,
                      &r->clean, &r->load) != 11) {
                continue; // Skip malformed lines
            }
        }
        
        (*count)++;
    }
    
    fclose(f);
    return true;
}

/* ============================================================================
 * DISPLAY
 * ============================================================================ */

/**
 * @brief Muestra el informe de puntuación de un equipo por consola
 * @param team Puntero a la estructura de puntuación del equipo
 * @param config Puntero a la configuración de puntuación
 */
void scoring_display_team(const team_score_t *team, const scoring_config_t *config) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════\n");
    printf("  SCORING REPORT: %s\n", team->name);
    printf("═══════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("  Total Score:          %.2f / 100\n", team->total_score);
    printf("\n");
    printf("  Breakdown:\n");
    printf("    Coverage:           %.1f%%  (weight: %.0f%%)\n", 
           team->avg_coverage, config->weight_coverage);
    printf("    Dirt Efficiency:    %.1f%%  (weight: %.0f%%)\n", 
           team->avg_dirt_efficiency, config->weight_dirt_efficiency);
    printf("    Battery Conservation: %.1f%%  (weight: %.0f%%)\n", 
           team->avg_battery_conservation, config->weight_battery_conservation);
    printf("    Movement Quality:   %.1f%%  (weight: %.0f%%)\n", 
           team->avg_movement_quality, config->weight_movement_quality);
    printf("\n");
    printf("  Bonuses/Penalties:\n");
    
    if (team->num_crashes == 0) {
        printf("    ✓ Completion bonus: +%.0f\n", config->completion_bonus);
    } else {
        printf("    ✗ Crash penalty: -%.0f\n", team->num_crashes * config->crash_penalty);
    }
    
    if (team->avg_movement_quality >= config->movement_quality_threshold) {
        printf("    ✓ Low bumps bonus: +%.0f\n", config->low_bumps_bonus);
    }
    
    printf("\n");
    printf("  Statistics:\n");
    printf("    Maps executed:      %d\n", team->num_maps);
    printf("    Crashes:            %d\n", team->num_crashes);
    printf("    Consistency:        %.1f\n", team->consistency_score);
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════\n");
    printf("\n");
}
