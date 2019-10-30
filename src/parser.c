/*
 * parser.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#include "../include/parser.h"
#include "../include/utils.h"
#include "../include/functions.h"
#include "../include/errors.h"

void Parse(FILE *f, COMPILER_INTERNAL *internal_state) {
	internal_state->TOKEN_PTR = internal_state->STATEMENT_POINTER =
			internal_state->STATEMENT_DATA_PTR = 0;
	size_t characters_read = fread(internal_state->text_read, sizeof(char),
	TEXT_READ_SIZE, f);
	internal_state->CHARACTERS_READ = characters_read;
	internal_state->FILE_PTR = internal_state->text_read;
	internal_state->DATA_PTR = internal_state->data_for_tokens;
	Token t;
	do {
		GetNextToken(internal_state, &t);
		internal_state->tokens[internal_state->TOKEN_PTR++] = t;
	} while (t.type != FILE_END_TOKEN);
	internal_state->TOKEN_PTR = 0;
	Statement statement;
	do {
		GetNextStatement(internal_state, &statement);
		internal_state->statements[internal_state->STATEMENT_POINTER++] =
				statement;
	} while (statement.type != NO_STATEMENT);
}

void GetNextToken(COMPILER_INTERNAL *state, Token *token) {
	char *ptr = state->FILE_PTR;
	while (is_whitespace(*ptr))
		ptr++;
	if (*ptr == '\0') {
		*token = Make_nodata_Token(FILE_END_TOKEN);
		state->FILE_PTR = ptr;
		return;
	}
	if (starts_with(ptr, "(")) {
		*token = Make_nodata_Token(P_OPEN_TOKEN);
		state->FILE_PTR = ++ptr;
		return;
	}
	if (starts_with(ptr, ")")) {
		*token = Make_nodata_Token(P_CLOSED_TOKEN);
		state->FILE_PTR = ++ptr;
		return;
	}
	if (starts_with(ptr, "\n")) {
		*token = Make_nodata_Token(NEW_LINE_TOKEN);
		state->FILE_PTR = ++ptr;
		return;
	}
	if (starts_with(ptr, "begin")) {
		*token = Make_nodata_Token(BEGIN_TOKEN);
		state->FILE_PTR = ptr + 5; //ptr + strlen("begin")
		return;
	}
	if (starts_with(ptr, "end")) {
		*token = Make_nodata_Token(END_TOKEN);
		state->FILE_PTR = ptr + 3; //ptr + strlen("end")
		return;
	}
	if (starts_with(ptr, "+")) {
		*token = Make_data_Token(ARITH_OP_TOKEN, (void*) ADD);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "-")) {
		*token = Make_data_Token(ARITH_OP_TOKEN, (void*) SUBTRACT);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "*")) {
		*token = Make_data_Token(ARITH_OP_TOKEN, (void*) MULTIPLY);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "/")) {
		*token = Make_data_Token(ARITH_OP_TOKEN, (void*) DIVIDE);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "<-")) {
		*token = Make_nodata_Token(ASSIGNMENT_TOKEN);
		state->FILE_PTR = ptr + 2;
		return;
	}
	if (starts_with(ptr, "=")) {
		*token = Make_nodata_Token(ASSIGNMENT_TOKEN);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, ",")) {
		*token = Make_nodata_Token(COMMA_TOKEN);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_class(ptr, DIGIT_CLASS)) {
		long long value = 0;
		do {
			value = value * 10 + (*ptr - '0');
			ptr++;
		} while (starts_class(ptr, DIGIT_CLASS));
		*(long long*) ((void*) state->DATA_PTR) = value;
		*token = Make_data_Token(NUMBER_TOKEN, state->DATA_PTR);
		state->DATA_PTR += sizeof(long long);
		state->FILE_PTR = ptr;
		return;
	}
	if (starts_class(ptr, IDENTIFIER_FIRST_CHARS)) {
		int i = 0;
		do {
			*(state->DATA_PTR + i++) = *ptr;
			ptr++;
		} while (starts_class(ptr, IDENTIFIER_OTHER_CHARS));
		*(state->DATA_PTR + i++) = '\0';
		*token = Make_data_Token(IDENTIFIER_TOKEN, state->DATA_PTR);
		state->DATA_PTR = state->DATA_PTR + i;
		state->FILE_PTR = ptr;
		return;
	}
}

void GetNextStatement(COMPILER_INTERNAL *state, Statement *statement) {
	while (state->tokens[state->TOKEN_PTR].type == NEW_LINE_TOKEN)
		state->TOKEN_PTR++;
	if (state->tokens[state->TOKEN_PTR].type == FILE_END_TOKEN) {
		statement->type = NO_STATEMENT;
		return;
	}
	if (state->tokens[state->TOKEN_PTR].type == BEGIN_TOKEN) {
		// composite statement
		statement->type = COMPOSITE_STATEMENT;
		unsigned int COMPOSITE_STATEMENT_ID =
				state->COMPOSITE_STATEMENT_POINTER++;
		statement->data =
				(state->composite_statements + COMPOSITE_STATEMENT_ID);
		Statement s;
		(state->TOKEN_PTR)++;
		unsigned int count = 0;
		while (state->tokens[state->TOKEN_PTR].type != END_TOKEN) {
			GetNextStatement(state, &s);
			state->inside_composite_statements[state->INSIDE_COMPOSITE_STATEMENT_POINTER] =
					s;
			state->composite_statements[COMPOSITE_STATEMENT_ID].statements[count] =
					&(state->inside_composite_statements[state->INSIDE_COMPOSITE_STATEMENT_POINTER]);
			(state->INSIDE_COMPOSITE_STATEMENT_POINTER)++;
			count++;
			while (state->tokens[state->TOKEN_PTR].type == NEW_LINE_TOKEN)
				state->TOKEN_PTR++;
		}
		state->composite_statements[COMPOSITE_STATEMENT_ID].inside_statements_count =
				count;
		state->TOKEN_PTR++;
		return;
	}
	if (state->tokens[state->TOKEN_PTR].type == IDENTIFIER_TOKEN) {
		if (state->tokens[state->TOKEN_PTR + 1].type == ASSIGNMENT_TOKEN) {
			// assignment
			statement->type = ASSIGNMENT_STATEMENT;
			state->TOKEN_PTR += 2;
			statement->data = (void*) (long long) state->TOKEN_PTR;
			while (state->tokens[state->TOKEN_PTR].type != NEW_LINE_TOKEN)
				state->TOKEN_PTR++;
			return;
		} else {
			//function call
			Function f;
			int found;
			GetFunction((const char*) state->tokens[state->TOKEN_PTR].data, &f,
					&found);
			if (found == 0) {
				fprintf(stderr, "Function '%s' not found\n",
						(const char*) state->tokens[state->TOKEN_PTR].data);
				error(FUNCTION_NOT_FOUND);
				return;
			} else {
				statement->type = FUNCTION_CALL_STATEMENT;
				long long *allocated_mem = (long long*) (state->statement_data
						+ state->STATEMENT_DATA_PTR);
				allocated_mem[0] = (long long) f.generate_assembly;
				allocated_mem[1] = (long long) (state->tokens
						+ ++(state->TOKEN_PTR));
				statement->data = allocated_mem;
				state->STATEMENT_DATA_PTR += 2 * sizeof(long long);
				while (state->tokens[state->TOKEN_PTR].type != NEW_LINE_TOKEN)
					state->TOKEN_PTR++;
				return;
			}
		}
	}
	printf("Unknown token at beginning of statement %d '%s'\n",
			state->tokens[state->TOKEN_PTR].type,
			(char*) state->tokens[state->TOKEN_PTR].data);
	state->TOKEN_PTR++;
}

void Write(FILE *f, COMPILER_INTERNAL *internal_state) {
	fprintf(f, ".globl _start\n_start:\n");
	for (int i = 0; i < internal_state->asmop_memptr; i++) {
		ASMOP *op = internal_state->asmop_mem + i;
		fprintf(f, "\t%s", op->operation);
		if (op->operand1[0] != '\0') {
			fprintf(f, " %s", op->operand1);
			if (op->operand2[0] != '\0') {
				fprintf(f, ", %s", op->operand2);
				if (op->operand3[0] != '\0') {
					fprintf(f, ", %s", op->operand3);
				}
			}
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\tmovq $0, %%rax\n\tcallq _pseudo_lib_exit@PLT\n");
}
