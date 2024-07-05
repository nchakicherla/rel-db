#include "nc_hashmap.h"

#define N_BUCKETS 16

unsigned long
djb2hash(char *data) {

	unsigned long hash = 5381;
	int c;

	while ((c = *data++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash % N_BUCKETS;
}

uint32_t // https://bpa.st/4P7Q
FNV_1a_hash(const char* str) {
    uint32_t h = 0x811c9dc5u;
    for (unsigned char *p = (unsigned char *) str; *p != '\0'; p++) {
        h = (h ^ *p) * 0x1000193u;
    }
    h = (h ^ (h >> 16)) * 0x7feb352du;
    h = (h ^ (h >> 15)) * 0x846ca68bu;
    return h ^ (h >> 16);
}
