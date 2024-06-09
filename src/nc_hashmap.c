#include "nc_hashmap.h"

#define N_BUCKETS 16

unsigned long
djb2hash (char *data) {

	unsigned long hash = 5381;
	int c;

	while ((c = *data++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash % N_BUCKETS;
}

