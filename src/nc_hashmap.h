#ifndef NVC_HASHMAP_H
#define NVC_HASHMAP_H

#include <inttypes.h>

unsigned long
djb2hash(char *str);

uint32_t // https://bpa.st/4P7Q
FNV_1a_hash(const char* str);

#endif // NVC_HASHMAP_H
