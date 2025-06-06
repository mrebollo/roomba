#ifndef SIMULA_MAP_H
#define SIMULA_MAP_H
#include "simula_types.h"

int generate_map(int nrow, int ncol, int num_dirty, float nobs);
int load_map(char *filename);
void print_map();
void save_map();
float put_base_at(int x, int y);

#endif // SIMULA_MAP_H
