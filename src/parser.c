/*
 * parser.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#include <string.h>
#include <stdlib.h>

#include "../include/parser.h"
#include "../include/utils.h"
#include "../include/functions.h"
#include "../include/errors.h"
#include "../include/compiler.h"

OPTS *(*getOpts)();

void Parser_init(OPTS *(*getOptsFunc)())
{
	getOpts = getOptsFunc;
}

void Parse(FILE *f, COMPILER_INTERNAL *internal_state, int *success)
{
	internal_state->TOKEN_PTR = internal_state->STATEMENT_POINTER =
		internal_state->STATEMENT_DATA_PTR = 0;
	size_t characters_read = fread(internal_state->text_read, sizeof(char),
								   TEXT_READ_SIZE, f);
	internal_state->CHARACTERS_READ = characters_read;
	internal_state->FILE_PTR = internal_state->text_read;
	internal_state->DATA_PTR = internal_state->data_for_tokens;
	Token t;
	int line = 1;
	do
	{
		GetNextToken(internal_state, &t, &line, success);
		internal_state->tokens[internal_state->TOKEN_PTR++] = t;
	} while (t.type != FILE_END_TOKEN && (*success));
	if (*success == 0)
		return;
	internal_state->TOKEN_PTR = 0;
	Statement statement;
	line = 1;
	do
	{
		GetNextStatement(internal_state, &statement, &line, success);
		internal_state->statements[internal_state->STATEMENT_POINTER++] =
			statement;
	} while (statement.type != NO_STATEMENT && (*success));
}

void GetNextToken(COMPILER_INTERNAL *state, Token *token, int *line, int *success)
{
	char *ptr = state->FILE_PTR;
	while (is_whitespace(*ptr))
		ptr++;
	if (*ptr == '\0')
	{
		*token = Make_nodata_Token(FILE_END_TOKEN);
		state->FILE_PTR = ptr;
		return;
	}
	if (starts_with(ptr, "("))
	{
		*token = Make_nodata_Token(P_OPEN_TOKEN);
		state->FILE_PTR = ++ptr;
		return;
	}
	if (starts_with(ptr, ")"))
	{
		*token = Make_nodata_Token(P_CLOSED_TOKEN);
		state->FILE_PTR = ++ptr;
		return;
	}
	if (starts_with(ptr, "\n"))
	{
		*token = Make_data_Token(NEW_LINE_TOKEN, (void *)((unsigned long long)*line));
		state->FILE_PTR = ++ptr;
		(*line)++;
		return;
	}
	if (starts_with(ptr, "begin"))
	{
		*token = Make_nodata_Token(BEGIN_TOKEN);
		state->FILE_PTR = ptr + 5; //ptr + strlen("begin")
		return;
	}
	if (starts_with(ptr, "end"))
	{
		*token = Make_nodata_Token(END_TOKEN);
		state->FILE_PTR = ptr + 3; //ptr + strlen("end")
		return;
	}
	if (starts_with(ptr, "if"))
	{
		*token = Make_nodata_Token(IF_TOKEN);
		state->FILE_PTR = ptr + 2;
		return;
	}
	if (starts_with(ptr, "then"))
	{
		*token = Make_nodata_Token(THEN_TOKEN);
		state->FILE_PTR = ptr + 4;
		return;
	}
	if (starts_with(ptr, "else"))
	{
		*token = Make_nodata_Token(ELSE_TOKEN);
		state->FILE_PTR = ptr + 4;
		return;
	}
	if (starts_with(ptr, "of type"))
	{
		*token = Make_nodata_Token(OF_TYPE_TOKEN);
		state->FILE_PTR = ptr + 7;
		return;
	}
	if (starts_with(ptr, "number"))
	{
		*token = Make_data_Token(TYPE_TOKEN, (void*) NUMBER_TYPE);
		state->FILE_PTR = ptr + 6;
		return;
	}
	if (starts_with(ptr, "string"))
	{
		*token = Make_data_Token(TYPE_TOKEN, (void*) STRING_TYPE);
		state->FILE_PTR = ptr + 6;
		return;
	}
	if (starts_with(ptr, "+"))
	{
		*token = Make_data_Token(ARITH_OP_TOKEN, (void *)ADD);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "-"))
	{
		*token = Make_data_Token(ARITH_OP_TOKEN, (void *)SUBTRACT);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "*"))
	{
		*token = Make_data_Token(ARITH_OP_TOKEN, (void *)MULTIPLY);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "/"))
	{
		*token = Make_data_Token(ARITH_OP_TOKEN, (void *)DIVIDE);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "=="))
	{
		*token = Make_data_Token(EQUALITY_OP_TOKEN, (void *)EQ);
		state->FILE_PTR = ptr + 2;
		return;
	}
	if (starts_with(ptr, "!="))
	{
		*token = Make_data_Token(EQUALITY_OP_TOKEN, (void *)NEQ);
		state->FILE_PTR = ptr + 2;
		return;
	}
	if (starts_with(ptr, "<="))
	{
		*token = Make_data_Token(EQUALITY_OP_TOKEN, (void *)LEQ);
		state->FILE_PTR = ptr + 2;
		return;
	}
	if (starts_with(ptr, ">="))
	{
		*token = Make_data_Token(EQUALITY_OP_TOKEN, (void *)GEQ);
		state->FILE_PTR = ptr + 2;
		return;
	}
	if (starts_with(ptr, "<-"))
	{
		*token = Make_nodata_Token(ASSIGNMENT_TOKEN);
		state->FILE_PTR = ptr + 2;
		return;
	}
	if (starts_with(ptr, "<"))
	{
		*token = Make_data_Token(EQUALITY_OP_TOKEN, (void *)L);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, ">"))
	{
		*token = Make_data_Token(EQUALITY_OP_TOKEN, (void *)G);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "="))
	{
		*token = Make_nodata_Token(ASSIGNMENT_TOKEN);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, ","))
	{
		*token = Make_nodata_Token(COMMA_TOKEN);
		state->FILE_PTR = ptr + 1;
		return;
	}
	if (starts_with(ptr, "\""))
	{
		char *startptr = ptr + 1;
		unsigned int escaped = 0, closed = 0;
		for (ptr++; *ptr != '\0'; ptr++)
		{
			if (*ptr == '\\')
				escaped = !escaped;
			else if (*ptr == '"')
			{
				if (!escaped)
				{
					closed = 1;
					break;
				}
			}
			else
			{
				if (*ptr == '\n')
					break;
				escaped = 0;
			}
		}
		if (!closed)
		{
			ERROR err;
			err.code = STRING_NOT_ENDED;
			err.extra = NULL;
			err.line = *line;
			err.clear = 0;
			load_error(err);
			(*success) = 0;
			return;
		}
		state->FILE_PTR = ptr + 1;
		int size = ptr - startptr;
		strncpy(state->DATA_PTR, startptr, size);
		*token = Make_data_Token(STRING_TOKEN, state->DATA_PTR);
		state->DATA_PTR += size + 1;
	}
	if (starts_class(ptr, DIGIT_CLASS))
	{
		long long value = 0;
		do
		{
			value = value * 10 + (*ptr - '0');
			ptr++;
		} while (starts_class(ptr, DIGIT_CLASS));
		*(long long *)((void *)state->DATA_PTR) = value;
		*token = Make_data_Token(NUMBER_TOKEN, state->DATA_PTR);
		state->DATA_PTR += sizeof(long long);
		state->FILE_PTR = ptr;
		return;
	}
	if (starts_class(ptr, IDENTIFIER_FIRST_CHARS))
	{
		int i = 0;
		do
		{
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

void GetNextStatement(COMPILER_INTERNAL *state, Statement *statement, int *line, int *success)
{
	while (state->tokens[state->TOKEN_PTR].type == NEW_LINE_TOKEN)
		state->TOKEN_PTR++, (*line)++;
	if (state->tokens[state->TOKEN_PTR].type == FILE_END_TOKEN)
	{
		statement->type = NO_STATEMENT;
		return;
	}
	if (state->tokens[state->TOKEN_PTR].type == BEGIN_TOKEN)
	{
		// composite statement
		statement->type = COMPOSITE_STATEMENT;
		unsigned int COMPOSITE_STATEMENT_ID =
			state->COMPOSITE_STATEMENT_POINTER++;
		statement->data =
			(state->composite_statements + COMPOSITE_STATEMENT_ID);
		Statement s;
		(state->TOKEN_PTR)++;
		unsigned int count = 0;
		while (state->tokens[state->TOKEN_PTR].type != END_TOKEN)
		{
			GetNextStatement(state, &s, line, success);
			state->inside_composite_statements[state->INSIDE_COMPOSITE_STATEMENT_POINTER] =
				s;
			state->composite_statements[COMPOSITE_STATEMENT_ID].statements[count] =
				&(state->inside_composite_statements[state->INSIDE_COMPOSITE_STATEMENT_POINTER]);
			(state->INSIDE_COMPOSITE_STATEMENT_POINTER)++;
			count++;
			while (state->tokens[state->TOKEN_PTR].type == NEW_LINE_TOKEN)
				state->TOKEN_PTR++, (*line)++;
		}
		state->composite_statements[COMPOSITE_STATEMENT_ID].inside_statements_count =
			count;
		state->TOKEN_PTR++;
		return;
	}
	statement->line = *line;
	if (state->tokens[state->TOKEN_PTR].type == IDENTIFIER_TOKEN)
	{
		if (state->tokens[state->TOKEN_PTR + 1].type == ASSIGNMENT_TOKEN)
		{
			// assignment
			statement->type = ASSIGNMENT_STATEMENT;
			statement->data = (void *)(long long)state->TOKEN_PTR;
			while (state->tokens[state->TOKEN_PTR].type != NEW_LINE_TOKEN)
				state->TOKEN_PTR++;
			return;
		}
		else
		{
			//function call
			Function f;
			int found;
			GetFunction((const char *)state->tokens[state->TOKEN_PTR].data, &f,
						&found);
			if (found == 0)
			{
				ERROR err;
				err.code = FUNCTION_NOT_FOUND;
				err.extra = (char *)state->tokens[state->TOKEN_PTR].data;
				err.clear = 0;
				while (state->tokens[state->TOKEN_PTR].type != NEW_LINE_TOKEN)
					state->TOKEN_PTR++;
				err.line = (unsigned long long)state->tokens[state->TOKEN_PTR].data;
				load_error(err);
				*success = 0;
				return;
			}
			else
			{
				statement->type = FUNCTION_CALL_STATEMENT;
				long long *allocated_mem = (long long *)(state->statement_data + state->STATEMENT_DATA_PTR);
				allocated_mem[0] = (long long)f.generate_assembly;
				allocated_mem[1] = (long long)(state->tokens + ++(state->TOKEN_PTR));
				statement->data = allocated_mem;
				state->STATEMENT_DATA_PTR += 2 * sizeof(long long);
				while (state->tokens[state->TOKEN_PTR].type != NEW_LINE_TOKEN)
					state->TOKEN_PTR++;
				return;
			}
		}
	}
	if (state->tokens[state->TOKEN_PTR].type == IF_TOKEN)
	{
		int ptr = state->TOKEN_PTR, start_line = *line;
		Condition cond;
		cond.ptr = state->tokens + ptr + 1;
		while (state->tokens[ptr].type != FILE_END_TOKEN && state->tokens[ptr].type != THEN_TOKEN)
		{
			ptr++;
			if (state->tokens[ptr].type == NEW_LINE_TOKEN)
			{
				(*line)++;
			}
		}
		if (state->tokens[ptr].type == FILE_END_TOKEN)
		{
			ERROR err;
			err.line = start_line;
			err.code = EXPECTED_TOKEN_NOT_FOUND;
			err.extra = (char *)malloc(5 * sizeof(char));
			strcpy(err.extra, "then");
			err.clear = 1;
			load_error(err);
			*success = 0;
			return;
		}
		cond.tn = state->tokens + ptr - cond.ptr;
		state->TOKEN_PTR = ++ptr;
		statement->type = CONDITIONAL_STATEMENT;
		statement->line = start_line;
		statement->data = state->conditional_statements_data + state->conditional_statements_ptr;
		state->conditional_statements_ptr++;
		Statement onTrue, onFalse;
		unsigned has_false = 0;
		GetNextStatement(state, &onTrue, line, success);
		while (state->tokens[state->TOKEN_PTR].type == NEW_LINE_TOKEN)
		{
			state->TOKEN_PTR++;
			(*line)++;
		}
		if (state->tokens[state->TOKEN_PTR].type == ELSE_TOKEN)
		{
			state->TOKEN_PTR++;
			GetNextStatement(state, &onFalse, line, success);
			has_false = 1;
		}
		((Conditional_Statement *)statement->data)->condition = cond;
		((Conditional_Statement *)statement->data)->onTrue = onTrue;
		if (has_false)
			((Conditional_Statement *)statement->data)->onFalse = onFalse;
		((Conditional_Statement *)statement->data)->has_false = has_false;
		return;
	}
	printf("Unknown token at beginning of statement %d '%s'\n",
		   state->tokens[state->TOKEN_PTR].type,
		   (char *)state->tokens[state->TOKEN_PTR].data);
	state->TOKEN_PTR++;
}

void Write(FILE *f, COMPILER_INTERNAL *internal_state)
{
	fprintf(f, ".globl _start\n_start:\n\tcallq _pseudo_lib_init@PLT\n\tmovq $%ld, STRINGS_POOL_PTR\n", getOpts()->stringPoolSize);
	for (int i = 0; i < internal_state->asmop_memptr; i++)
	{
		ASMOP *op = internal_state->asmop_mem + i;
		if (op->operation[0] == '\0')
			continue;
		fprintf(f, "\t%s", op->operation);
		if (op->operand1[0] == '\0')
		{
			fprintf(f, "\n");
			continue;
		}
		fprintf(f, " %s", op->operand1);
		if (op->operand2[0] == '\0')
		{
			fprintf(f, "\n");
			continue;
		}
		fprintf(f, ", %s", op->operand2);
		if (op->operand3[0] == '\0')
		{
			fprintf(f, "\n");
			continue;
		}
		fprintf(f, ", %s", op->operand3);
		fprintf(f, "\n");
	}
	fprintf(f, "\tmovq $0, %%rax\n\tcallq _pseudo_lib_exit@PLT\n");
	compiler_writeDataAndVariables(f);
}
