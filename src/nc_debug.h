#ifndef NC_DEBUG_H
#define NC_DEBUG_H

#include "nc_chunk.h"

void disassemble_chunk(Chunk *chunk, char *name);

size_t disassemble_instruction(Chunk *chunk, size_t offset);

#endif // NC_DEBUG_H