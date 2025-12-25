/**
 * @file score.c
 * @brief Sistema de puntuación para la competición Roomba
 *
 * Reimplemented to use libscore for consistent calculations.
 */
#include "libscore.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEAMS 100
#define MAX_TOTAL_RESULTS (MAX_TEAMS * MAX_MAPS)

// UI Defaults (since libscore ignores display config)
const int highlight_top = 3;

/**
 * @brief Function to compare teams for sorting
 */
int compare_teams(const void *a, const void *b) {
  team_score_t *ta = (team_score_t *)a;
  team_score_t *tb = (team_score_t *)b;

  if (tb->total_score > ta->total_score)
    return 1;
  if (tb->total_score < ta->total_score)
    return -1;
  return 0;
}

/**
 * @brief Display ranking in console
 */
void display_ranking(team_score_t teams[], int team_count,
                     scoring_config_t *cfg) {
  printf("\n");
  printf("═════════════════════════════════════════════════════════════════════"
         "═════════════\n");
  printf("                        ROOMBA COMPETITION RANKING                   "
         "             \n");
  printf("═════════════════════════════════════════════════════════════════════"
         "═════════════\n");
  printf("Rank  Team            Score    Cover  DirtEff  BatCons  Movement  "
         "Consist  Maps\n");
  printf("─────────────────────────────────────────────────────────────────────"
         "─────────────\n");

  for (int i = 0; i < team_count; i++) {
    team_score_t *t = &teams[i];
    char marker = ' ';
    if (i < highlight_top) {
      marker = (i == 0) ? '*' : '+';
    }
    printf(
        "%c %-3d %-15s %6.2f   %5.1f  %6.1f   %6.1f   %6.1f    %6.1f    %d\n",
        marker, i + 1, t->name, t->total_score, t->avg_coverage,
        t->avg_dirt_efficiency, t->avg_battery_conservation,
        t->avg_movement_quality, t->consistency_score, t->num_maps);
    if (t->num_crashes > 0) {
      printf("      └─ [!] %d crash(es) detected\n", t->num_crashes);
    }
  }

  printf("═════════════════════════════════════════════════════════════════════"
         "═════════════\n");
  printf("\nScoring Rules:\n");
  printf("  Weights: Cover=%.0f%% Dirt=%.0f%% Bat=%.0f%% Move=%.0f%%\n",
         cfg->weight_coverage, cfg->weight_dirt_efficiency,
         cfg->weight_battery_conservation, cfg->weight_movement_quality);
  printf("  Bonuses: Consistency > %.0f (+%.0f) | LowBumps > %.0f%% (+%.0f) | "
         "Completion (+%.0f)\n",
         cfg->consistency_threshold, cfg->consistency_bonus,
         cfg->movement_quality_threshold, cfg->low_bumps_bonus,
         cfg->completion_bonus);
  printf("  Penalty: Crash (-%.0f)\n", cfg->crash_penalty);
  printf("\n");
}

/**
 * @brief Save ranking to text file
 */
void save_ranking_txt(team_score_t teams[], int team_count,
                      scoring_config_t *cfg) {
  FILE *f = fopen("results/ranking.txt", "w");
  if (!f) {
    fprintf(stderr, "Warning: Could not create results/ranking.txt\n");
    return;
  }

  fprintf(f, "ROOMBA COMPETITION RANKING\n");
  fprintf(f, "==========================\n\n");

  fprintf(f, "Configuration:\n");
  fprintf(f, "  Weights: Cover=%.0f%% Dirt=%.0f%% Bat=%.0f%% Move=%.0f%%\n",
          cfg->weight_coverage, cfg->weight_dirt_efficiency,
          cfg->weight_battery_conservation, cfg->weight_movement_quality);
  fprintf(f,
          "  Bonuses: Consistency > %.0f (+%.0f) | LowBumps > %.0f%% (+%.0f) | "
          "Completion (+%.0f)\n",
          cfg->consistency_threshold, cfg->consistency_bonus,
          cfg->movement_quality_threshold, cfg->low_bumps_bonus,
          cfg->completion_bonus);
  fprintf(f, "  Penalty: Crash (-%.0f)\n\n", cfg->crash_penalty);

  fprintf(f, "Rank  Team            Total Score  Cover  DirtEff  BatCons  "
             "Movement  Consist  Maps\n");
  fprintf(f, "-----------------------------------------------------------------"
             "----------------------\n");

  for (int i = 0; i < team_count; i++) {
    team_score_t *t = &teams[i];
    fprintf(
        f, "%-5d %-15s %8.2f     %5.1f  %6.1f   %6.1f   %6.1f    %6.1f    %d\n",
        i + 1, t->name, t->total_score, t->avg_coverage, t->avg_dirt_efficiency,
        t->avg_battery_conservation, t->avg_movement_quality,
        t->consistency_score, t->num_maps);

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

  fprintf(f, "rank,team,total_score,coverage,dirt_efficiency,battery_"
             "conservation,movement_quality,consistency,maps,crashes\n");

  for (int i = 0; i < team_count; i++) {
    team_score_t *t = &teams[i];
    fprintf(f, "%d,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d\n", i + 1, t->name,
            t->total_score, t->avg_coverage, t->avg_dirt_efficiency,
            t->avg_battery_conservation, t->avg_movement_quality,
            t->consistency_score, t->num_maps, t->num_crashes);
  }

  fclose(f);
  printf("[OK] Detailed scores saved to results/scores.csv\n");
}

int main(int argc, char *argv[]) {
  const char *config_file = "scoring.conf";
  const char *stats_file = "stats.csv";

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
      config_file = argv[++i];
    } else if (strcmp(argv[i], "--stats") == 0 && i + 1 < argc) {
      stats_file = argv[++i];
    } else if (strcmp(argv[i], "--help") == 0) {
      printf("Usage: %s [OPTIONS]\n", argv[0]);
      return 0;
    }
  }

  // Load config via libscore
  scoring_config_t cfg;
  scoring_load_config(config_file, &cfg);

  // Load stats via libscore
  map_result_t *all_results = malloc(sizeof(map_result_t) * MAX_TOTAL_RESULTS);
  if (!all_results) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return 1;
  }

  int result_count = 0;
  if (!scoring_load_stats(stats_file, all_results, &result_count,
                          MAX_TOTAL_RESULTS)) {
    fprintf(stderr, "Error loading stats from %s\n", stats_file);
    free(all_results);
    return 1;
  }

  if (result_count == 0) {
    fprintf(stderr, "No results found in %s\n", stats_file);
    free(all_results);
    return 1;
  }

  printf("[OK] Loaded %d results from %s\n", result_count, stats_file);

  // Calculate map scores
  for (int i = 0; i < result_count; i++) {
    scoring_calculate_map(&all_results[i], &cfg);
  }

  // Identify teams
  char team_names[MAX_TEAMS][TEAM_NAME_LEN];
  int team_count = 0;

  for (int i = 0; i < result_count; i++) {
    int known = 0;
    for (int t = 0; t < team_count; t++) {
      if (strcmp(team_names[t], all_results[i].team_name) == 0) {
        known = 1;
        break;
      }
    }
    if (!known && team_count < MAX_TEAMS) {
      strncpy(team_names[team_count], all_results[i].team_name, TEAM_NAME_LEN);
      printf("Entrando en carpeta/equipo: %s\n", team_names[team_count]);
      team_count++;
    }
  }

  // Aggregate by team
  team_score_t teams[MAX_TEAMS];

  for (int t = 0; t < team_count; t++) {
    map_result_t team_maps[MAX_MAPS];
    int map_count = 0;

    for (int i = 0; i < result_count; i++) {
      if (strcmp(all_results[i].team_name, team_names[t]) == 0) {
        if (map_count < MAX_MAPS) {
          team_maps[map_count++] = all_results[i];
        }
      }
    }

    scoring_aggregate_team(team_maps, map_count, &teams[t], &cfg);
  }

  printf("[OK] Processed %d teams\n", team_count);

  // Sort
  qsort(teams, team_count, sizeof(team_score_t), compare_teams);

  // Display and Save
  display_ranking(teams, team_count, &cfg);
  save_ranking_txt(teams, team_count, &cfg);

  if (cfg.generate_csv) {
    save_scores_csv(teams, team_count);
  }

  free(all_results);
  return 0;
}
