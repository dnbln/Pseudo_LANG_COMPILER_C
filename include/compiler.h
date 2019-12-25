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
#include "cache.h"

typedef struct {
	char strings[8192][4096];
	char strName[8192][32];
	size_t string_count;
} STRINGS_LIST;

typedef struct {
	char label_name[64];
} LABEL;

void Compile(COMPILER_INTERNAL*, int*);

void Compile_Statement(COMPILER_INTERNAL*, Statement, int*);

TYPE compiler_value_instructions(Token*, int, ASMOP*, int*, int*);

char* compiler_varname(const char *identifier_name);

TYPE callOperator(CACHE_PTR a, unsigned long long int op, int op_class, TYPE b, ASMOP* memory, int* ptr, int* success);

char* compiler_loadString(const char *stringVal);

int compiler_lookUpString(const char *stringVal);
char* compiler_stringName(const int index);
char* compiler_addString(const char *stringVal);

void compiler_init();

void compiler_writeDataAndVariables(FILE* f);

CACHE_PTR save_result(TYPE*, ASMOP*, int*, int*);

#define CACHE_SIZE 16384

void cache_init();

void cache_clear();

void cache_delete();

int compiler_getLine();

LABEL make_label();

void cond_generate_assembly(COMPILER_INTERNAL *, Conditional_Statement*, ASMOP*, int*, int*);

#endif /* INCLUDE_COMPILER_H_ */
