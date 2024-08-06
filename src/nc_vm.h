#ifndef NC_VM_H
#define NC_VM_H

#include "nc_chunk.h"

typedef struct {
	Chunk *chunk;
	uint8_t *ip;
} VM;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} Interpret_Result;

void init_vm();
void free_vm();
Interpret_Result interpret(Chunk *chunk);

#endif // NC_VM_H