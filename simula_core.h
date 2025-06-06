#ifndef SIMULA_CORE_H
#define SIMULA_CORE_H
#include "simula_types.h"

void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time);
void run();
void visualize();

#endif // SIMULA_CORE_H
