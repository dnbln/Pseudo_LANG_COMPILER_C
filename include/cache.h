#ifndef CACHE_H
#define CACHE_H

#include "types.h"

typedef struct {
	TYPE data_type;
	int cache_index;
} CACHE;

typedef CACHE* CACHE_PTR;

#endif