/*
 * compiler.h
 *
 *  Created on: Oct 25, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_COMPILER_H_
#define INCLUDE_COMPILER_H_

#include "parser.h"
#include "statements.h"
#include "types.h"

void Compile(COMPILER_INTERNAL*, int*);

void Compile_Statement(COMPILER_INTERNAL*, Statement, int*);

TYPE compiler_value_instructions(Token*, int, ASMOP*, int*, int*);

char* compiler_varname(const char *identifier_name);

#endif /* INCLUDE_COMPILER_H_ */
