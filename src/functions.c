/*
 * functions.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */
#include "../include/functions.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/utils.h"
#include "../include/vars.h"
#include "../include/errors.h"
#include "../include/tokens.h"

#define PREDEFINED_FUNCTIONS 3

Function declared_functions[PREDEFINED_FUNCTIONS];

int (*getLine_func)();

void GetFunction(const char *name, Function *f, int *found)
{
	for (int i = 0; i < PREDEFINED_FUNCTIONS; i++)
	{
		if (strcmp(name, declared_functions[i].name) == 0)
		{
			*f = declared_functions[i];
			*found = 1;
			return;
		}
	}
	*found = 0;
}

int end_of_line(Token *t); // returns the index in the array t of the first new line token

ASMOP *print_generate_assembly(Token *t, ASMOP *memory, size_t *ptr, int *success);

ASMOP *declare_generate_assembly(Token *t, ASMOP *memory, size_t *ptr, int *success);

TYPE(*value_instructions)
(Token *, int, ASMOP *, size_t *, int *);
void (*clean_str_pool)(ASMOP *, size_t *, int *);

void Init_Functions(int (*getLine)(), TYPE (*v_instructions)(Token *, int, ASMOP *, size_t *, int *),
					void (*clean_str_pool_func)(ASMOP *, size_t *, int *))
{
	strcpy(declared_functions[0].name, "print");
	declared_functions[0].generate_assembly = &print_generate_assembly;

	strcpy(declared_functions[1].name, "write");
	declared_functions[1].generate_assembly = &print_generate_assembly;

	strcpy(declared_functions[2].name, "declare");
	declared_functions[2].generate_assembly = &declare_generate_assembly;

	getLine_func = getLine;
	value_instructions = v_instructions;
	clean_str_pool = clean_str_pool_func;
}

void print_num_handler(ASMOP *memory, size_t *ptr, int *success)
{
	strcpy(memory[*ptr].operation, "callq");
	strcpy(memory[*ptr].operand1, "_print_number@PLT");
	memory[*ptr].operand2[0] = '\0';
	(*ptr)++;
}

void print_str_handler(ASMOP *memory, size_t *ptr, int *success)
{
	strcpy(memory[*ptr].operation, "callq");
	strcpy(memory[*ptr].operand1, "_print_string@PLT");
	memory[*ptr].operand2[0] = '\0';
	(*ptr)++;
	clean_str_pool(memory, ptr, success);
}

ASMOP *print_generate_assembly(Token *t, ASMOP *memory, size_t *ptr, int *success)
{
	int eol = end_of_line(t);
	if ((eol > 2 && t[eol - 2].type == FROM_TO_TOKEN) || (eol > 3 && t[eol - 3].type == FROM_TO_TOKEN))
	{
		strcpy(memory[*ptr].operation, "movq");
		if (t[eol - 2].type == FROM_TO_TOKEN && t[eol - 1].type == CONSOLE_TOKEN)
			strcpy(memory[*ptr].operand1, "$1");
		else if (t[eol - 3].type == FROM_TO_TOKEN && t[eol - 2].type == FILE_TOKEN && t[eol - 1].type == IDENTIFIER_TOKEN)
			strcpy(memory[*ptr].operand1, varname(t[eol - 1].data));
		strcpy(memory[*ptr].operand2, STREAM_REGISTER);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;
	}
	else
	{
		strcpy(memory[*ptr].operation, "movq");
		strcpy(memory[*ptr].operand1, "$1");
		strcpy(memory[*ptr].operand2, STREAM_REGISTER);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;
	}
	int last_p = 0;
	for (int i = 0; i < eol; i++)
	{
		if (t[i].type == COMMA_TOKEN)
		{
			int len = i - last_p;
			TYPE type = value_instructions(t + last_p, len, memory, ptr,
										   success);
			last_p = i + 1;
			if (type.typeid == NUMBER_TYPE)
			{
				print_num_handler(memory, ptr, success);
			}
			else if (type.typeid == STRING_TYPE)
			{
				print_str_handler(memory, ptr, success);
			}
		}
	}

	int len = eol - last_p;
	TYPE type = value_instructions(t + last_p, len, memory, ptr, success);
	if (type.typeid == NUMBER_TYPE)
	{
		print_num_handler(memory, ptr, success);
	}
	else if (type.typeid == STRING_TYPE)
	{
		print_str_handler(memory, ptr, success);
	}

	return memory;
}

ASMOP *declare_generate_assembly(Token *t, ASMOP *memory, size_t *ptr, int *success)
{
	int eol = end_of_line(t);
	if (t[eol - 2].type != OF_TYPE_TOKEN)
	{
		ERROR err;
		err.code = EXPECTED_TOKEN_NOT_FOUND;
		err.line = getLine_func();
		err.extra = (char *)malloc(8 * sizeof(char));
		err.clear = 1;
		strcpy(err.extra, "OF TYPE");
		load_error(err);
		*success = 0;
		return memory;
	}
	unsigned long long var_type = (unsigned long long)t[eol - 1].data;
	int last_p = 0;
	unsigned assignment = 0;
	for (int i = 0; i < eol - 2; i++)
	{
		if (t[i].type == ASSIGNMENT_TOKEN)
			assignment = i;
		if (t[i].type == COMMA_TOKEN)
		{
			if (t[last_p].type != IDENTIFIER_TOKEN)
			{
				ERROR err;
				err.code = EXPECTED_TOKEN_NOT_FOUND;
				err.line = getLine_func();
				err.extra = (char *)malloc(11 * sizeof(char));
				err.clear = 1;
				strcpy(err.extra, "IDENTIFIER");
				load_error(err);
				*success = 0;
				return memory;
			}
			char *identifier_name = t[last_p].data;
			addVar(identifier_name, var_type);
			if (assignment)
			{
				TYPE tp = value_instructions(t + assignment + 1, i - assignment - 1, memory, ptr, success);
				if (tp.typeid != var_type)
				{
					ERROR err;
					err.code = INCOMPATIBLE_TYPES;
					err.line = getLine_func();
					err.extra = (char *)malloc(64 * sizeof(char));
					err.clear = 1;
					err.extra[0] = '\0';
					strcat(err.extra, types_typename(var_type));
					strcat(err.extra, " and ");
					strcat(err.extra, types_typename(tp.typeid));
					load_error(err);
					*success = 0;
					return memory;
				}
				char *variable_name = varname(identifier_name);
				if (var_type == NUMBER_TYPE)
				{
					strcpy(memory[*ptr].operation, "movq");
					strcpy(memory[*ptr].operand1, "%rax");
					strcpy(memory[*ptr].operand2, variable_name);
					memory[*ptr].operand3[0] = '\0';
					(*ptr)++;
				}
				else if (var_type == STRING_TYPE)
				{
					strcpy(memory[*ptr].operation, "movq");
					strcpy(memory[*ptr].operand1, "%rax");
					strcpy(memory[*ptr].operand2, variable_name);
					memory[*ptr].operand3[0] = '\0';
					(*ptr)++;

					strcpy(memory[*ptr].operation, "movq");
					strcpy(memory[*ptr].operand1, "%rbx");
					strcpy(memory[*ptr].operand2, variable_name);
					strcat(memory[*ptr].operand2, "+8");
					memory[*ptr].operand3[0] = '\0';
					(*ptr)++;
				}
				assignment = 0;
			}
			last_p = i + 1;
		}
	}
	if (t[last_p].type != IDENTIFIER_TOKEN)
	{
		ERROR err;
		err.code = EXPECTED_TOKEN_NOT_FOUND;
		err.line = getLine_func();
		err.extra = (char *)malloc(11 * sizeof(char));
		err.clear = 1;
		strcpy(err.extra, "IDENTIFIER");
		load_error(err);
		*success = 0;
		return memory;
	}
	char *identifier_name = t[last_p].data;
	addVar(identifier_name, var_type);
	int i = eol - 2;
	if (assignment)
	{
		TYPE tp = value_instructions(t + assignment + 1, i - assignment - 1, memory, ptr, success);
		if (tp.typeid != var_type)
		{
			ERROR err;
			err.code = INCOMPATIBLE_TYPES;
			err.line = getLine_func();
			err.extra = (char *)malloc(64 * sizeof(char));
			err.clear = 1;
			err.extra[0] = '\0';
			strcat(err.extra, types_typename(var_type));
			strcat(err.extra, " and ");
			strcat(err.extra, types_typename(tp.typeid));
			load_error(err);
			*success = 0;
			return memory;
		}
		char *variable_name = varname(identifier_name);
		if (var_type == NUMBER_TYPE)
		{
			strcpy(memory[*ptr].operation, "movq");
			strcpy(memory[*ptr].operand1, "%rax");
			strcpy(memory[*ptr].operand2, variable_name);
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;
		}
		else if (var_type == STRING_TYPE)
		{
			strcpy(memory[*ptr].operation, "movq");
			strcpy(memory[*ptr].operand1, "%rax");
			strcpy(memory[*ptr].operand2, variable_name);
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;

			strcpy(memory[*ptr].operation, "movq");
			strcpy(memory[*ptr].operand1, "%rbx");
			strcpy(memory[*ptr].operand2, variable_name);
			strcat(memory[*ptr].operand2, "+8");
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;
		}
		assignment = 0;
	}
	return memory;
}

int end_of_line(Token *t)
{
	int i;
	for (i = 0; t[i].type != FILE_END_TOKEN && t[i].type != NEW_LINE_TOKEN; i++)
		;
	return i;
}
