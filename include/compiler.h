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

typedef TYPE OPERATOR_CALL(Token a, Token b, ASMOP* memory, int* ptr, int* success);

typedef struct {
	int a1_type;//arg 1 type
	int a2_type;//arg 2 type (if any)
	int op; // operation id
	OPERATOR_CALL* call;
} OPERATOR;

typedef struct {
	char strings[8192][4096];
	char strName[8192][32];
	size_t string_count;
} STRINGS_LIST;

void Compile(COMPILER_INTERNAL*, int*);

void Compile_Statement(COMPILER_INTERNAL*, Statement, int*);

TYPE compiler_value_instructions(Token*, int, ASMOP*, int*, int*);

char* compiler_varname(const char *identifier_name);

TYPE callOperator(Token a, int op, Token b, ASMOP* memory, int* ptr, int* success);

char* compiler_loadString(const char *stringVal);

int compiler_lookUpString(const char *stringVal);
char* compiler_stringName(const int index);
char* compiler_addString(const char *stringVal);

void compiler_init();

void compiler_writeDataAndVariables(FILE* f);

#endif /* INCLUDE_COMPILER_H_ */
