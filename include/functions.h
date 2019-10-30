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
#include "../include/types.h"

typedef ASMOP* ASM_FUNC_GENERATE(Token*, ASMOP*, int*, int*);

typedef struct {
	char name[64];
	ASM_FUNC_GENERATE *generate_assembly;
} Function;

#define STREAM_REGISTER "%r15w"

void GetFunction(const char *name, Function *f, int *found);

void Init_Functions(char* (*varname)(const char *identifier_name),
		TYPE (*value_instructions)(Token*, int, ASMOP*, int*, int*));

#endif /* INCLUDE_FUNCTIONS_H_ */
