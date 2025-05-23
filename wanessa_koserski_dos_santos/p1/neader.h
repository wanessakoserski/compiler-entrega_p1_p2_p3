#ifndef NEADER_H
#define NEADER_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Register Register;

Register* init();
bool verify_file(u_int8_t dig1, u_int8_t dig2, u_int8_t dig3, u_int8_t dig4);
bool set_memory(Register* reg, FILE *file);
void show_memory(Register* reg);
void destroy(Register* reg);
void execute(Register* reg);


#endif
