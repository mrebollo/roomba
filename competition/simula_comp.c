/**
 * @file simula_comp.c
 * @brief Competition mode function overrides
 * 
 * This small wrapper file overrides run() and save_stats() to use
 * competition-specific versions. Must be compiled AFTER simula.c
 * and competition_ext.c to properly override the default functions.
 * 
 * Compilation order:
 *   gcc main.c simula.c competition_ext.c simula_comp.c -lm -o roomba
 * 
 * Or with precompiled libraries:
 *   gcc main.c simula.o competition_ext.o simula_comp.o -lm -o roomba
 * 
 * Architecture:
 * - Students call run() and save_stats() normally in main.c
 * - These calls are intercepted and redirected to competition versions
 * - No changes needed to student code
 * 
 * @author IPR-GIIROB-ETSINF-UPV
 * @date 2025
 */

#include "competition_ext.h"

/* ============================================================================
 * FUNCTION OVERRIDES FOR COMPETITION MODE
 * ============================================================================ */

/**
 * @brief Override run() with competition version
 * 
 * When student code calls run(), this version executes instead.
 * Manages multi-map execution according to config.txt.
 */
void run(void) __attribute__((weak));
void run(void) {
    run_competition();
}

/**
 * @brief Override save_stats() with competition version
 * 
 * When student code (or atexit) calls save_stats(), this version executes.
 * Saves stats with team ID and map type to centralized file.
 */
void save_stats(void) __attribute__((weak));
void save_stats(void) {
    save_stats_competition();
}
