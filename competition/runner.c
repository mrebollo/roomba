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

#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h> // For access()

#define MAX_TEAMS 100    ///< Número máximo de equipos permitidos
#define TEAM_NAME_LEN 64 ///< Longitud máxima del nombre de equipo

// Default values (if config file is missing)
#define DEFAULT_MAPS_COUNT 8
#define DEFAULT_REPS_PER_MAP 5
#define DEFAULT_TIMEOUT 30
#define DEFAULT_LOGS_DIR "logs"
#define DEFAULT_MAPS_DIR "maps"

/**
 * @brief Configuración del Runner
 */
typedef struct {
  int maps_count;
  int reps_per_map;
  int timeout_seconds;
  int max_ticks;
  char maps_dir[256];
  char logs_dir[256];
} runner_config_t;

runner_config_t runner_cfg;

/**
 * @brief Carga la configuración desde runner.conf
 */
void load_runner_config(const char *filename) {
  // Set defaults
  runner_cfg.maps_count = DEFAULT_MAPS_COUNT;
  runner_cfg.reps_per_map = DEFAULT_REPS_PER_MAP;
  runner_cfg.timeout_seconds = DEFAULT_TIMEOUT;
  strcpy(runner_cfg.maps_dir, DEFAULT_MAPS_DIR);
  strcpy(runner_cfg.logs_dir, DEFAULT_LOGS_DIR);
  runner_cfg.max_ticks = 2000; // Valor por defecto si no se encuentra en runner.conf

  FILE *f = fopen(filename, "r");
  if (!f) {
    printf("ℹ️  Config file '%s' not found. Using defaults.\n", filename);
    return;
  }

  char line[256];
  while (fgets(line, sizeof(line), f)) {
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
      continue;

    char key[128], val[128];
    if (sscanf(line, "%127[^=]=%127s", key, val) == 2) {
      // Trim whitespace (simple version)
      char *k = key;
      while (*k == ' ')
        k++;
      char *v = val;
      while (*v == ' ')
        v++;

      // Remove trailing keys whitespace? sscanf %s stops at space, so val is
      // likely clean if no internal spaces.

      if (strcmp(k, "maps_count ") == 0 || strcmp(k, "maps_count") == 0) {
        runner_cfg.maps_count = atoi(v);
      } else if (strcmp(k, "reps_per_map ") == 0 ||
                 strcmp(k, "reps_per_map") == 0) {
        runner_cfg.reps_per_map = atoi(v);
      } else if (strcmp(k, "timeout_seconds ") == 0 ||
                 strcmp(k, "timeout_seconds") == 0) {
        runner_cfg.timeout_seconds = atoi(v);
      } else if (strcmp(k, "maps_dir ") == 0 || strcmp(k, "maps_dir") == 0) {
        strcpy(runner_cfg.maps_dir, v);
      } else if (strcmp(k, "logs_dir ") == 0 || strcmp(k, "logs_dir") == 0) {
        strcpy(runner_cfg.logs_dir, v);
      } else if (strcmp(k, "max_ticks ") == 0 || strcmp(k, "max_ticks") == 0) {
        runner_cfg.max_ticks = atoi(v);
      }
    }
  }
  fclose(f);
    printf("✓ Loaded config: %d maps, %d reps, %ds timeout, %d max_ticks from %s\n",
      runner_cfg.maps_count, runner_cfg.reps_per_map,
      runner_cfg.timeout_seconds, runner_cfg.max_ticks, filename);
}

/**
 * @brief Estructura para almacenar los resultados de cada equipo
 */
typedef struct {
  char name[TEAM_NAME_LEN]; ///< Nombre del equipo
  int total_cells_visited;  ///< Total de celdas visitadas
  int total_dirt_cleaned;   ///< Total de suciedad limpiada
  float total_battery_used; ///< Batería total consumida
  int total_bumps;          ///< Total de colisiones
} team_result_t;

/**
 * @brief Inicializa el archivo stats.csv con cabeceras
 * @param filename Ruta al archivo de estadísticas
 */
void init_stats_file(const char *filename) {
  FILE *fd = fopen(filename, "w");
  if (!fd) {
    fprintf(stderr, "Error creating stats file\n");
    exit(1);
  }

  fprintf(fd, "team,map_type,cell_total,cell_visited,dirt_total,dirt_cleaned,"
              "bat_total,bat_mean,forward,turn,bumps,clean,load\n");
  fclose(fd);

  printf("✓ Statistics file initialized: %s\n", filename);
}

/**
 * @brief Verifica el resultado de ejecución y registra información de crash
 * @param status Código de estado del proceso
 * @param stderr_file Archivo de log de errores
 * @return Cadena con el estado: "success", "error", "crash", "stopped",
 * "unknown"
 */
const char *check_execution_result(int status, const char *stderr_file) {
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
      switch (signal) {
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

// ... (previous functions unchaged)

/**
 * @brief Inicializa el directorio de logs
 */
void init_logs_dir(void) {
  struct stat st = {0};

  // Create logs directory if it doesn't exist
  if (stat(runner_cfg.logs_dir, &st) == -1) {
    mkdir(runner_cfg.logs_dir, 0755);
  }

  printf("✓ Logs directory ready: %s/\n", runner_cfg.logs_dir);
}

/**
 * @brief Verifica si una entrada de directorio es un equipo válido
 * @param base_path Ruta base
 * @param entry Entrada de directorio
 * @return 1 si es equipo válido, 0 si no
 */
int is_team_dir(const char *base_path, struct dirent *entry) {
  if (entry->d_type != DT_DIR)
    return 0;

  if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
    return 0;

  // Check if main.c exists in the directory
  char main_path[512];
  snprintf(main_path, sizeof(main_path), "%s/%s/main.c", base_path,
           entry->d_name);

  struct stat st;
  return (stat(main_path, &st) == 0);
}

/**
 * @brief Compila el código de un equipo con el simulador de competición
 * @param teams_dir Directorio de equipos
 * @param team_name Nombre del equipo
 * @return 0 si OK, -1 si error
 */
int compile_team(const char *teams_dir, const char *team_name) {
  char cmd[2048];
  char comp_path[256];
  char main_file[256] = "main.c";

  // Get absolute path to competition directory
  getcwd(comp_path, sizeof(comp_path));

  // Check if main.c exists, otherwise look for any .c file
  char test_path[512];
  snprintf(test_path, sizeof(test_path), "%s/%s/main.c", teams_dir, team_name);

  if (access(test_path, F_OK) != 0) {
    // Look for any .c file
    DIR *dir;
    struct dirent *entry;
    char dir_path[512];

    snprintf(dir_path, sizeof(dir_path), "%s/%s", teams_dir, team_name);
    dir = opendir(dir_path);

    if (dir) {
      while ((entry = readdir(dir)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len > 2 && strcmp(entry->d_name + len - 2, ".c") == 0) {
          strncpy(main_file, entry->d_name, sizeof(main_file) - 1);
          break;
        }
      }
      closedir(dir);
    }
  }

  // Use precompiled object file from lib/
  // Add -DCOMPETITION_MODE to disable visualization
    snprintf(
      cmd, sizeof(cmd),
      "cd %s/%s && gcc -Wall -Wno-unused-function -DCOMPETITION_MODE -DCOMPETITION_EXEC_TIME=%d -I%s/.. "
      "%s %s/lib/simula.o "
      "%s/competition_ext.c %s/simula_comp.c -lm -o roomba 2>&1",
      teams_dir, team_name, runner_cfg.max_ticks, comp_path, main_file, comp_path, comp_path, comp_path);

  printf("  Compiling %s... ", team_name);
  fflush(stdout);

  int result = system(cmd);

  if (result == 0) {
    printf("✓\n");
    return 0;
  } else {
    printf("✗ FAILED\n");
    return -1;
  }
}

/**
 * @brief Inicializa el archivo config.txt para un equipo
 * @param teams_dir Directorio de equipos
 * @param team_name Nombre del equipo
 */
void init_team_config(const char *teams_dir, const char *team_name) {
  char config_path[512];
  snprintf(config_path, sizeof(config_path), "%s/%s/config.txt", teams_dir,
           team_name);

  FILE *fd = fopen(config_path, "w");
  if (fd) {
    fprintf(fd, "0 0"); // Start with rep 0, map 0
    fclose(fd);
  }
}

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
  for (int i = 0; i < bar_width; i++) {
    if (i < filled) {
      printf("#");
    } else {
      printf("-");
    }
  }
  printf("] %3d%% (%d/%d)", (int)(progress * 100), current, total);
  fflush(stdout);
}

/**
 * @brief Obtiene la lista de mapas oficiales
 * @param maps_dir Directorio de mapas
 * @param map_files Array para guardar los nombres de archivo
 * @param max_maps Máximo número de mapas a leer
 * @return Número de mapas encontrados
 */
int get_official_maps(const char *maps_dir, char map_files[][256],
                      int max_maps) {
  DIR *d = opendir(maps_dir);
  if (!d)
    return 0;

  struct dirent *entry;
  int count = 0;

  // Buscar archivos .pgm
  while ((entry = readdir(d)) != NULL && count < max_maps) {
    size_t len = strlen(entry->d_name);
    if (len > 4 && strcmp(entry->d_name + len - 4, ".pgm") == 0) {
      strncpy(map_files[count], entry->d_name, 255);
      count++;
    }
  }

  closedir(d);

  // Si no hay mapas, advertir pero permitir continuar (quizás fallback a
  // random)
  if (count == 0) {
    printf("  [!] Warning: No .pgm maps found in %s/\n", maps_dir);
  } else {
    // Ordenar alfabéticamente para consistencia
    for (int i = 0; i < count - 1; i++) {
      for (int j = i + 1; j < count; j++) {
        if (strcmp(map_files[i], map_files[j]) > 0) {
          char temp[256];
          strcpy(temp, map_files[i]);
          strcpy(map_files[i], map_files[j]);
          strcpy(map_files[j], temp);
        }
      }
    }
  }

  return count;
}

/**
 * @brief Ejecuta todas las rondas para un equipo
 * @param teams_dir Directorio de equipos
 * @param team_name Nombre del equipo
 * @return Número de ejecuciones exitosas
 */
int execute_team_rounds(const char *teams_dir, const char *team_name) {
  char cmd[1024];
  char log_stdout[256];
  char log_stderr[256];
  char team_stats[512];

// Allocate map files dynamically or use a max constant
// Reusing MAX_TEAMS as a safe upper bound for maps for now or defining MAX_MAPS
#define MAX_MAPS 100
  char map_files_buf[MAX_MAPS][256];


  // Obtener mapas oficiales
  int available_maps = get_official_maps(runner_cfg.maps_dir, map_files_buf,
                                         runner_cfg.maps_count);
  int maps_to_run = (available_maps > 0) ? available_maps : runner_cfg.maps_count;

  int total_runs = maps_to_run * runner_cfg.reps_per_map;
  int successful_runs = 0;
  int crashes = 0;
  int errors = 0;

  // ...existing code...

  // Initialize team's stats.csv with header
  snprintf(team_stats, sizeof(team_stats), "%s/%s/stats.csv", teams_dir, team_name);
  FILE *stats_fd = fopen(team_stats, "w");
  if (stats_fd) {
    fprintf(stats_fd,
            "cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, "
            "bat_mean, forward, turn, bumps, clean, load\n");
    fclose(stats_fd);
  }

  // Abrir log de experimentos
  FILE *exp_log = fopen("runner_experiments.log", "a");
  if (!exp_log) {
      fprintf(stderr, "Error opening runner_experiments.log\n");
  }

  printf("  Executing %d rounds for %s:\n", total_runs, team_name);


  for (int run = 0; run < total_runs; run++) {
    int map_idx = run / runner_cfg.reps_per_map;
    int rep = run % runner_cfg.reps_per_map;

    // Prepare map command part
    char map_arg[300] = "";

    if (available_maps > 0) {
      // Copiar el mapa oficial
      snprintf(cmd, sizeof(cmd), "cp %s/%s %s/%s/map.pgm", runner_cfg.maps_dir,
               map_files_buf[map_idx], teams_dir, team_name);
      system(cmd);

      // Argumento para ./roomba será "map.pgm"
      strcpy(map_arg, "map.pgm");
    } else {
      map_arg[0] = '\0';
    }

    // Create log file paths
    snprintf(log_stdout, sizeof(log_stdout), "%s/%s_map%d_run%d.stdout",
             runner_cfg.logs_dir, team_name, map_idx, rep);
    snprintf(log_stderr, sizeof(log_stderr), "%s/%s_map%d_run%d.stderr",
             runner_cfg.logs_dir, team_name, map_idx, rep);

    // Execute with logging and crash detection
    // Using configured timeout could be implemented with 'timeout' command if
    // installed
    snprintf(cmd, sizeof(cmd),
             "cd %s/%s && VISUAL=0 ./roomba %s > ../../%s 2> ../../%s",
             teams_dir, team_name, map_arg, log_stdout, log_stderr);

    // Show progress bar
    printf("\r");
    print_progress_bar(run, total_runs, 30);
    printf(" Map %d Rep %d", map_idx, rep);
    if (available_maps > 0)
      printf(" [%s] ", map_files_buf[map_idx]);
    fflush(stdout);

    int result = system(cmd);
    const char *status = check_execution_result(result, log_stderr);

    // Log de experimento
    if (exp_log) {
      fprintf(exp_log, "TEAM=%s MAP=%s IDX=%d REP=%d STATUS=%s\n",
        team_name,
        (available_maps > 0) ? map_files_buf[map_idx] : "N/A",
        map_idx, rep, status);
      fflush(exp_log);
    }

    if (strcmp(status, "success") == 0) {
      successful_runs++;
    } else if (strcmp(status, "crash") == 0) {
      crashes++;
    } else if (strcmp(status, "error") == 0) {
      errors++;
    }

    // Limpieza de mapa copiado
    if (available_maps > 0) {
      snprintf(cmd, sizeof(cmd), "rm -f %s/%s/map.pgm", teams_dir, team_name);
      system(cmd);
    }
  }

  // Cerrar log de experimentos
  if (exp_log) fclose(exp_log);

  // Final progress bar
  printf("\r");
  print_progress_bar(total_runs, total_runs, 30);
  printf(" [OK] Done\n");

  // Summary
  printf("  Results: %d OK, %d CRASH, %d ERROR (Total: %d/%d successful)\n\n",
         successful_runs, crashes, errors, successful_runs, total_runs);

  return successful_runs;
}

/**
 * @brief Valida que todos los archivos de log esperados existen
 * @param team_name Nombre del equipo
 * @param total_runs Número total de ejecuciones
 * @return Número de logs faltantes
 */
int validate_team_logs(const char *team_name, int total_runs) {
  int missing = 0;

  for (int run = 0; run < total_runs; run++) {
    int map_id = run / runner_cfg.reps_per_map;
    int rep = run % runner_cfg.reps_per_map;

    char log_stdout[256];
    char log_stderr[256];
    snprintf(log_stdout, sizeof(log_stdout), "%s/%s_map%d_run%d.stdout",
             runner_cfg.logs_dir, team_name, map_id, rep);
    snprintf(log_stderr, sizeof(log_stderr), "%s/%s_map%d_run%d.stderr",
             runner_cfg.logs_dir, team_name, map_id, rep);

    if (access(log_stdout, F_OK) != 0 || access(log_stderr, F_OK) != 0) {
      missing++;
    }
  }

  if (missing > 0) {
    printf("  [!] Warning: %d log files missing\n", missing);
  }

  return missing;
}

/**
 * @brief Consolida las estadísticas de un equipo en el archivo central
 */
void consolidate_team_stats(const char *teams_dir, const char *team_name,
                            const char *central_stats) {
  char team_stats_file[512];
  snprintf(team_stats_file, sizeof(team_stats_file), "%s/%s/stats.csv",
           teams_dir, team_name);

  FILE *src = fopen(team_stats_file, "r");
  if (!src) {
    printf("  [!] Warning: No stats.csv found for %s\n", team_name);
    return;
  }

  FILE *dest = fopen(central_stats, "a");
  if (!dest) {
    fclose(src);
    return;
  }

  // Skip header line in source
  char line[1024];
  if (fgets(line, sizeof(line), src) == NULL) {
    fclose(src);
    fclose(dest);
    return;
  }

  // Copy all data lines, prepending team name
  int line_num = 0;
  while (fgets(line, sizeof(line), src)) {
    // Each line is a different execution, calculate map type
    int map_type = line_num / runner_cfg.reps_per_map;
    fprintf(dest, "%s,%d,%s", team_name, map_type, line);
    line_num++;
  }

  fclose(src);
  fclose(dest);
}

// ... (previous functions including consolidate_team_stats)

/**
 * @brief Calcula las puntuaciones de los equipos a partir de stats.csv
 * @param stats_file Archivo de estadísticas
 * @param results Array de resultados de equipos
 * @param team_count Puntero a número de equipos
 */
void calculate_rankings(const char *stats_file, team_result_t results[],
                        int *team_count) {
  FILE *fd = fopen(stats_file, "r");
  if (!fd) {
    fprintf(stderr, "Error opening stats file for ranking\n");
    return;
  }

  char line[512];
  fgets(line, sizeof(line), fd); // Skip header

  *team_count = 0;

  while (fgets(line, sizeof(line), fd)) {
    char team_name[TEAM_NAME_LEN];
    int map_type, cell_total, cell_visited, dirt_total, dirt_cleaned;
    float bat_total, bat_mean;
    int forward, turn, bumps, clean, load;

    sscanf(line, "%[^,],%d,%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d", team_name,
           &map_type, &cell_total, &cell_visited, &dirt_total, &dirt_cleaned,
           &bat_total, &bat_mean, &forward, &turn, &bumps, &clean, &load);

    // Find or create team entry
    int team_idx = -1;
    for (int i = 0; i < *team_count; i++) {
      if (strcmp(results[i].name, team_name) == 0) {
        team_idx = i;
        break;
      }
    }

    if (team_idx == -1) {
      team_idx = (*team_count)++;
      strcpy(results[team_idx].name, team_name);
      results[team_idx].total_cells_visited = 0;
      results[team_idx].total_dirt_cleaned = 0;
      results[team_idx].total_battery_used = 0;
      results[team_idx].total_bumps = 0;
    }

    // Accumulate statistics
    results[team_idx].total_cells_visited += cell_visited;
    results[team_idx].total_dirt_cleaned += dirt_cleaned;
    results[team_idx].total_battery_used += bat_total;
    results[team_idx].total_bumps += bumps;
  }

  fclose(fd);
}

/**
 * @brief Función de comparación para ordenar equipos por puntuación
 * @param a Puntero al primer equipo
 * @param b Puntero al segundo equipo
 * @return Diferencia de puntuación (descendente)
 */
int compare_teams(const void *a, const void *b) {
  team_result_t *ta = (team_result_t *)a;
  team_result_t *tb = (team_result_t *)b;
  // Orden descendente por celdas visitadas
  return tb->total_cells_visited - ta->total_cells_visited;
}

/**
 * @brief Genera y muestra el ranking de la competición
 * @param stats_file Archivo de estadísticas
 */
void display_ranking(const char *stats_file) {
  team_result_t results[MAX_TEAMS];
  int team_count = 0;

  calculate_rankings(stats_file, results, &team_count);

  if (team_count == 0) {
    printf("No results to display\n");
    return;
  }

  // Sort teams by score
  qsort(results, team_count, sizeof(team_result_t), compare_teams);

  // Display ranking
  printf("\n");
  printf("═══════════════════════════════════════════════════════════════\n");
  printf("                    COMPETITION RANKING (TOTALS)                     "
         "  \n");
  printf("═══════════════════════════════════════════════════════════════\n");
  printf("Rank  Team            Cells    Dirt    Battery   Bumps\n");
  printf("───────────────────────────────────────────────────────────────\n");

  for (int i = 0; i < team_count; i++) {
    printf("%-5d %-15s %-8d %-8d %-9.1f %-6d\n", i + 1, results[i].name,
           results[i].total_cells_visited, results[i].total_dirt_cleaned,
           results[i].total_battery_used, results[i].total_bumps);
  }

  printf("═══════════════════════════════════════════════════════════════\n\n");

  // Save to file
  FILE *fd = fopen("ranking.txt", "w");
  if (fd) {
    fprintf(fd, "ROOMBA COMPETITION RANKING (TOTALS)\n\n");
    fprintf(fd, "Rank  Team            Cells    Dirt    Battery   Bumps\n");
    fprintf(fd,
            "-----------------------------------------------------------\n");
    for (int i = 0; i < team_count; i++) {
      fprintf(fd, "%-5d %-15s %-8d %-8d %-9.1f %-6d\n", i + 1, results[i].name,
              results[i].total_cells_visited, results[i].total_dirt_cleaned,
              results[i].total_battery_used, results[i].total_bumps);
    }
    fclose(fd);
    printf("Ranking saved to ranking.txt\n");
  }
}

/**
 * @brief Función principal del runner de competición
 */
int main(int argc, char *argv[]) {
  const char *teams_dir = "teams";
  const char *stats_file = "stats.csv";
  int dry_run = 0;
  const char *single_team = NULL;

  // Load configuration first
  load_runner_config("runner.conf");

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--dry-run") == 0) {
      dry_run = 1;
    } else if (strncmp(argv[i], "--team=", 7) == 0) {
      single_team = argv[i] + 7;
    } else if (argv[i][0] != '-') {
      teams_dir = argv[i];
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      fprintf(stderr, "Usage: %s [teams_dir] [--dry-run] [--team=XX]\n",
              argv[0]);
      return 1;
    }
  }


  printf("\n");
  printf("═══════════════════════════════════════════════════════════════\n");
  printf("              ROOMBA COMPETITION RUNNER                        \n");
  printf("═══════════════════════════════════════════════════════════════\n");

  // Mostrar información de contexto del experimento (global, no por equipo)
  printf("Configuration Loaded:\n");
  printf("  Maps Count: %d\n", runner_cfg.maps_count);
  printf("  Reps per Map: %d\n", runner_cfg.reps_per_map);
  printf("  Maps Directory: %s\n", runner_cfg.maps_dir);
  printf("  Logs Directory: %s\n", runner_cfg.logs_dir);
  printf("  Timeout: %ds\n", runner_cfg.timeout_seconds);
  printf("  Max Ticks: %d\n", runner_cfg.max_ticks);
  if (dry_run) {
    printf("[DRY RUN MODE - No execution, just showing what would run]\n");
  }
  if (single_team) {
    printf("Single team mode: %s\n", single_team);
  }
  printf("Teams directory: %s\n", teams_dir);

  // Listar mapas detectados
  char map_files_buf[100][256];
  int available_maps = get_official_maps(runner_cfg.maps_dir, map_files_buf, runner_cfg.maps_count);
  printf("  Mapas detectados en '%s': %d\n", runner_cfg.maps_dir, available_maps);
  for (int i = 0; i < available_maps; i++) {
    printf("    [%d] %s\n", i, map_files_buf[i]);
  }
  printf("  Repeticiones por mapa: %d\n", runner_cfg.reps_per_map);
  printf("  Total de ejecuciones por equipo: %d\n", (available_maps > 0 ? available_maps : runner_cfg.maps_count) * runner_cfg.reps_per_map);
  printf("\n");

  // Check if simula.o exists, if not build it
  if (access("lib/simula.o", F_OK) != 0) {
    printf("⚙️  Building competition library...\n");
    int result = system("cd .. && make lib-competition > /dev/null 2>&1");
    if (result != 0) {
      fprintf(stderr, "❌ Failed to build competition library\n");
      fprintf(
          stderr,
          "   Please run 'make lib-competition' manually from project root\n");
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
  if (!d) {
    fprintf(stderr, "Error: Cannot open teams directory: %s\n", teams_dir);
    return 1;
  }

  // Process each team
  struct dirent *dir;
  int team_count = 0;
  int successful_teams = 0;

  printf("Discovering teams...\n");
  while ((dir = readdir(d)) != NULL) {
    if (!is_team_dir(teams_dir, dir))
      continue;

    // Filter by single team if specified
    if (single_team && strcmp(dir->d_name, single_team) != 0)
      continue;

    team_count++;
    printf("\n[Team %d: %s]\n", team_count, dir->d_name);

    if (dry_run) {
      printf("  [DRY RUN] Would compile and execute %d rounds\n",
             runner_cfg.maps_count * runner_cfg.reps_per_map);
      continue;
    }

    // Initialize team config
    init_team_config(teams_dir, dir->d_name);

    // Compile team code
    if (compile_team(teams_dir, dir->d_name) != 0) {
      printf("  Skipping due to compilation errors\n");
      continue;
    }

    // Execute all rounds
    int runs = execute_team_rounds(teams_dir, dir->d_name);

    // Validate logs were created
    validate_team_logs(dir->d_name,
                       runner_cfg.maps_count * runner_cfg.reps_per_map);

    if (runs > 0) {
      successful_teams++;
      // Consolidate team stats to central file
      consolidate_team_stats(teams_dir, dir->d_name, stats_file);
    } else {
      printf("  [!] Warning: Team had 0 successful runs\n");
    }
  }

  closedir(d);

  printf("\n═══════════════════════════════════════════════════════════════\n");
  if (dry_run) {
    printf("Dry run completed!\n");
    printf("Would process %d team(s)\n", team_count);
  } else {
    printf("Competition completed!\n");
    printf("Total teams: %d\n", team_count);
    printf("Successful teams: %d\n", successful_teams);
  }
  printf("═══════════════════════════════════════════════════════════════\n");

  // Generate and display ranking
  if (!dry_run && successful_teams > 0) {
    display_ranking(stats_file);
  }

  return 0;
}
