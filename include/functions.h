/*
 * functions.h
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_FUNCTIONS_H_
#define INCLUDE_FUNCTIONS_H_

#include "asmop.h"
#include "tokens.h"

typedef struct {
	char name[64];
	ASMOP* (*genertate_assembly)(Token *t);
} Function;

void GetFunction(const char* name, Function *f, int* found);

#endif /* INCLUDE_FUNCTIONS_H_ */
