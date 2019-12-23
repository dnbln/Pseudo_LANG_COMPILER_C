/*
 * compiler.c
 *
 *  Created on: Oct 25, 2019
 *      Author: dinu
 */

#include "../include/compiler.h"
#include "../include/functions.h"
#include "../include/utils.h"
#include "../include/operators.h"
#include "../include/errors.h"
#include <stdlib.h>
#include <string.h>

int compiler_line;

void Compile(COMPILER_INTERNAL *internal_state, int *success)
{
	internal_state->asmop_memptr = 0;
	*success = 1;
	compiler_line = 1;
	for (int i = 0; *success == 1 && i < internal_state->STATEMENT_POINTER; i++)
		Compile_Statement(internal_state, internal_state->statements[i],
						  success);
}

void Compile_Statement(COMPILER_INTERNAL *internal_state, Statement s,
					   int *success)
{
	const int type = s.type;
	*success = 1;
	compiler_line = s.line;
	if (type == FUNCTION_CALL_STATEMENT)
	{
		long long *statement_data = s.data;
		ASM_FUNC_GENERATE *generate_assembly =
			(ASM_FUNC_GENERATE *)statement_data[0];
		Token *tokens = (Token *)statement_data[1];
		generate_assembly(tokens, internal_state->asmop_mem,
						  &(internal_state->asmop_memptr), success);
	}
	else if (type == COMPOSITE_STATEMENT)
	{
		Composite_Statement *data = (Composite_Statement *)s.data;
		for (int i = 0; i < data->inside_statements_count; i++)
			Compile_Statement(internal_state, *data->statements[i], success);
	}
}

TYPE compiler_value_instructions(Token *t, int n, ASMOP *memory, int *ptr,
								 int *success)
{
	TYPE type;
	type.typeid = VOID_TYPE;
	if (n == 0)
	{
		return type;
	}
	if (n == 1)
	{
		if (t->type == NUMBER_TOKEN)
		{
			strcpy(memory[*ptr].operation, "movq");
			char str_[20];
			num_to_str(*((long long *)t->data), str_ + 1);
			str_[0] = '$';
			strcpy(memory[*ptr].operand1, str_);
			strcpy(memory[*ptr].operand2, "%rax");
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;
			type.typeid = NUMBER_TYPE;
			return type;
		}
		else if (t->type == STRING_TOKEN)
		{
			char *name = compiler_loadString((char *)t->data);
			strcpy(memory[*ptr].operation, "movq");
			memory[*ptr].operand1[0] = '$';
			strcpy(memory[*ptr].operand1 + 1, name);
			strcpy(memory[*ptr].operand2, "%rax");
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;

			strcpy(memory[*ptr].operation, "movq");
			memory[*ptr].operand1[0] = '$';
			num_to_str(compute_length((char *)t->data), memory[*ptr].operand1 + 1);
			strcpy(memory[*ptr].operand2, "%rbx");
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;

			type.typeid = STRING_TYPE;
			return type;
		}
	}
	// n >= 2
	for (int i = 0; i < n; i++)
	{
		if (t[i].type == P_OPEN_TOKEN)
		{
			int depth = 1, j;
			for (j = i + 1; j < n; j++)
			{
				if (t[j].type == P_OPEN_TOKEN)
				{
					depth++;
				}
				else if (t[j].type == P_CLOSED_TOKEN)
				{
					depth--;
					if (depth == 0)
						break;
				}
			}
			int len = j - i - 1;
			TYPE parenthesis_result_type = compiler_value_instructions(t + i + 1, len, memory, ptr, success);
			t[i].data = &t[j];
			t[j].data = &t[i];
			t[i].extra_data = t[j].extra_data = save_result(parenthesis_result_type, memory, ptr, success);
			i = j + 1;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == LOGIC_OP_TOKEN)
		{
			CACHE_PTR left = save_result(compiler_value_instructions(t, i, memory, ptr, success), memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory, ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type, right, memory, ptr, success);
			return result;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == EQUALITY_OP_TOKEN)
		{
			CACHE_PTR left = save_result(compiler_value_instructions(t, i, memory, ptr, success), memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory, ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type, right, memory, ptr, success);
			return result;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == ARITH_OP_TOKEN && ((unsigned long long int)t[i].data == ADD || (unsigned long long int)t[i].data == SUBTRACT))
		{
			CACHE_PTR left = save_result(compiler_value_instructions(t, i, memory, ptr, success), memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory, ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type, right, memory, ptr, success);
			return result;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == ARITH_OP_TOKEN && ((unsigned long long int)t[i].data == MULTIPLY || (unsigned long long int)t[i].data == DIVIDE))
		{
			CACHE_PTR left = save_result(compiler_value_instructions(t, i, memory, ptr, success), memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory, ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type, right, memory, ptr, success);
			return result;
		}
	}
	return type;
}

void make_signature(unsigned long long op, int op_class, TYPE type1, TYPE type2, char sig[64])
{
	sig[0] = '\0';
	if (op_class == ARITH_OP_TOKEN)
	{
		switch (op)
		{
		case ADD:
			strcpy(sig, "add");
			break;
		case SUBTRACT:
			strcpy(sig, "subtract");
			break;
		default:
			break;
		}

		if (type1.typeid == NUMBER_TYPE)
			strcat(sig, "<Number");
		else if (type1.typeid == STRING_TYPE)
			strcat(sig, "<String");
		else if (type1.typeid == ERROR_TYPE)
			strcat(sig, "<Error");
		else if (type1.typeid == VOID_TYPE)
			strcat(sig, "<Void");

		if (type2.typeid == NUMBER_TYPE)
			strcat(sig, ",Number>");
		else if (type2.typeid == STRING_TYPE)
			strcat(sig, ",String>");
		else if (type2.typeid == ERROR_TYPE)
			strcat(sig, ",Error>");
		else if (type2.typeid == VOID_TYPE)
			strcat(sig, ",Void>");
	}
}

TYPE callOperator(CACHE_PTR a, unsigned long long op, int op_class, TYPE b, ASMOP *memory, int *ptr, int *success)
{
	char sig[64];
	make_signature(op, op_class, a->data_type, b, sig);
	OPERATOR *opr = getOperator(sig);
	if (opr == NULL)
	{
		ERROR err;
		err.code = OPERATOR_NOT_FOUND;
		err.line = compiler_getLine();
		err.extra = malloc(64 * sizeof(char));
		err.clear = 1;
		strcpy((char*) err.extra, sig);
		load_error(err);
		*success = 0;
		TYPE t;
		t.typeid = ERROR_TYPE;
		return t;
	}
	return opr->call(a, b, memory, ptr, success);
}

char *compiler_varname(const char *identifier_name)
{
	return NULL;
}

char *compiler_loadString(const char *stringValue)
{
	int index = compiler_lookUpString(stringValue);
	if (index != -1)
	{
		return compiler_stringName(index);
	}
	else
	{
		return compiler_addString(stringValue);
	}
}

STRINGS_LIST string_list;

int compiler_lookUpString(const char *stringVal)
{
	for (size_t i = 0; i < string_list.string_count; i++)
	{
		if (strcmp(string_list.strings[i], stringVal) == 0)
		{
			return i;
		}
	}
	return -1;
}

char *compiler_stringName(int index)
{
	return string_list.strName[index];
}

char *compiler_addString(const char *stringVal)
{
	strcpy(string_list.strings[string_list.string_count], stringVal);
	strcpy(string_list.strName[string_list.string_count], "str");
	num_to_str(string_list.string_count, string_list.strName[string_list.string_count] + 3);
	return string_list.strName[string_list.string_count++];
}

CACHE_PTR base;
CACHE_PTR current;
size_t cache_offset;

void compiler_init()
{
	string_list.string_count = 0;
	cache_init();
	cache_clear();
	operators_init();
}

void cache_init()
{
	base = (CACHE_PTR)malloc(CACHE_SIZE * sizeof(CACHE));
}

void cache_clear()
{
	current = base;
	cache_offset = 0U;
}

void compiler_writeDataAndVariables(FILE *f)
{
	if (string_list.string_count)
	{
		fprintf(f, ".section .rodata\n");
		for (size_t i = 0; i < string_list.string_count; i++)
		{
			fprintf(f, "%s:\n\t.asciz \"%s\"\n", string_list.strName[i], string_list.strings[i]);
		}
	}
	fprintf(f, ".section .bss\n\t.lcomm CACHE_MEM, 524288\n");
}

CACHE_PTR save_result(TYPE result_type, ASMOP *memory, int *ptr, int *success)
{
	if (result_type.typeid == NUMBER_TYPE)
	{
		strcpy(memory[*ptr].operation, "movq");
		strcpy(memory[*ptr].operand1, "%rax");
		strcpy(memory[*ptr].operand2, "CACHE_MEM+");
		num_to_str(cache_offset, memory[*ptr].operand2 + 10);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;

		current->cache_index = cache_offset;
		current->data_type = result_type;
		cache_offset += 8;
		return current++;
	}
	else if (result_type.typeid == STRING_TYPE)
	{
		strcpy(memory[*ptr].operation, "movq");
		strcpy(memory[*ptr].operand1, "%rax");
		strcpy(memory[*ptr].operand2, "CACHE_MEM+");
		num_to_str(cache_offset, memory[*ptr].operand2 + 10);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;

		strcpy(memory[*ptr].operation, "callq");
		strcpy(memory[*ptr].operand1, "_load_strlen_to_rbx@PLT");
		memory[*ptr].operand2[0] = '\0';
		(*ptr)++;

		strcpy(memory[*ptr].operation, "movq");
		strcpy(memory[*ptr].operand1, "%rbx");
		strcpy(memory[*ptr].operand2, "CACHE_MEM+");
		num_to_str(cache_offset + 8, memory[*ptr].operand2 + 10);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;

		current->cache_index = cache_offset;
		current->data_type = result_type;
		cache_offset += 16;
		return current++;
	}
	return NULL;
}

int compiler_getLine()
{
	return compiler_line;
}