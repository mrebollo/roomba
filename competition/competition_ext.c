/**
 * @file competition_ext.c
 * @brief Competition extensions implementation
 * 
 * Implements competition-specific functionality that extends the base
 * simulator without modifying simula.c.
 * 
 * @author IPR-GIIROB-ETSINF-UPV
 * @date 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "competition_ext.h"
#include "../simula_internal.h"

/* ============================================================================
 * GLOBAL CONFIGURATION
 * ============================================================================ */

static const char* g_maps_dir = "../maps";
static const char* g_stats_file = COMP_STATS_FILE;

/* ============================================================================
 * EXTERNAL SYMBOLS FROM SIMULA.C
 * ============================================================================ */

// Core simulator state (defined in simula.c)
extern map_t map;
extern robot_t r;
extern sensor_t *hist;
extern config_t config;
extern int timer;
extern struct _stat stats;

// Core functions we need
extern int load_map(char* filename);
extern void sim_request_stop(void);

/* ============================================================================
 * PRIVATE HELPER FUNCTIONS
 * ============================================================================ */

/**
 * @brief Get team ID from current working directory
 * 
 * Extracts the last component of the current directory path as team ID.
 * 
 * @return Static buffer with team ID (or "unknown" if extraction fails)
 */
static const char* get_team_id(void) {
    static char cwd[256];
    char *team_id;
    
    if(getcwd(cwd, sizeof(cwd)) == NULL) {
        return "unknown";
    }
    
    team_id = strrchr(cwd, '/');
    return (team_id) ? team_id + 1 : "unknown";
}

/**
 * @brief Update config.txt for next round/map
 * 
 * Manages the rotation through maps and repetitions:
 * - Increments repetition counter
 * - After COMP_REPS_PER_MAP repetitions, moves to next map
 * - Wraps around after all maps completed
 * 
 * @param rep Current repetition number (0-4)
 * @param map_id Current map identifier (0-3)
 */
static void update_config(int rep, int map_id) {
    FILE *fd = fopen("config.txt", "w");
    if(!fd) {
        fprintf(stderr, "Warning: Cannot update config.txt\n");
        return;
    }
    
    // Increment repetition, wrap at COMP_REPS_PER_MAP
    rep = (rep + 1) % COMP_REPS_PER_MAP;
    
    // If we wrapped reps, move to next map
    if(rep == 0) {
        map_id++;
    }
    
    fprintf(fd, "%d %d\n", rep, map_id);
    fclose(fd);
}

/* ============================================================================
 * PUBLIC FUNCTIONS IMPLEMENTATION
 * ============================================================================ */

/**
 * @brief Inicializa la configuración global de la competición
 * @param maps_dir Directorio de mapas
 * @param stats_file Archivo de estadísticas
 */
void competition_init(const char* maps_dir, const char* stats_file) {
    if(maps_dir) {
        g_maps_dir = maps_dir;
    }
    if(stats_file) {
        g_stats_file = stats_file;
    }
}

/**
 * @brief Obtiene el identificador del equipo actual
 * @return Cadena con el ID del equipo
 */
const char* competition_get_team_id(void) {
    return get_team_id();
}

/**
 * @brief Ejecuta la simulación sobre un mapa concreto
 * @param filename Ruta al archivo de mapa
 * @param map_type Identificador del tipo de mapa
 */
void competition_execute_map(const char* filename, int map_type) {
    // Store map type for statistics
    if(filename) {
        snprintf(map.name, sizeof(map.name), "%s", filename);
    } else {
        map.name[0] = '\0';
    }
    
    // Load the map
    if(load_map((char*)filename) != 0) {
        fprintf(stderr, "Error: Cannot load map %s\n", filename);
        exit(1);
    }
    
    // Execute initialization callback if provided
    if(config.on_start != NULL) {
        config.on_start();
    }
    
    // Execute behavior loop until time expires or stop requested
    while(timer < config.exec_time) {
        config.exec_beh();
        
        // Check for early termination
        extern int sim_should_stop;
        if(sim_should_stop) break;
    }
}

/**
 * @brief Ejecuta el ciclo completo de competición (multi-mapa y repeticiones)
 */
void run_competition(void) {
    // Map list for competition
    char *maps[] = {
        "../maps/noobs.pgm", 
        "../maps/random1.pgm",
        "../maps/random3.pgm", 
        "../maps/random5.pgm"
    };
    
    int rep, map_id;
    
    // Read current configuration
    FILE *fd = fopen("config.txt", "r");
    if(!fd) {
        fprintf(stderr, "Error: config.txt not found\n");
        fprintf(stderr, "Please create config.txt with: echo \"0 0\" > config.txt\n");
        exit(1);
    }
    
    if(fscanf(fd, "%d %d", &rep, &map_id) != 2) {
        fprintf(stderr, "Error: Invalid config.txt format\n");
        fprintf(stderr, "Expected: <rep> <map_id> (e.g., \"0 0\")\n");
        fclose(fd);
        exit(1);
    }
    fclose(fd);
    
    // Validate map ID
    if(map_id < 0 || map_id >= COMP_MAPS_COUNT) {
        fprintf(stderr, "Competition cycle completed (map_id=%d)\n", map_id);
        fprintf(stderr, "Total runs: %d maps × %d reps = %d runs\n", 
                COMP_MAPS_COUNT, COMP_REPS_PER_MAP, 
                COMP_MAPS_COUNT * COMP_REPS_PER_MAP);
        exit(0);
    }
    
    // Build map path
    char map_path[256];
    if(g_maps_dir) {
        snprintf(map_path, sizeof(map_path), "%s/%s", 
                 g_maps_dir, strrchr(maps[map_id], '/') + 1);
    } else {
        strncpy(map_path, maps[map_id], sizeof(map_path) - 1);
    }
    
    // Execute the map
    competition_execute_map(map_path, map_id);
    
    // Update config for next run
    update_config(rep, map_id);
}

/**
 * @brief Guarda las estadísticas de la ejecución en el archivo centralizado
 */
void save_stats_competition(void) {
    const char *team_id;
    int i;
    
    // Get team identifier
    team_id = get_team_id();
    
    // Calculate mean battery
    stats.bat_mean = 0;
    for(i = 0; i < config.exec_time && i < timer; i++) {
        stats.bat_mean += hist[i].battery;
    }
    if(timer > 0) {
        stats.bat_mean /= timer;
    }
    
    // Open centralized stats file in append mode
    FILE *fd = fopen(g_stats_file, "a");
    if(!fd) {
        fprintf(stderr, "Warning: Cannot open stats file: %s\n", g_stats_file);
        return;
    }
    
    // Read current map type from config.txt
    int rep, map_type = 0;
    FILE *cfg = fopen("config.txt", "r");
    if(cfg) {
        fscanf(cfg, "%d %d", &rep, &map_type);
        fclose(cfg);
    }
    
    // Write statistics line with team ID and map type
    // Format: team,map_type,cell_total,cell_visited,dirt_total,dirt_cleaned,
    //         bat_total,bat_mean,forward,turn,bumps,clean,load
    fprintf(fd, "%s,%d,%d,%d,%d,%d,%.1f,%.1f,%d,%d,%d,%d,%d\n",
        team_id, map_type,
        stats.cell_total, stats.cell_visited,
        stats.dirt_total, stats.dirt_cleaned,
        stats.bat_total, stats.bat_mean,
        stats.moves[FWD], stats.moves[TURN], 
        stats.moves[BUMP], stats.moves[CLEAN], stats.moves[LOAD]);
    
    fclose(fd);
}
