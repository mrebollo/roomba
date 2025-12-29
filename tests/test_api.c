#include "../simula.h"
#include "../sim_world_api.h"
#include "../simula_internal.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


void setUp(void) {
    // Reset robot awake state before each test
    rmb_reset_awake();
}

void tearDown(void) {}


//-----------------------------------
// WRONG MAPS
//-----------------------------------

void test_load_map_base_in_wall_should_fail(void) {
    // Base at corner (0,0), which is a wall
    int res = system("./mapgen maps/badbase.pgm 50 0 0");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map with base in wall generation failed");
    res = load_map("maps/badbase.pgm");
    TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(0, res, "Map loading should fail"); // Should fail
}


void test_load_map_no_base_should_fail(void) {
    // Map without base
    int res = system("./mapgen maps/nobase.pgm 50");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map without base generation failed");
    res = load_map("maps/nobase.pgm");
    TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(0, res, "Map loading should fail"); // Should fail
}

//-----------------------------------
// CORRECT MAP
//-----------------------------------

void test_awake_on_empty_map(void) {
    // Generate a 50x50 map with base at (2,3)
    int res = system("./mapgen maps/empty.pgm 50 2 3");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Empty map generation failed");

    // Load the generated map
    res = load_map("maps/empty.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map loading failed");
    // Test rmb_awake
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully");
    // Change the expected values according to the base position in the map
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, x, "Robot X position incorrect");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, y, "Robot Y position incorrect");
}

void test_map_8x8(void) {
    // Generar el mapa 8x8 con base en (1,1)
    int ret = system("./mapgen maps/map8x8.pgm 8 1 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Map 8x8 generation failed");
    int res = load_map("maps/map8x8.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map 8x8 loading failed");
    // Comprobar rmb_awake
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully on 8x8 map");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, x, "Robot X position incorrect on 8x8 map");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, y, "Robot Y position incorrect on 8x8 map");
}

//-----------------------------------
// ROBOT AWAKEN TESTS
//-----------------------------------

void test_awake_without_coordinates_should_fail(void) {
    int ok = rmb_awake(NULL, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ok, "Robot awake should fail with NULL coordinates");
    rmb_reset_awake();
    int x, y;
    ok = rmb_awake(&x, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ok, "Robot awake should fail with NULL y coordinate");
    rmb_reset_awake();
    ok = rmb_awake(NULL, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ok, "Robot awake should fail with NULL x coordinate");
}

void test_awake_twice_should_fail(void) {
    int ret = system("./mapgen maps/map8x8.pgm 8 1 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Map 8x8 generation failed");
    int res = load_map("maps/map8x8.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map 8x8 loading failed");
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully on 8x8 map");
    // Second awake should fail
    ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ok, "Robot should not awake twice");
}

void test_robot_initial_state(void) {
    int ret = system("./mapgen maps/map8x8.pgm 8 1 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Map 8x8 generation failed");
    int res = load_map("maps/map8x8.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map 8x8 loading failed");
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully on 8x8 map");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, rmb_at_base(), "Robot should start at base");
    // Comprobar orientación válida (0 <= heading < 2π)
    sensor_t s = rmb_state();
    float h = s.heading;
    TEST_ASSERT_TRUE_MESSAGE(h >= 0.0f && h < 2*M_PI, "Heading out of range"); // heading
    TEST_ASSERT_TRUE_MESSAGE(s.x == x && s.y == y, "Robot position incorrect");    // position
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, s.bumper, "Bumper should be off");        // bumper
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, s.infrared, "Infrared should be off");      // infrared
    TEST_ASSERT_TRUE_MESSAGE(s.battery > 0.0f, "Battery should be positive");
}


void test_robot_at_north_wall(void) {
    int ret = system("./mapgen maps/north.pgm 8 3 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Map north generation failed");
    int res = load_map("maps/north.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map north loading failed");
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully on 8x8 map");
    // Comprobar orientación válida (base norte -> heading = 3π/2)
    sensor_t s = rmb_state();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(M_PI/2, s.heading, "Robot should face south (3π/2)"); // heading
}


void test_robot_at_south_wall(void) {
    int ret = system("./mapgen maps/south.pgm 8 3 6");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Map south generation failed");
    int res = load_map("maps/south.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map south loading failed");
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully on 8x8 map");
    // Comprobar orientación válida (base sur -> heading = π/2)
    sensor_t s = rmb_state();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3*M_PI/2, s.heading, "Robot should face north (π/2)"); // heading
}


void test_robot_at_east_wall(void) {
    int ret = system("./mapgen maps/east.pgm 8 6 3");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Map east generation failed");
    int res = load_map("maps/east.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map east loading failed");
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully on 8x8 map");
    // Comprobar orientación válida (base este -> heading = π)
    sensor_t s = rmb_state();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(M_PI, s.heading, "Robot should face west (π)"); // heading
}


void test_robot_at_west_wall(void) {
    int ret = system("./mapgen maps/west.pgm 8 1 3");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "Map west generation failed");
    int res = load_map("maps/west.pgm");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "Map west loading failed");
    int x = -1, y = -1;
    int ok = rmb_awake(&x, &y);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ok, "Robot should awake successfully on 8x8 map");
    // Comprobar orientación válida (base oeste -> heading = 0)
    sensor_t s = rmb_state();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, s.heading, "Robot should face east (0)"); // heading
}


//-----------------------------------
// ROBOT DYNAMICS TESTS
//-----------------------------------

// Función auxiliar para inicializar el robot en la base de un mapa generado
void setup_robot_at_base(const char *mapfile, int size, int base_x, int base_y, int *x, int *y) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "./mapgen %s %d %d %d", mapfile, size, base_x, base_y);
    system(cmd);
    // No repetimos asserts aquí, asumimos que la generación y carga funcionan
    load_map(mapfile);
    rmb_awake(x, y);
}

void test_robot_moves_north(void) {
    int x, y;
    setup_robot_at_base("maps/north.pgm", 8, 3, 1, &x, &y); // base en north wall (3,1)
    sensor_t s0 = rmb_state();
    rmb_forward();
    sensor_t s1 = rmb_state();
    // El robot debe haber avanzado una celda al Norte (y creciente, matricial)
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.x, s1.x, "Robot should have the same x");
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.y + 1, s1.y, "Robot should have increased y by 1");
}

void test_robot_moves_south(void) {
    int x, y;
    setup_robot_at_base("maps/south.pgm", 8, 3, 6, &x, &y); // base en south wall (3,6)
    sensor_t s0 = rmb_state();
    rmb_forward();
    sensor_t s1 = rmb_state();
    // El robot debe haber avanzado una celda al Sur (y decreciente, matricial)
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.x, s1.x, "Robot should have the same x");
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.y - 1, s1.y, "Robot should have decreased y by 1");
}
void test_robot_moves_east(void) {
    int x, y;
    setup_robot_at_base("maps/west.pgm", 8, 1, 3, &x, &y); // base en (1,3)
    sensor_t s0 = rmb_state();
    rmb_forward();
    sensor_t s1 = rmb_state();
    // El robot debe haber avanzado una celda al Este
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.x + 1, s1.x, "Robot should have increased x by 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.y, s1.y, "Robot should have the same y");
}

void test_robot_moves_west(void) {
    int x, y;
    setup_robot_at_base("maps/east.pgm", 8, 6, 3, &x, &y); // base en (6,3)
    sensor_t s0 = rmb_state();
    rmb_forward();
    sensor_t s1 = rmb_state();
    // El robot debe haber avanzado una celda al Este
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.x - 1, s1.x, "Robot should have decreased x by 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.y, s1.y, "Robot should have the same y");
}

void test_try_walk_on_empty_cell(void){
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 2, 3, &x, &y); // base en (2,3)
    sensor_t s0 = rmb_state();
    rmb_forward();
    sensor_t s1 = rmb_state();
    // El robot debe haber avanzado una celda al Sur (por descarte)
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.x, s1.x, "Robot should have the same x");
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.y - 1, s1.y, "Robot should have decreased y by 1");
}

void test_try_walk_against_wall(void){
    int x, y;
    // base at distance 2 to north wall and walk twice
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (3,2) -> (3,1) wall
    rmb_forward();
    sensor_t s0 = rmb_state();
    rmb_forward(); // Second move should hit the wall
    sensor_t s1 = rmb_state();
    // El robot no debe haberse movido y tener el bumper activado
    TEST_ASSERT_TRUE_MESSAGE(s0.x == s1.x && s0.y == s1.y, "Robot should have the same position");
    TEST_ASSERT_EQUAL_INT_MESSAGE(s1.bumper, 1, "Robot should have the bumper activated (from sensors)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(rmb_bumper(), 1, "Robot should have the bumper activated (from API)");
}


void test_infrared_on_base(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3
    sensor_t s = rmb_state();
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, s.infrared, "Infrared should read 0 on base");
}

void test_infrared_on_clean_cell(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3) clean cell
    rmb_forward(); // Move to (3,1) clean cell
    sensor_t s = rmb_state();
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, s.infrared, "Infrared should read 0 on clean cell");
}

void test_infrared_on_dirty_cell(void) {
    system("./mapgen maps/map8x8d.pgm 8 3 2 3 1"); // base at (2,3), dirt at (1,3)
    load_map("maps/map8x8d.pgm");
    int x = -1, y = -1;
    rmb_awake(&x, &y);
    rmb_forward(); 
    sensor_t s = rmb_state();
    TEST_ASSERT_GREATER_THAN_INT_MESSAGE(0, s.infrared, "Infrared should read greater than 0 on dirty cell");
}

void test_try_clean_on_clean_cell(void){
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3) clean cell
    rmb_forward(); // Move to (3,1) clean cell
    sensor_t s0 = rmb_state();
    rmb_clean();
    sensor_t s1 = rmb_state();
    // Infrared should remain 0 and battery should not decrease
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, s1.infrared, "Infrared should remain 0 after cleaning clean cell");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(s0.battery, s1.battery, "Battery should not decrease after cleaning clean cell");
}

void test_try_clean_on_dirty_cell(void){
    system("./mapgen maps/map8x8d.pgm 8 3 2 3 1"); // base at (2,3), dirt at (1,3)
    load_map("maps/map8x8d.pgm");
    int x = -1, y = -1;
    rmb_awake(&x, &y);
    rmb_forward(); // Move to dirty cell (3,1)
    sensor_t s0 = rmb_state();
    rmb_clean();
    sensor_t s1 = rmb_state();
    // Infrared should decrease
    TEST_ASSERT_EQUAL_INT_MESSAGE(s0.infrared, s1.infrared + 1, "Infrared should decrease after cleaning dirty cell");
}

void test_try_clean_on_base(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3) clean cell
    rmb_clean();
    sensor_t s = rmb_state();
    // Infrared should remain 0 and battery should not decreas
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, s.infrared, "Infrared should remain 0 after cleaning on base");
}

void test_robot_at_base(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3)
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, rmb_at_base(), "Robot should wake up at base");
    rmb_forward();
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, rmb_at_base(), "Robot should not be at base");
    rmb_turn(M_PI); // Turn around
    rmb_forward(); // Back to base
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, rmb_at_base(), "Robot should be at base again");
}

void test_load_out_of_base_should_fail(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3)
    rmb_forward();
    int loaded = rmb_load();
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, loaded, "Load should fail when not at base");
}

void test_load_at_base(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3)
    sensor_t s0 = rmb_state();
    for(int i = 0; i < 200; i++) 
        rmb_turn(M_PI); // drain battery spinning in place
    sensor_t s1 = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s1.battery < s0.battery, "Battery should have decreased after moves");
    int loaded = rmb_load();
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, loaded, "Load should succeed at base");
    sensor_t s2 = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s2.battery > s1.battery, "Battery should have increased after loading at base");
}



// Tests de giros básicos y libres
void test_turn_updates_heading_correctly(void);

#define rad2grad(r) ((r) * 180.0f / M_PI)
#define EPSILON 1e-5
// Test parametrizable de giro y avance
void test_turn_and_move(float angle, int expected_dx, int expected_dy, int steps) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    // turn south to east (0 rad)
    rmb_turn(M_PI/2);
    sensor_t s = rmb_state();
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(EPSILON, 0.0f, s.heading, "Heading should be 0 ar beginning");
    // Apply turn and move
    rmb_turn(angle);
    s = rmb_state();
    for(int i=0; i<steps; ++i) {
        rmb_forward();
        s = rmb_state();
    }
    s = rmb_state();
    char msg[128];
    snprintf(msg, sizeof(msg), "[%.2f] incorrect expected position (%d,%d)", rad2grad(angle), s.x, s.y);
    TEST_ASSERT_TRUE_MESSAGE((x+expected_dx == s.x) && (y+expected_dy == s.y), msg);
}


void test_turn_and_move_neighbors(void) {
    // Ángulos y desplazamientos esperados (dx, dy)
    // recordatorio: N: (0,1), S: (0,-1), E: (1,0), W:(-1,0)
    struct { float angle; int dx; int dy; } cases[] = {
        {0.0f, 1, 0}, // Este
        {M_PI/2, 0, 1}, // Norte
        {M_PI, -1, 0}, // Oeste
        {3*M_PI/2, 0, -1} // Sur
    };
    int n = sizeof(cases)/sizeof(cases[0]);
    for(int i=0; i<n; ++i) {
        rmb_reset_awake();
        test_turn_and_move(cases[i].angle, cases[i].dx, cases[i].dy, 1);
    }
}

void test_turn_and_move_diags(void) {
    // Ángulos y desplazamientos esperados (dx, dy)
    // recordatorio: N: (0,1), S: (0,-1), E: (1,0), W:(-1,0)
    struct { float angle; int dx; int dy; } cases[] = {
        {M_PI/4, 1, 1}, // Noreste (45)
        {3*M_PI/4, -1, 1}, // Noroeste (135)
        {5*M_PI/4, -1, -1}, // Suroeste (225)
        {7*M_PI/4, 1, -1} // Sureste (315)
    };
    int n = sizeof(cases)/sizeof(cases[0]);
    for(int i=0; i<n; ++i) {
        // Diagonales requieren 2 pasos
        rmb_reset_awake();
        test_turn_and_move(cases[i].angle, cases[i].dx, cases[i].dy,1);
    }
}

void test_turn_and_move_free_angles (void) {
       // Ángulos y desplazamientos esperados (dx, dy)
    // recordatorio: N: (0,1), S: (0,-1), E: (1,0), W:(-1,0)
    struct { float angle; int dx; int dy; } cases[] = {
        {M_PI/6, 1, 1}, // ~30° (redondeo)
        {M_PI/3, 1, 1}, // ~60°
        {2*M_PI/3, 0, 1}, // ~120°
        {5*M_PI/6, -1, 1} // ~150°
    };
    int n = sizeof(cases)/sizeof(cases[0]);
    for(int i=0; i<n; ++i) {
        rmb_reset_awake();
        test_turn_and_move(cases[i].angle, cases[i].dx, cases[i].dy, 1);
    }
}

// test de descuento correcto de la bateria con distintas acciones

// Auxiliar para agotar la batería del robot
void drain_battery(void) {
    while (1) {
        sensor_t s = rmb_state();
        if (s.battery <= 0.0f) break;
        rmb_turn(M_PI/2);
    }
}

void test_battery_decreases_on_straight_move(void) {
    int x, y;
    setup_robot_at_base("maps/north.pgm", 8, 3, 1, &x, &y); // centro del mapa
    sensor_t s0 = rmb_state();
    rmb_forward();
    sensor_t s1 = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s1.battery < s0.battery, "Battery should decrease after move");
    // test cummulative decrease
    float step_cost = s1.battery - s0.battery;
    int steps = 3;
    for(int i = 0; i < steps; i++) rmb_forward();
    sensor_t s2 = rmb_state();
    float expected_battery = s1.battery + step_cost * steps;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, expected_battery, s2.battery, "Battery decrease after multiple moves incorrect");
}

void test_battery_decreases_on_diagonal_move(void) {
    int x, y;
    setup_robot_at_base("maps/north.pgm", 8, 3, 1, &x, &y); // centro del mapa
 
    // get strainght movement cost
    sensor_t s0 = rmb_state();
    rmb_forward();
    sensor_t s1 = rmb_state();
    // get diagonal movement cost
    rmb_turn(M_PI/4); // turn 45 degrees
    rmb_forward(); // diagonal move
    sensor_t s2 = rmb_state();
    // diagonal consumption should be higher
    float scale = (s1.battery - s0.battery) / (s2.battery - s1.battery);
    TEST_ASSERT_TRUE_MESSAGE(scale < 1, "Battery should decrease more on diagonal move than on straight move");
}

void test_battery_decreases_on_turn(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    sensor_t s0 = rmb_state();
    rmb_turn(M_PI/2);
    sensor_t s1 = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s1.battery < s0.battery, "Battery should decrease after turn");
}

void test_battery_decreases_on_bump(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (3,2) -> (3,1) wall
    rmb_forward(); // First move
    sensor_t s0 = rmb_state();
    rmb_forward(); // Second move should hit the wall
    sensor_t s1 = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s1.battery < s0.battery, "Battery should decrease after bump");
}

void test_battery_decreases_on_clean(void) {
    system("./mapgen maps/map8x8d.pgm 8 3 2 3 1"); // base at (2,3), dirt at (1,3)
    load_map("maps/map8x8d.pgm");
    int x = -1, y = -1;
    rmb_awake(&x, &y);
    rmb_forward(); // Move to dirty cell (3,1)
    sensor_t s0 = rmb_state();
    rmb_clean();
    sensor_t s1 = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s1.battery < s0.battery, "Battery should decrease after cleaning");
}

void test_battery_maintains_cleaning_clean_cell(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 3, 2, &x, &y); // base en (2,3) clean cell
    rmb_forward(); // Move to (3,1) clean cell
    sensor_t s0 = rmb_state();
    rmb_clean();
    sensor_t s1 = rmb_state();
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(EPSILON, s0.battery, s1.battery, "Battery should not decrease when cleaning clean cell");
}

void test_battery_does_not_go_below_zero(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    // Drain battery completely
    drain_battery();
    sensor_t s_final = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s_final.battery >= 0.0f, "Battery should not go below zero");
}

void test_battery_does_not_exceed_max_after_load(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    // Drain battery partially (0.5 units)
    for(int i = 0; i < 5; i++) rmb_turn(M_PI/2);
    // Load at base (10 units)
    int load_result = rmb_load();
    sensor_t s2 = rmb_state();
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, load_result, "Load should succeed at base");
    TEST_ASSERT_TRUE_MESSAGE(s2.battery <= 1000.0f, "Battery should not exceed maximum after load");
}

void test_battery_cannot_move_when_empty(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    // Drain battery completely
    drain_battery();
    sensor_t s_before = rmb_state();
    rmb_forward(); // Try to move
    sensor_t s_after = rmb_state();
    TEST_ASSERT_TRUE_MESSAGE(s_before.x == s_after.x && s_before.y == s_after.y, "Robot should not move when battery is empty");
}

void test_battery_cannot_clean_when_empty(void) {
    system("./mapgen maps/map8x8d.pgm 8 3 2 3 1"); // base at (2,3), dirt at (1,3)
    load_map("maps/map8x8d.pgm");
    int x = -1, y = -1;
    rmb_reset_awake();
    rmb_awake(&x, &y);
    rmb_forward(); // Move to dirty cell (3,1)
    // Drain battery completely
    drain_battery();
    sensor_t s_before = rmb_state();
    rmb_clean(); // Try to clean
    sensor_t s_after = rmb_state();
    TEST_ASSERT_EQUAL_INT_MESSAGE(s_before.infrared, s_after.infrared, "Robot should not clean when battery is empty");

}

void test_battery_cannot_turn_when_empty(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    // Drain battery completely
    drain_battery();
    sensor_t s_before = rmb_state();
    rmb_turn(M_PI/2); // Try to turn
    sensor_t s_after = rmb_state();
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(EPSILON, s_before.heading, s_after.heading, "Robot should not turn when battery is empty");
}

void test_baterry_can_load_when_empty(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    // Drain battery completely
    drain_battery();
    sensor_t s_before = rmb_state();
    int load_result = rmb_load(); // Try to load
    sensor_t s_after = rmb_state();
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, load_result, "Load should succeed at base even when battery is empty");
    TEST_ASSERT_TRUE_MESSAGE(s_after.battery > s_before.battery, "Battery should increase after loading at base");
}

void test_battery_full_after_load_at_base(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    // Drain battery partially (0.5 units)
    for(int i = 0; i < 5; i++) rmb_turn(M_PI/2);
    // Load at base (10 units)
    int load_result = rmb_load();
    sensor_t s2 = rmb_state();
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, load_result, "Load should succeed at base");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 1000.0f, s2.battery, "Battery should be full after load at base");
}

void test_battery_unchanged_on_invalid_action(void) {
    int x, y;
    setup_robot_at_base("maps/map8x8.pgm", 8, 4, 4, &x, &y); // centro del mapa
    sensor_t s0 = rmb_state();
    // Try invalid actions

    sensor_t s1 = rmb_state();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(s0.battery, s1.battery, "Battery should only decrease for valid actions");
}


//-----------------------------------
// MAIN
//-----------------------------------

int main(void) {
    UNITY_BEGIN();

    // Maps and initial state
    RUN_TEST(test_load_map_base_in_wall_should_fail);
    RUN_TEST(test_load_map_no_base_should_fail);
    
    RUN_TEST(test_awake_on_empty_map);
    RUN_TEST(test_awake_without_coordinates_should_fail);
    RUN_TEST(test_awake_twice_should_fail);
    RUN_TEST(test_map_8x8);
    
    RUN_TEST(test_robot_initial_state);
    RUN_TEST(test_robot_at_north_wall);
    RUN_TEST(test_robot_at_south_wall);
    RUN_TEST(test_robot_at_east_wall);
    RUN_TEST(test_robot_at_west_wall);
    
    // Moves
    RUN_TEST(test_try_walk_on_empty_cell);
    RUN_TEST(test_robot_moves_north);
    RUN_TEST(test_robot_moves_south);
    RUN_TEST(test_robot_moves_east);
    RUN_TEST(test_robot_moves_west);
    RUN_TEST(test_try_walk_against_wall);

    // Infrared and cleaning
    RUN_TEST(test_infrared_on_base);
    RUN_TEST(test_infrared_on_clean_cell);
    RUN_TEST(test_infrared_on_dirty_cell);
    RUN_TEST(test_try_clean_on_clean_cell);
    RUN_TEST(test_try_clean_on_dirty_cell);
    RUN_TEST(test_try_clean_on_base);

    // At base
    RUN_TEST(test_robot_at_base);
    RUN_TEST(test_load_out_of_base_should_fail);
    RUN_TEST(test_load_at_base);

    // Turn and move tests
    RUN_TEST(test_turn_and_move_neighbors);
    RUN_TEST(test_turn_and_move_diags);
    RUN_TEST(test_turn_and_move_free_angles);

    // Battery tests
    RUN_TEST(test_battery_decreases_on_straight_move);
    RUN_TEST(test_battery_decreases_on_diagonal_move);
    RUN_TEST(test_battery_decreases_on_turn);
    RUN_TEST(test_battery_decreases_on_bump);
    RUN_TEST(test_battery_decreases_on_clean);
    RUN_TEST(test_battery_maintains_cleaning_clean_cell);
    RUN_TEST(test_battery_does_not_go_below_zero);
    RUN_TEST(test_battery_does_not_exceed_max_after_load);
    RUN_TEST(test_battery_cannot_move_when_empty);
    RUN_TEST(test_battery_cannot_clean_when_empty);
    RUN_TEST(test_battery_cannot_turn_when_empty);
    RUN_TEST(test_baterry_can_load_when_empty);
    RUN_TEST(test_battery_full_after_load_at_base);
    return UNITY_END();
}

