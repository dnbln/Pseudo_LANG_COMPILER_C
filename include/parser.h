/*
 * parser.h
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_PARSER_H_
#define INCLUDE_PARSER_H_
#include <stdio.h>

#include "../include/tokens.h"
#include "../include/statements.h"
#include "../include/asmop.h"

#define TEXT_READ_SIZE 65536
#define TOKENS_MAX 32768
#define DATA_FOR_TOKENS 65536
#define STATEMENTS 65536
#define COMPOSITE_STATEMENTS 16384
#define ASMOP_MEMORY 65536
#define CONDITIONAL_STATEMENTS 65536

typedef struct {
	size_t TOKEN_PTR;
	size_t STATEMENT_POINTER;
	size_t COMPOSITE_STATEMENT_POINTER;
	size_t INSIDE_COMPOSITE_STATEMENT_POINTER;
	char *FILE_PTR;
	size_t CHARACTERS_READ;
	char *DATA_PTR;
	size_t asmop_memptr;
	size_t STATEMENT_DATA_PTR;

	char text_read[TEXT_READ_SIZE];
	Token tokens[TOKENS_MAX];
	char data_for_tokens[DATA_FOR_TOKENS];
	Statement statements[STATEMENTS];
	Composite_Statement composite_statements[COMPOSITE_STATEMENTS];
	Statement inside_composite_statements[STATEMENTS];
	char statement_data[STATEMENTS];
	ASMOP asmop_mem[ASMOP_MEMORY];

	Conditional_Statement conditional_statements_data[CONDITIONAL_STATEMENTS];
	size_t conditional_statements_ptr;
} COMPILER_INTERNAL;

void Parse(FILE*, COMPILER_INTERNAL*, int*);

void GetNextToken(COMPILER_INTERNAL*, Token*, int*, int*);

void GetNextStatement(COMPILER_INTERNAL*, Statement*, int*, int*);

// Write out the generated assembly
void Write(FILE*, COMPILER_INTERNAL*);

#endif /* INCLUDE_PARSER_H_ */
