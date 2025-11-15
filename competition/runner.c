/**
 * Competition Runner
 * 
 * This program orchestrates the entire Roomba competition:
 * - Discovers all team directories
 * - Compiles each team's code with competition simulator
 * - Executes multiple rounds and maps for each team
 * - Generates consolidated ranking based on statistics
 * 
 * Directory structure expected:
 *   teams/
 *     team1/
 *       main.c
 *     team2/
 *       main.c
 *     ...
 * 
 * Usage:
 *   ./runner [teams_dir]
 * 
 * Output:
 *   stats.csv - Complete statistics for all teams and maps
 *   ranking.txt - Final team rankings
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_TEAMS 100
#define TEAM_NAME_LEN 64
#define MAPS_COUNT 4
#define REPS_PER_MAP 5

typedef struct {
    char name[TEAM_NAME_LEN];
    int total_cells_visited;
    int total_dirt_cleaned;
    float total_battery_used;
    int score;
} team_result_t;

/**
 * @brief Initialize stats.csv file with headers
 */
void init_stats_file(const char *filename) {
    FILE *fd = fopen(filename, "w");
    if(!fd) {
        fprintf(stderr, "Error creating stats file\n");
        exit(1);
    }
    
    fprintf(fd, "team,map_type,cell_total,cell_visited,dirt_total,dirt_cleaned,"
                "bat_total,bat_mean,forward,turn,bumps,clean,load\n");
    fclose(fd);
    
    printf("✓ Statistics file initialized: %s\n", filename);
}

/**
 * @brief Check if a directory entry is a valid team directory
 */
int is_team_dir(const char *base_path, struct dirent *entry) {
    if(entry->d_type != DT_DIR)
        return 0;
    
    if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        return 0;
    
    // Check if main.c exists in the directory
    char main_path[512];
    snprintf(main_path, sizeof(main_path), "%s/%s/main.c", base_path, entry->d_name);
    
    struct stat st;
    return (stat(main_path, &st) == 0);
}

/**
 * @brief Compile a team's code with competition simulator
 */
int compile_team(const char *teams_dir, const char *team_name) {
    char cmd[512];
    char comp_path[256];
    
    // Get absolute path to competition directory
    getcwd(comp_path, sizeof(comp_path));
    
    snprintf(cmd, sizeof(cmd),
        "cd %s/%s && gcc -DCOMPETITION_MODE -Wall -Wno-unused-function "
        "main.c %s/simula_comp.c -lm -o roomba 2>&1",
        teams_dir, team_name, comp_path);
    
    printf("  Compiling %s... ", team_name);
    fflush(stdout);
    
    int result = system(cmd);
    
    if(result == 0) {
        printf("✓\n");
        return 0;
    } else {
        printf("✗ FAILED\n");
        return -1;
    }
}

/**
 * @brief Initialize config.txt for a team
 */
void init_team_config(const char *teams_dir, const char *team_name) {
    char config_path[512];
    snprintf(config_path, sizeof(config_path), "%s/%s/config.txt", teams_dir, team_name);
    
    FILE *fd = fopen(config_path, "w");
    if(fd) {
        fprintf(fd, "0 0");  // Start with rep 0, map 0
        fclose(fd);
    }
}

/**
 * @brief Execute all rounds for a team
 */
int execute_team_rounds(const char *teams_dir, const char *team_name) {
    char cmd[512];
    int total_runs = MAPS_COUNT * REPS_PER_MAP;
    int successful_runs = 0;
    
    printf("  Executing %d rounds for %s:\n", total_runs, team_name);
    
    for(int run = 0; run < total_runs; run++) {
        int map_id = run / REPS_PER_MAP;
        int rep = run % REPS_PER_MAP;
        
        snprintf(cmd, sizeof(cmd), 
            "cd %s/%s && timeout 30s ./roomba > /dev/null 2>&1",
            teams_dir, team_name);
        
        printf("    Map %d, Rep %d... ", map_id, rep);
        fflush(stdout);
        
        int result = system(cmd);
        
        if(result == 0) {
            printf("✓\n");
            successful_runs++;
        } else {
            printf("✗ (timeout or error)\n");
        }
    }
    
    printf("  Completed: %d/%d runs\n\n", successful_runs, total_runs);
    return successful_runs;
}

/**
 * @brief Calculate team scores from stats.csv
 */
void calculate_rankings(const char *stats_file, team_result_t results[], int *team_count) {
    FILE *fd = fopen(stats_file, "r");
    if(!fd) {
        fprintf(stderr, "Error opening stats file for ranking\n");
        return;
    }
    
    char line[512];
    fgets(line, sizeof(line), fd);  // Skip header
    
    *team_count = 0;
    
    while(fgets(line, sizeof(line), fd)) {
        char team_name[TEAM_NAME_LEN];
        int map_type, cell_total, cell_visited, dirt_total, dirt_cleaned;
        float bat_total, bat_mean;
        int forward, turn, bumps, clean, load;
        
        sscanf(line, "%[^,],%d,%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d",
            team_name, &map_type, &cell_total, &cell_visited, 
            &dirt_total, &dirt_cleaned, &bat_total, &bat_mean,
            &forward, &turn, &bumps, &clean, &load);
        
        // Find or create team entry
        int team_idx = -1;
        for(int i = 0; i < *team_count; i++) {
            if(strcmp(results[i].name, team_name) == 0) {
                team_idx = i;
                break;
            }
        }
        
        if(team_idx == -1) {
            team_idx = (*team_count)++;
            strcpy(results[team_idx].name, team_name);
            results[team_idx].total_cells_visited = 0;
            results[team_idx].total_dirt_cleaned = 0;
            results[team_idx].total_battery_used = 0;
            results[team_idx].score = 0;
        }
        
        // Accumulate statistics
        results[team_idx].total_cells_visited += cell_visited;
        results[team_idx].total_dirt_cleaned += dirt_cleaned;
        results[team_idx].total_battery_used += bat_total;
        
        // Calculate score: dirt cleaned (priority) + cells visited - battery penalty
        results[team_idx].score += (dirt_cleaned * 100) + cell_visited - (int)(bat_total / 10);
    }
    
    fclose(fd);
}

/**
 * @brief Comparison function for sorting teams by score
 */
int compare_teams(const void *a, const void *b) {
    team_result_t *ta = (team_result_t*)a;
    team_result_t *tb = (team_result_t*)b;
    return tb->score - ta->score;  // Descending order
}

/**
 * @brief Generate and display ranking
 */
void display_ranking(const char *stats_file) {
    team_result_t results[MAX_TEAMS];
    int team_count = 0;
    
    calculate_rankings(stats_file, results, &team_count);
    
    if(team_count == 0) {
        printf("No results to display\n");
        return;
    }
    
    // Sort teams by score
    qsort(results, team_count, sizeof(team_result_t), compare_teams);
    
    // Display ranking
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    COMPETITION RANKING                        \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Rank  Team            Score    Cells    Dirt    Battery\n");
    printf("───────────────────────────────────────────────────────────────\n");
    
    for(int i = 0; i < team_count; i++) {
        printf("%-5d %-15s %-8d %-8d %-7d %.1f\n",
            i + 1,
            results[i].name,
            results[i].score,
            results[i].total_cells_visited,
            results[i].total_dirt_cleaned,
            results[i].total_battery_used);
    }
    
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    // Save to file
    FILE *fd = fopen("ranking.txt", "w");
    if(fd) {
        fprintf(fd, "ROOMBA COMPETITION RANKING\n\n");
        fprintf(fd, "Rank  Team            Score    Cells    Dirt    Battery\n");
        fprintf(fd, "-----------------------------------------------------------\n");
        for(int i = 0; i < team_count; i++) {
            fprintf(fd, "%-5d %-15s %-8d %-8d %-7d %.1f\n",
                i + 1, results[i].name, results[i].score,
                results[i].total_cells_visited,
                results[i].total_dirt_cleaned,
                results[i].total_battery_used);
        }
        fclose(fd);
        printf("✓ Ranking saved to ranking.txt\n");
    }
}

/**
 * @brief Main competition runner
 */
int main(int argc, char *argv[]) {
    const char *teams_dir = (argc > 1) ? argv[1] : "../teams";
    const char *stats_file = "stats.csv";
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("              ROOMBA COMPETITION RUNNER                        \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Teams directory: %s\n", teams_dir);
    printf("Maps: %d, Repetitions: %d, Total runs per team: %d\n\n",
        MAPS_COUNT, REPS_PER_MAP, MAPS_COUNT * REPS_PER_MAP);
    
    // Initialize statistics file
    init_stats_file(stats_file);
    
    // Open teams directory
    DIR *d = opendir(teams_dir);
    if(!d) {
        fprintf(stderr, "Error: Cannot open teams directory: %s\n", teams_dir);
        return 1;
    }
    
    // Process each team
    struct dirent *dir;
    int team_count = 0;
    int successful_teams = 0;
    
    printf("Discovering teams...\n");
    while((dir = readdir(d)) != NULL) {
        if(!is_team_dir(teams_dir, dir))
            continue;
        
        team_count++;
        printf("\n[Team %d: %s]\n", team_count, dir->d_name);
        
        // Initialize team config
        init_team_config(teams_dir, dir->d_name);
        
        // Compile team code
        if(compile_team(teams_dir, dir->d_name) != 0) {
            printf("  Skipping due to compilation errors\n");
            continue;
        }
        
        // Execute all rounds
        int runs = execute_team_rounds(teams_dir, dir->d_name);
        if(runs > 0)
            successful_teams++;
    }
    
    closedir(d);
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Competition completed!\n");
    printf("Total teams: %d\n", team_count);
    printf("Successful teams: %d\n", successful_teams);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    // Generate and display ranking
    if(successful_teams > 0) {
        display_ranking(stats_file);
    }
    
    return 0;
}
