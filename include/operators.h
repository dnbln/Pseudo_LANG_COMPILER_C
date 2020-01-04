#ifndef OPERATORS_H
#define OPERATORS_H

#include "cache.h"
#include "compiler.h"


typedef TYPE OPERATOR_CALL(CACHE_PTR a, TYPE b, ASMOP* memory, size_t* ptr, int* success);

typedef struct {
	OPERATOR_CALL* call;
} OPERATOR;

void operators_init();

OPERATOR *getOperator(const char *signature);

void freeStringPool();// deallocates the memory that was allocated in the string pool

#endif