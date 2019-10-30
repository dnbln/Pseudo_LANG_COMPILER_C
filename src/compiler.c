/*
 * compiler.c
 *
 *  Created on: Oct 25, 2019
 *      Author: dinu
 */

#include "../include/compiler.h"
#include "../include/functions.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>

void Compile(COMPILER_INTERNAL *internal_state, int *success) {
	internal_state->asmop_memptr = 0;
	*success = 1;
	for (int i = 0; *success == 1 && i < internal_state->STATEMENT_POINTER; i++)
		Compile_Statement(internal_state, internal_state->statements[i],
				success);
}

void Compile_Statement(COMPILER_INTERNAL *internal_state, Statement s,
		int *success) {
	const int type = s.type;
	*success = 1;
	if (type == FUNCTION_CALL_STATEMENT) {
		long long *statement_data = s.data;
		ASM_FUNC_GENERATE *generate_assembly =
				(ASM_FUNC_GENERATE*) statement_data[0];
		Token *tokens = (Token*) statement_data[1];
		generate_assembly(tokens, internal_state->asmop_mem,
				&(internal_state->asmop_memptr), success);
	}
}

TYPE compiler_value_instructions(Token *t, int n, ASMOP *memory, int *ptr,
		int *success) {
	TYPE type;
	type.typeid = VOID_TYPE;
	if (n == 0) {
		return type;
	}
	if (n == 1) {
		if (t->type == NUMBER_TOKEN) {
			strcpy(memory[*ptr].operation, "movq");
			char str_[20];
			num_to_str(*((long long*) t->data), str_ + 1);
			str_[0] = '$';
			strcpy(memory[*ptr].operand1, str_);
			strcpy(memory[*ptr].operand2, "%rax");
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;
			type.typeid = NUMBER_TYPE;
			return type;
		}
	}
	return type;
}

char* compiler_varname(const char *identifier_name) {
	return NULL;
}
