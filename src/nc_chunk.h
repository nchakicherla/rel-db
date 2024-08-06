#ifndef NC_CHUNK_H
#define NC_CHUNK_H

#include "nc_common.h"

typedef enum {
	OP_RETURN,
} OpCode;

typedef struct {
	size_t count;
	size_t capacity;
	uint8_t *code;
} Chunk;

void init_chunk(Chunk *chunk);

void free_chunk(Chunk *chunk);

void write_chunk(Chunk *chunk, uint8_t byte);

#endif // NC_CHUNK_H