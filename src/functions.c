/*
 * functions.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */
#include "../include/functions.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PREDEFINED_FUNCTIONS 1

Function declared_functions[PREDEFINED_FUNCTIONS];

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

ASMOP *print_generate_assembly(Token *t, ASMOP *memory, int *size, int *success);
char *(*_varname)(const char *identifier_name);
TYPE(*value_instructions)
(Token *, int, ASMOP *, int *, int *);

void Init_Functions(char *(*varname)(const char *identifier_name),
					TYPE (*v_instructions)(Token *, int, ASMOP *, int *, int *))
{
	strcpy(declared_functions[0].name, "print");
	declared_functions[0].generate_assembly = &print_generate_assembly;
	_varname = varname;
	value_instructions = v_instructions;
}

void print_num_handler(ASMOP *memory, int *ptr, int *success)
{
	strcpy(memory[*ptr].operation, "callq");
	strcpy(memory[*ptr].operand1, "_print_number@PLT");
	memory[*ptr].operand2[0] = '\0';
	(*ptr)++;
}

void print_str_handler(ASMOP *memory, int *ptr, int *success)
{
	strcpy(memory[*ptr].operation, "callq");
	strcpy(memory[*ptr].operand1, "_load_strlen_to_rbx@PLT");
	memory[*ptr].operand2[0] = '\0';
	(*ptr)++;
	strcpy(memory[*ptr].operation, "callq");
	strcpy(memory[*ptr].operand1, "_print_string@PLT");
	memory[*ptr].operand2[0] = '\0';
	(*ptr)++;
}

ASMOP *print_generate_assembly(Token *t, ASMOP *memory, int *ptr, int *success)
{
	int eol = end_of_line(t);
	*success = 1;
	if ((eol > 2 && t[eol - 2].type == FROM_TO_TOKEN) || (eol > 3 && t[eol - 3].type == FROM_TO_TOKEN))
	{
		strcpy(memory[*ptr].operation, "movq");
		if (t[eol - 2].type == FROM_TO_TOKEN && t[eol - 1].type == CONSOLE_TOKEN)
			strcpy(memory[*ptr].operand1, "$1");
		else if (t[eol - 3].type == FROM_TO_TOKEN && t[eol - 2].type == FILE_TOKEN && t[eol - 1].type == IDENTIFIER_TOKEN)
			strcpy(memory[*ptr].operand1, _varname(t[eol - 1].data));
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
				// movq %rax, %rax # does nothing
				print_num_handler(memory, ptr, success);
			}
			else if (type.typeid == STRING_TYPE)
			{
				// movq %rax, %rax # does nothing
				print_str_handler(memory, ptr, success);
			}
		}
	}

	int len = eol - last_p;
	TYPE type = value_instructions(t + last_p, len, memory, ptr, success);
	if (type.typeid == NUMBER_TYPE)
	{
		// movq %rax, %rax # does nothing
		print_num_handler(memory, ptr, success);
	}
	else if (type.typeid == STRING_TYPE)
	{
		// movq %rax, %rax # does nothing
		print_str_handler(memory, ptr, success);
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
