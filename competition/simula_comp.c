/**
 * Competition Simulator Wrapper
 * 
 * This file wraps the base simulator (simula.c) with competition-specific features:
 * - Disables visualization and debug output
 * - Executes multiple maps in sequence according to config.txt
 * - Accumulates statistics in centralized ../stats.csv file
 * - Automatically rotates through maps and teams
 * 
 * Usage: Compile student's main.c with this file using -DCOMPETITION_MODE flag
 *        gcc -DCOMPETITION_MODE main.c simula_comp.c -lm -o roomba
 */

#define COMPETITION_MODE
#include "../simula.c"

#include <unistd.h>

// Additional competition-specific functions

/**
 * @brief Load a map and execute the competition run
 * 
 * @param filename Path to the map file
 * @param type Map type identifier (0-3)
 */
void execute_map(char* filename, int type) {
    // Store map type for statistics
    extern map_t map;
    map.name = filename;
    
    if(load_map(filename) != 0) {
        fprintf(stderr, "Error loading map: %s\n", filename);
        exit(1);
    }
    
    // Execute the robot behavior
    if(config.on_start != NULL)
        config.on_start();
    
    for(;;)
        config.exec_beh();
}

/**
 * @brief Update config.txt for next round/map rotation
 * 
 * @param rep Current repetition number
 * @param id Current map identifier
 */
void update_config(int rep, int id) {
    FILE *fd = fopen("config.txt", "w");
    if(!fd) return;
    
    rep = (++rep) % 5;  // 5 repetitions per map
    if(rep == 0) id++;   // Move to next map after 5 reps
    
    fprintf(fd, "%d %d", rep, id);
    fclose(fd);
}

/**
 * @brief Save statistics with team ID for competition ranking
 * 
 * Overrides the base save_stats() to include team identification
 * and append to centralized statistics file.
 */
void save_stats_competition() {
    char cwd[128];
    char *team_id;
    int i;
    extern struct _stat stats;
    extern sensor_t hist[];
    extern config_t config;
    extern int timer;
    
    // Get team ID from current directory name
    getcwd(cwd, 128);
    team_id = strrchr(cwd, '/');
    if(team_id) team_id++;
    else team_id = "unknown";
    
    // Calculate mean battery
    stats.bat_mean = 0;
    for(i = 0; i < config.exec_time && i < timer; i++)
        stats.bat_mean += hist[i].battery;
    if(timer > 0)
        stats.bat_mean /= timer;
    
    // Append to centralized stats file
    FILE *fd = fopen(STATS_FILE, "a");
    if(!fd) return;
    
    // Read current map type (stored in config.txt)
    int rep, map_type = 0;
    FILE *cfg = fopen("config.txt", "r");
    if(cfg) {
        fscanf(cfg, "%d%d", &rep, &map_type);
        fclose(cfg);
    }
    
    // Write statistics line
    fprintf(fd, "%s,%d,%d,%d,%d,%d,%.1f,%.1f,%d,%d,%d,%d,%d\n",
        team_id, map_type,
        stats.cell_total, stats.cell_visited,
        stats.dirt_total, stats.dirt_cleaned,
        stats.bat_total, stats.bat_mean,
        stats.moves[FWD], stats.moves[TURN], 
        stats.moves[BUMP], stats.moves[CLE], stats.moves[LOAD]);
    
    fclose(fd);
}

/**
 * @brief Main competition execution function
 * 
 * Reads config.txt to determine which map to run,
 * executes the simulation, and updates config for next run.
 */
void run_competition() {
    char *maps[] = {
        "../maps/noobs.pgm", 
        "../maps/random1.pgm",
        "../maps/random3.pgm", 
        "../maps/random5.pgm"
    };
    int rep, id;
    
    // Read configuration
    FILE *fd = fopen("config.txt", "r");
    if(!fd) {
        fprintf(stderr, "Error: config.txt not found\n");
        exit(1);
    }
    fscanf(fd, "%d%d", &rep, &id);
    fclose(fd);
    
    // Validate map ID
    if(id < 0 || id >= 4) {
        fprintf(stderr, "Competition completed or invalid map ID: %d\n", id);
        exit(0);
    }
    
    // Execute map
    execute_map(maps[id], id);
    
    // Update config for next run
    update_config(rep, id);
}

/**
 * @brief Override run() function for competition mode
 * 
 * Students call run() in their code, but in competition mode
 * it executes the competition sequence instead.
 */
#ifdef run
#undef run
#endif

void run() {
    run_competition();
}

/**
 * @brief Override save_stats() for competition mode
 */
#ifdef save_stats  
#undef save_stats
#endif

void save_stats() {
    save_stats_competition();
}
