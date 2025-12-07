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
#include <unistd.h>  // For access()
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_TEAMS 100              ///< Número máximo de equipos permitidos
#define TEAM_NAME_LEN 64           ///< Longitud máxima del nombre de equipo
#define MAPS_COUNT 4               ///< Cantidad de mapas en la competición
#define REPS_PER_MAP 5             ///< Repeticiones por cada mapa
#define LOGS_DIR "logs"            ///< Directorio para almacenar logs
#define EXEC_TIMEOUT 30            ///< Tiempo máximo de ejecución por equipo (segundos)

/**
 * @brief Estructura para almacenar los resultados de cada equipo
 */
typedef struct {
    char name[TEAM_NAME_LEN];         ///< Nombre del equipo
    int total_cells_visited;          ///< Total de celdas visitadas
    int total_dirt_cleaned;           ///< Total de suciedad limpiada
    float total_battery_used;         ///< Batería total consumida
    int score;                        ///< Puntuación final del equipo
} team_result_t;


/**
 * @brief Inicializa el archivo stats.csv con cabeceras
 * @param filename Ruta al archivo de estadísticas
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

// ...existing code...
/**
 * @brief Verifica el resultado de ejecución y registra información de crash
 * @param status Código de estado del proceso
 * @param stderr_file Archivo de log de errores
 * @return Cadena con el estado: "success", "error", "crash", "stopped", "unknown"
 */
const char* check_execution_result(int status, const char *stderr_file) {
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0) {
            return "success";
        } else {
            return "error";
        }
    } else if (WIFSIGNALED(status)) {
        int signal = WTERMSIG(status);
        FILE *log = fopen(stderr_file, "a");
        if (log) {
            fprintf(log, "\n=== CRASH DETECTED ===\n");
            switch(signal) {
                case SIGSEGV:
                    fprintf(log, "Signal: SIGSEGV (Segmentation fault)\n");
                    break;
                case SIGABRT:
                    fprintf(log, "Signal: SIGABRT (Abort)\n");
                    break;
                case SIGFPE:
                    fprintf(log, "Signal: SIGFPE (Floating point exception)\n");
                    break;
                case SIGILL:
                    fprintf(log, "Signal: SIGILL (Illegal instruction)\n");
                    break;
                case SIGBUS:
                    fprintf(log, "Signal: SIGBUS (Bus error)\n");
                    break;
                default:
                    fprintf(log, "Signal: %d (Unknown)\n", signal);
                    break;
            }
            fprintf(log, "======================\n");
            fclose(log);
        }
        return "crash";
    } else if (WIFSTOPPED(status)) {
        return "stopped";
    }
    return "unknown";
}

// ...existing code...
/**
 * @brief Inicializa el directorio de logs
 */
void init_logs_dir(void) {
    struct stat st = {0};
    
    // Create logs directory if it doesn't exist
    if (stat(LOGS_DIR, &st) == -1) {
        mkdir(LOGS_DIR, 0755);
    }
    
    printf("✓ Logs directory ready: %s/\n", LOGS_DIR);
}

// ...existing code...
/**
 * @brief Verifica si una entrada de directorio es un equipo válido
 * @param base_path Ruta base
 * @param entry Entrada de directorio
 * @return 1 si es equipo válido, 0 si no
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

// ...existing code...
/**
 * @brief Compila el código de un equipo con el simulador de competición
 * @param teams_dir Directorio de equipos
 * @param team_name Nombre del equipo
 * @return 0 si OK, -1 si error
 */
int compile_team(const char *teams_dir, const char *team_name) {
    char cmd[1024];
    char comp_path[256];
    char main_file[256] = "main.c";
    
    // Get absolute path to competition directory
    getcwd(comp_path, sizeof(comp_path));
    
    // Check if main.c exists, otherwise look for any .c file
    char test_path[512];
    snprintf(test_path, sizeof(test_path), "%s/%s/main.c", teams_dir, team_name);
    
    if(access(test_path, F_OK) != 0) {
        // Look for any .c file
        DIR *dir;
        struct dirent *entry;
        char dir_path[512];
        
        snprintf(dir_path, sizeof(dir_path), "%s/%s", teams_dir, team_name);
        dir = opendir(dir_path);
        
        if(dir) {
            while((entry = readdir(dir)) != NULL) {
                size_t len = strlen(entry->d_name);
                if(len > 2 && strcmp(entry->d_name + len - 2, ".c") == 0) {
                    strncpy(main_file, entry->d_name, sizeof(main_file) - 1);
                    break;
                }
            }
            closedir(dir);
        }
    }
    
    // Use precompiled object file from lib/
    // Add -DCOMPETITION_MODE to disable visualization
    snprintf(cmd, sizeof(cmd),
        "cd %s/%s && gcc -Wall -Wno-unused-function -DCOMPETITION_MODE -I%s/.. "
        "%s %s/lib/simula.o "
        "%s/competition_ext.c %s/simula_comp.c -lm -o roomba 2>&1",
        teams_dir, team_name, comp_path,
        main_file, comp_path,
        comp_path, comp_path);
    
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

// ...existing code...
/**
 * @brief Inicializa el archivo config.txt para un equipo
 * @param teams_dir Directorio de equipos
 * @param team_name Nombre del equipo
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

// ...existing code...
/**
 * @brief Muestra una barra de progreso en consola
 * @param current Valor actual
 * @param total Valor total
 * @param bar_width Ancho de la barra
 */
void print_progress_bar(int current, int total, int bar_width) {
    float progress = (float)current / total;
    int filled = (int)(progress * bar_width);
    
    printf("  [");
    for(int i = 0; i < bar_width; i++) {
        if(i < filled) {
            printf("#");
        } else {
            printf("-");
        }
    }
    printf("] %3d%% (%d/%d)", (int)(progress * 100), current, total);
    fflush(stdout);
}

// ...existing code...
/**
 * @brief Ejecuta todas las rondas para un equipo
 * @param teams_dir Directorio de equipos
 * @param team_name Nombre del equipo
 * @return Número de ejecuciones exitosas
 */
int execute_team_rounds(const char *teams_dir, const char *team_name) {
    char cmd[512];
    char log_stdout[256];
    char log_stderr[256];
    char team_stats[512];
    int total_runs = MAPS_COUNT * REPS_PER_MAP;
    int successful_runs = 0;
    int crashes = 0;
    int errors = 0;
    
    // Initialize team's stats.csv with header
    snprintf(team_stats, sizeof(team_stats), "%s/%s/stats.csv", teams_dir, team_name);
    FILE *stats_fd = fopen(team_stats, "w");
    if(stats_fd) {
        fprintf(stats_fd, "cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, forward, turn, bumps, clean, load\n");
        fclose(stats_fd);
    }
    
    printf("  Executing %d rounds for %s:\n", total_runs, team_name);
    
    for(int run = 0; run < total_runs; run++) {
        int map_id = run / REPS_PER_MAP;
        int rep = run % REPS_PER_MAP;
        
        // Create log file paths
        snprintf(log_stdout, sizeof(log_stdout), 
            "%s/%s_map%d_run%d.stdout", LOGS_DIR, team_name, map_id, rep);
        snprintf(log_stderr, sizeof(log_stderr), 
            "%s/%s_map%d_run%d.stderr", LOGS_DIR, team_name, map_id, rep);
        
        // Execute with logging and crash detection
        // Set VISUAL=0 environment variable to disable visualization
        snprintf(cmd, sizeof(cmd), 
            "cd %s/%s && VISUAL=0 ./roomba > ../../%s 2> ../../%s",
            teams_dir, team_name, log_stdout, log_stderr);
        
        // Show progress bar
        printf("\r");
        print_progress_bar(run, total_runs, 30);
        printf(" Map %d Rep %d", map_id, rep);
        fflush(stdout);
        
        int result = system(cmd);
        const char *status = check_execution_result(result, log_stderr);
        
        if(strcmp(status, "success") == 0) {
            successful_runs++;
        } else if(strcmp(status, "crash") == 0) {
            crashes++;
        } else if(strcmp(status, "error") == 0) {
            errors++;
        }
    }
    
    // Final progress bar
    printf("\r");
    print_progress_bar(total_runs, total_runs, 30);
    printf(" [OK] Done\n");
    
    // Summary
    printf("  Results: %d OK, %d CRASH, %d ERROR (Total: %d/%d successful)\n\n", 
           successful_runs, crashes, errors, successful_runs, total_runs);
    
    return successful_runs;
}

// ...existing code...
/**
 * @brief Valida que todos los archivos de log esperados existen
 * @param team_name Nombre del equipo
 * @param total_runs Número total de ejecuciones
 * @return Número de logs faltantes
 */
int validate_team_logs(const char *team_name, int total_runs) {
    int missing = 0;
    
    for(int run = 0; run < total_runs; run++) {
        int map_id = run / REPS_PER_MAP;
        int rep = run % REPS_PER_MAP;
        
        char log_stdout[256];
        char log_stderr[256];
        snprintf(log_stdout, sizeof(log_stdout), 
            "%s/%s_map%d_run%d.stdout", LOGS_DIR, team_name, map_id, rep);
        snprintf(log_stderr, sizeof(log_stderr), 
            "%s/%s_map%d_run%d.stderr", LOGS_DIR, team_name, map_id, rep);
        
        if(access(log_stdout, F_OK) != 0 || access(log_stderr, F_OK) != 0) {
            missing++;
        }
    }
    
    if(missing > 0) {
        printf("  [!] Warning: %d log files missing\n", missing);
    }
    
    return missing;
}

// ...existing code...
/**
 * @brief Consolida las estadísticas de un equipo en el archivo central
 * @param teams_dir Directorio de equipos
 * @param team_name Nombre del equipo
 * @param central_stats Archivo central de estadísticas
 */
void consolidate_team_stats(const char *teams_dir, const char *team_name, const char *central_stats) {
    char team_stats_file[512];
    snprintf(team_stats_file, sizeof(team_stats_file), "%s/%s/stats.csv", teams_dir, team_name);
    
    FILE *src = fopen(team_stats_file, "r");
    if(!src) {
        printf("  [!] Warning: No stats.csv found for %s\n", team_name);
        return;
    }
    
    FILE *dest = fopen(central_stats, "a");
    if(!dest) {
        fclose(src);
        return;
    }
    
    // Skip header line in source
    char line[1024];
    if(fgets(line, sizeof(line), src) == NULL) {
        fclose(src);
        fclose(dest);
        return;
    }
    
    // Copy all data lines, prepending team name
    int line_num = 0;
    while(fgets(line, sizeof(line), src)) {
        // Each line is a different execution, calculate map type
        int map_type = line_num / REPS_PER_MAP;
        fprintf(dest, "%s,%d,%s", team_name, map_type, line);
        line_num++;
    }
    
    fclose(src);
    fclose(dest);
}

// ...existing code...
/**
 * @brief Calcula las puntuaciones de los equipos a partir de stats.csv
 * @param stats_file Archivo de estadísticas
 * @param results Array de resultados de equipos
 * @param team_count Puntero a número de equipos
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

// ...existing code...
/**
 * @brief Función de comparación para ordenar equipos por puntuación
 * @param a Puntero al primer equipo
 * @param b Puntero al segundo equipo
 * @return Diferencia de puntuación (descendente)
 */
int compare_teams(const void *a, const void *b) {
    team_result_t *ta = (team_result_t*)a;
    team_result_t *tb = (team_result_t*)b;
    return tb->score - ta->score;  // Descending order
}

// ...existing code...
/**
 * @brief Genera y muestra el ranking de la competición
 * @param stats_file Archivo de estadísticas
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

// ...existing code...
/**
 * @brief Función principal del runner de competición
 *
 * Parsea argumentos, compila y ejecuta equipos, genera ranking y archivos.
 * @param argc Número de argumentos
 * @param argv Vector de argumentos
 * @return 0 si OK, 1 si error
 */
int main(int argc, char *argv[]) {
    const char *teams_dir = "teams";
    const char *stats_file = "stats.csv";
    int dry_run = 0;
    const char *single_team = NULL;
    
    // Parse arguments
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--dry-run") == 0) {
            dry_run = 1;
        } else if(strncmp(argv[i], "--team=", 7) == 0) {
            single_team = argv[i] + 7;
        } else if(argv[i][0] != '-') {
            teams_dir = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            fprintf(stderr, "Usage: %s [teams_dir] [--dry-run] [--team=XX]\n", argv[0]);
            return 1;
        }
    }
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("              ROOMBA COMPETITION RUNNER                        \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    if(dry_run) {
        printf("[DRY RUN MODE - No execution, just showing what would run]\n\n");
    }
    if(single_team) {
        printf("Single team mode: %s\n", single_team);
    }
    printf("Teams directory: %s\n", teams_dir);
    printf("Maps: %d, Repetitions: %d, Total runs per team: %d\n\n",
        MAPS_COUNT, REPS_PER_MAP, MAPS_COUNT * REPS_PER_MAP);
    
    // Check if simula.o exists, if not build it
    if(access("lib/simula.o", F_OK) != 0) {
        printf("⚙️  Building competition library...\n");
        int result = system("cd .. && make lib-competition > /dev/null 2>&1");
        if(result != 0) {
            fprintf(stderr, "❌ Failed to build competition library\n");
            fprintf(stderr, "   Please run 'make lib-competition' manually from project root\n");
            return 1;
        }
        printf("✓ Competition library built\n\n");
    } else {
        printf("✓ Using existing competition library (lib/simula.o)\n\n");
    }
    
    // Initialize logs directory
    init_logs_dir();
    
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
        
        // Filter by single team if specified
        if(single_team && strcmp(dir->d_name, single_team) != 0)
            continue;
        
        team_count++;
        printf("\n[Team %d: %s]\n", team_count, dir->d_name);
        
        if(dry_run) {
            printf("  [DRY RUN] Would compile and execute %d rounds\n", MAPS_COUNT * REPS_PER_MAP);
            continue;
        }
        
        // Initialize team config
        init_team_config(teams_dir, dir->d_name);
        
        // Compile team code
        if(compile_team(teams_dir, dir->d_name) != 0) {
            printf("  Skipping due to compilation errors\n");
            continue;
        }
        
        // Execute all rounds
        int runs = execute_team_rounds(teams_dir, dir->d_name);
        
        // Validate logs were created
        validate_team_logs(dir->d_name, MAPS_COUNT * REPS_PER_MAP);
        
        if(runs > 0) {
            successful_teams++;
            // Consolidate team stats to central file
            consolidate_team_stats(teams_dir, dir->d_name, stats_file);
        } else {
            printf("  [!] Warning: Team had 0 successful runs\n");
        }
    }
    
    closedir(d);
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    if(dry_run) {
        printf("Dry run completed!\n");
        printf("Would process %d team(s)\n", team_count);
    } else {
        printf("Competition completed!\n");
        printf("Total teams: %d\n", team_count);
        printf("Successful teams: %d\n", successful_teams);
    }
    printf("═══════════════════════════════════════════════════════════════\n");
    
    // Generate and display ranking
    if(!dry_run && successful_teams > 0) {
        display_ranking(stats_file);
    }
    
    return 0;
}
