/*
 * compiler.c
 *
 *  Created on: Oct 25, 2019
 *      Author: dinu
 */

#include "../include/compiler.h"

#include <stdlib.h>
#include <string.h>

#include "../include/errors.h"
#include "../include/functions.h"
#include "../include/labels.h"
#include "../include/operators.h"
#include "../include/utils.h"
#include "../include/vars.h"

int compiler_line;

OPTS *(*getOpts)();

void Compile(COMPILER_INTERNAL *internal_state, int *success)
{
	internal_state->asmop_memptr = 0;
	compiler_line = 1;
	for (int i = 0; *success == 1 && i < internal_state->STATEMENT_POINTER; i++)
	{
		Compile_Statement(internal_state, internal_state->statements[i], success);
	}
}

void Compile_Statement(COMPILER_INTERNAL *internal_state,
					   Statement s,
					   int *success)
{
	const int type = s.type;
	compiler_line = s.line;
	cache_clear();
	if (type == FUNCTION_CALL_STATEMENT)
	{
		long long *statement_data = s.data;
		ASM_FUNC_GENERATE *generate_assembly =
			(ASM_FUNC_GENERATE *)statement_data[0];
		Token *tokens = (Token *)statement_data[1];
		generate_assembly(tokens, internal_state->asmop_mem,
						  &(internal_state->asmop_memptr), success);
	}
	else if (type == ASSIGNMENT_STATEMENT)
	{
		Token *tokenptr = internal_state->tokens + (long long)s.data;
		Token *assignment_token_adr = tokenptr;
		while (assignment_token_adr->type != ASSIGNMENT_TOKEN && assignment_token_adr->type != NEW_LINE_TOKEN)
		{
			assignment_token_adr++;
		}
		if (assignment_token_adr->type == NEW_LINE_TOKEN)
		{
			ERROR err;
			err.code = EXPECTED_TOKEN_NOT_FOUND;
			err.line = compiler_getLine();
			err.extra = (char *)malloc(10 * sizeof(char));
			err.clear = 1;
			strcpy(err.extra, "ASSIGNMENT TOKEN");
			load_error(err);
			*success = 0;
			return;
		}
		if (tokenptr->type != IDENTIFIER_TOKEN)
		{
			ERROR err;
			err.code = EXPECTED_TOKEN_NOT_FOUND;
			err.line = compiler_getLine();
			err.extra = (char *)malloc(10 * sizeof(char));
			err.clear = 1;
			strcpy(err.extra, "IDENTIFIER TOKEN");
			load_error(err);
			*success = 0;
			return;
		}
		char *variable_name = varname(tokenptr->data);
		if (variable_name == NULL)
		{
			ERROR err;
			err.code = VARIABLE_NOT_FOUND;
			err.line = compiler_getLine();
			err.extra = (char *)malloc(64 * sizeof(char));
			err.clear = 1;
			strcpy(err.extra, tokenptr->data);
			load_error(err);
			*success = 0;
			return;
		}
		tokenptr = assignment_token_adr + 1;
		while (tokenptr->type != NEW_LINE_TOKEN)
		{
			tokenptr++;
		}
		TYPE t = compiler_value_instructions(assignment_token_adr + 1, tokenptr - assignment_token_adr - 1, internal_state->asmop_mem, &(internal_state->asmop_memptr), success);
		unsigned long long int vartype = getVarType(variable_name);
		if(t.typeid != vartype){
			ERROR err;
			err.code = INCOMPATIBLE_TYPES;
			err.line = compiler_getLine();
			err.extra = (char *)malloc(64 * sizeof(char));
			err.clear = 1;
			err.extra[0] = '\0';
			strcat(err.extra, types_typename(vartype));
			strcat(err.extra, " and ");
			strcat(err.extra, types_typename(t.typeid));
			load_error(err);
			*success = 0;
			return;
		}
		if(vartype == NUMBER_TYPE){
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operation, "movq");
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operand1, "%rax");
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operand2, variable_name);
			internal_state->asmop_mem[internal_state->asmop_memptr].operand3[0] = '\0';
			internal_state->asmop_memptr++;
		}
		else if(vartype == STRING_TYPE){
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operation, "movq");
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operand1, "%rax");
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operand2, variable_name);
			internal_state->asmop_mem[internal_state->asmop_memptr].operand3[0] = '\0';
			internal_state->asmop_memptr++;

			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operation, "movq");
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operand1, "%rbx");
			strcpy(internal_state->asmop_mem[internal_state->asmop_memptr].operand2, variable_name);
			strcat(internal_state->asmop_mem[internal_state->asmop_memptr].operand2, "+8");
			internal_state->asmop_mem[internal_state->asmop_memptr].operand3[0] = '\0';
			internal_state->asmop_memptr++;
		}
	}
	else if (type == COMPOSITE_STATEMENT)
	{
		Composite_Statement *data = (Composite_Statement *)s.data;
		for (int i = 0; i < data->inside_statements_count; i++)
			Compile_Statement(internal_state, *data->statements[i], success);
	}
	else if (type == CONDITIONAL_STATEMENT)
	{
		Conditional_Statement *statement_data = (Conditional_Statement *)s.data;
		cond_generate_assembly(internal_state, statement_data,
							   internal_state->asmop_mem,
							   &(internal_state->asmop_memptr), success);
	}
}

TYPE compiler_value_instructions(Token *t,
								 int n,
								 ASMOP *memory,
								 size_t *ptr,
								 int *success)
{
	TYPE type = makeType();
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
		else if (t->type == IDENTIFIER_TOKEN)
		{
			char *name = varname((const char *)t->data);
			type.typeid = getVarType(name);
			if (type.typeid == NUMBER_TYPE)
			{
				strcpy(memory[*ptr].operation, "movq");
				strcpy(memory[*ptr].operand1, name);
				strcpy(memory[*ptr].operand2, "%rax");
				memory[*ptr].operand3[0] = '\0';
				(*ptr)++;
			}
			else if (type.typeid == STRING_TYPE)
			{
				strcpy(memory[*ptr].operation, "movq");
				strcpy(memory[*ptr].operand1, name);
				strcpy(memory[*ptr].operand2, "%rax");
				memory[*ptr].operand3[0] = '\0';
				(*ptr)++;

				strcpy(memory[*ptr].operation, "movq");
				strcpy(memory[*ptr].operand1, name);
				strcat(memory[*ptr].operand1, "+8");
				strcpy(memory[*ptr].operand2, "%rbx");
				memory[*ptr].operand3[0] = '\0';
				(*ptr)++;
			}
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
			if (t[i].data != NULL)
			{
				if (i == 0 && j == n - 1)
				{
					CACHE_PTR cache = t[i].data;
					strcpy(memory[*ptr].operation, "movq");
					strcpy(memory[*ptr].operand1, "CACHE_MEM+");
					num_to_str(cache->cache_index, memory[*ptr].operand1 + 10);
					strcpy(memory[*ptr].operand2, "%rax");
					memory[*ptr].operand3[0] = '\0';
					(*ptr)++;
					return cache->data_type;
				}
				i = j;
				continue;
			}
			int len = j - i - 1;
			TYPE parenthesis_result_type =
				compiler_value_instructions(t + i + 1, len, memory, ptr, success);
			CACHE_PTR retptr =
				save_result(&parenthesis_result_type, memory, ptr, success);
			t[i].data = retptr;
			t[j].data = &t[i];
			if (i == 0 && j == n - 1)
			{
				return parenthesis_result_type;
			}
			i = j;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == P_CLOSED_TOKEN)
		{
			i = ((Token *)t[i].data) - t + 1;
			continue;
		}
		if (t[i].type == LOGIC_OP_TOKEN)
		{
			TYPE left_type = compiler_value_instructions(t, i, memory, ptr, success);
			CACHE_PTR left = save_result(&left_type, memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory,
													 ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type,
									   right, memory, ptr, success);
			return result;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == P_CLOSED_TOKEN)
		{
			i = ((Token *)t[i].data) - t + 1;
			continue;
		}
		if (t[i].type == EQUALITY_OP_TOKEN)
		{
			TYPE left_type = compiler_value_instructions(t, i, memory, ptr, success);
			CACHE_PTR left = save_result(&left_type, memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory,
													 ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type,
									   right, memory, ptr, success);
			return result;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == P_CLOSED_TOKEN)
		{
			i = ((Token *)t[i].data) - t + 1;
			continue;
		}
		if (t[i].type == ARITH_OP_TOKEN &&
			((unsigned long long int)t[i].data == ADD ||
			 (unsigned long long int)t[i].data == SUBTRACT))
		{
			TYPE left_type = compiler_value_instructions(t, i, memory, ptr, success);
			CACHE_PTR left = save_result(&left_type, memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory,
													 ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type,
									   right, memory, ptr, success);
			return result;
		}
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (t[i].type == P_CLOSED_TOKEN)
		{
			i = ((Token *)t[i].data) - t + 1;
			continue;
		}
		if (t[i].type == ARITH_OP_TOKEN &&
			((unsigned long long int)t[i].data == MULTIPLY ||
			 (unsigned long long int)t[i].data == DIVIDE))
		{
			TYPE left_type = compiler_value_instructions(t, i, memory, ptr, success);
			CACHE_PTR left = save_result(&left_type, memory, ptr, success);
			TYPE right = compiler_value_instructions(t + i + 1, n - i - 1, memory,
													 ptr, success);
			TYPE result = callOperator(left, (unsigned long long)t[i].data, t[i].type,
									   right, memory, ptr, success);
			return result;
		}
	}
	return type;
}

void make_signature(unsigned long long op,
					int op_class,
					TYPE type1,
					TYPE type2,
					char sig[64])
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
		case MULTIPLY:
			strcpy(sig, "multiply");
			break;
		default:
			break;
		}

		strcat(sig, "<");
		strcat(sig, types_typename(type1.typeid));
		strcat(sig, ",");
		strcat(sig, types_typename(type2.typeid));
		strcat(sig, ">");
	}
	else if (op_class == EQUALITY_OP_TOKEN)
	{
		switch (op)
		{
		case EQ:
			strcpy(sig, "eq");
			break;
		case NEQ:
			strcpy(sig, "neq");
			break;
		case LEQ:
			strcpy(sig, "leq");
			break;
		case GEQ:
			strcpy(sig, "geq");
			break;
		case G:
			strcpy(sig, "g");
			break;
		case L:
			strcpy(sig, "l");
			break;
		default:
			break;
		}

		strcat(sig, "<");
		strcat(sig, types_typename(type1.typeid));
		strcat(sig, ",");
		strcat(sig, types_typename(type2.typeid));
		strcat(sig, ">");
	}
}

TYPE callOperator(CACHE_PTR a,
				  unsigned long long op,
				  int op_class,
				  TYPE b,
				  ASMOP *memory,
				  size_t *ptr,
				  int *success)
{
	char sig[64] = {};
	make_signature(op, op_class, a->data_type, b, sig);
	OPERATOR *opr = getOperator(sig);
	if (opr == NULL)
	{
		ERROR err;
		err.code = OPERATOR_NOT_FOUND;
		err.line = compiler_getLine();
		err.extra = malloc(64 * sizeof(char));
		err.clear = 1;
		strcpy((char *)err.extra, sig);
		load_error(err);
		*success = 0;
		TYPE t = makeType();
		t.typeid = ERROR_TYPE;
		return t;
	}
	return opr->call(a, b, memory, ptr, success);
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
	num_to_str(string_list.string_count,
			   string_list.strName[string_list.string_count] + 3);
	return string_list.strName[string_list.string_count++];
}

CACHE_PTR base;
CACHE_PTR current;
size_t cache_offset;

void compiler_init(OPTS *(*getOptsFunc)())
{
	string_list.string_count = 0;
	cache_init();
	cache_clear();
	operators_init();
	labels_init();
	getOpts = getOptsFunc;
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

void cache_delete()
{
	free(base);
}

void compiler_writeDataAndVariables(FILE *f)
{
	if (string_list.string_count)
	{
		fprintf(f, ".section .rodata\n");
		for (size_t i = 0; i < string_list.string_count; i++)
		{
			fprintf(f, "%s:\n\t.ascii \"%s\"\n", string_list.strName[i],
					string_list.strings[i]);
		}
	}
	fprintf(f, ".section .bss\n\t.lcomm CACHE_MEM, 524288\n\t.lcomm STRINGS_POOL, %ld\n\t.lcomm STRINGS_POOL_PTR, 8\n", getOpts()->stringPoolSize);
	vars_print(f);
}

CACHE_PTR
save_result(TYPE *result_type, ASMOP *memory, size_t *ptr, int *success)
{
	if (result_type->saved != NULL)
		return result_type->saved;
	if (result_type->typeid == NUMBER_TYPE)
	{
		strcpy(memory[*ptr].operation, "movq");
		strcpy(memory[*ptr].operand1, "%rax");
		strcpy(memory[*ptr].operand2, "CACHE_MEM+");
		num_to_str(cache_offset, memory[*ptr].operand2 + 10);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;

		current->cache_index = cache_offset;
		result_type->saved = (void *)current;
		current->data_type = *result_type;
		cache_offset += 8;
		CACHE_PTR ret = current;
		current++;
		return ret;
	}
	else if (result_type->typeid == STRING_TYPE)
	{
		strcpy(memory[*ptr].operation, "movq");
		strcpy(memory[*ptr].operand1, "%rax");
		strcpy(memory[*ptr].operand2, "CACHE_MEM+");
		num_to_str(cache_offset, memory[*ptr].operand2 + 10);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;

		strcpy(memory[*ptr].operation, "movq");
		strcpy(memory[*ptr].operand1, "%rbx");
		strcpy(memory[*ptr].operand2, "CACHE_MEM+");
		num_to_str(cache_offset + 8, memory[*ptr].operand2 + 10);
		memory[*ptr].operand3[0] = '\0';
		(*ptr)++;

		current->cache_index = cache_offset;
		result_type->saved = (void *)current;
		current->data_type = *result_type;
		cache_offset += 16;
		CACHE_PTR ret = current;
		current++;
		return ret;
	}
	else if (result_type->typeid == VOID_TYPE ||
			 result_type->typeid == ERROR_TYPE)
	{
		result_type->saved = (void *)current;
		current->data_type = *result_type;
		current->cache_index = -1;
		CACHE_PTR ret = current;
		current++;
		return ret;
	}
	return current++;
}

int compiler_getLine()
{
	return compiler_line;
}

void cond_generate_assembly(COMPILER_INTERNAL *internal_state,
							Conditional_Statement *statement,
							ASMOP *memory,
							size_t *ptr,
							int *success)
{
	TYPE t = compiler_value_instructions(
		statement->condition.ptr, statement->condition.tn, memory, ptr, success);
	if (*success == 0)
		return;
	if (t.typeid != NUMBER_TYPE)
		return;
	strcpy(memory[*ptr].operation, "cmpq");
	strcpy(memory[*ptr].operand1, "$0");
	strcpy(memory[*ptr].operand2, "%rax");
	memory[*ptr].operand3[0] = '\0';
	(*ptr)++;
	LABEL onFalse = make_label();
	LABEL end = make_label();
	strcpy(memory[*ptr].operation, "je");
	if (statement->has_false)
		strcpy(memory[*ptr].operand1, onFalse.label_name);
	else
		strcpy(memory[*ptr].operand1, end.label_name);
	memory[*ptr].operand2[0] = '\0';
	(*ptr)++;
	Compile_Statement(internal_state, statement->onTrue, success);
	if (statement->has_false)
	{
		strcpy(memory[*ptr].operation, "jmp");
		strcpy(memory[*ptr].operand1, end.label_name);
		memory[*ptr].operand2[0] = '\0';
		(*ptr)++;
		strcpy(memory[*ptr].operation, onFalse.label_name);
		strcat(memory[*ptr].operation, ":");
		memory[*ptr].operand1[0] = '\0';
		(*ptr)++;
		Compile_Statement(internal_state, statement->onFalse, success);
	}
	strcpy(memory[*ptr].operation, end.label_name);
	strcat(memory[*ptr].operation, ":");
	memory[*ptr].operand1[0] = '\0';
	(*ptr)++;
}

void clean_str_pool_func(ASMOP *mem, size_t *ptr, int *success)
{
	strcpy(mem[*ptr].operation, "movq");
	OPTS *opts = getOpts();
	strcpy(mem[*ptr].operand1, "$");
	num_to_str(opts->stringPoolSize, mem[*ptr].operand1 + 1);
	strcpy(mem[*ptr].operand2, "STRINGS_POOL_PTR");
	mem[*ptr].operand3[0] = '\0';
	(*ptr)++;
}

typedef struct const_hashmap_elem
{
	char key[OPERAND_SIZE];
	char value[OPERAND_SIZE];
	unsigned is_const;
	struct const_hashmap_elem *next;
} const_hashmap_elem;

typedef struct
{
	unsigned value;
	const_hashmap_elem *elem;
} is_const_answer;

void replace_with_const(char *operand, const_hashmap_elem *elem);
is_const_answer is_const(char *operand);
void set_const(char *operand, char *value, unsigned is_const);
unsigned isImmediate(char *operand);
const_hashmap_elem *head;
unsigned flags;
unsigned flags_const;

void remove_until_label(ASMOP *mem, size_t size, size_t i);
size_t indexOfLabel(ASMOP *mem, size_t i, size_t size, char *label);

void print_const_map();

#define E_FLAG 1
#define G_FLAG 2
#define L_FLAG 4

size_t countLabelOccurances(ASMOP *mem, size_t size, char *label);
size_t nextOp(ASMOP *mem, size_t size, size_t i);
size_t prevOp(ASMOP *mem, size_t size, size_t i);

void const_optimize(ASMOP *mem, size_t size)
{
	flags_const = 0;
	for (size_t i = 0; i < size; i++)
	{
		if (mem[i].operation[0] == '\0' || strncmp(mem[i].operation, "mul", 3) == 0 || strncmp(mem[i].operation, "div", 3) == 0)
			continue;
		if (mem[i].operand1[0] == '\0')
			continue;
		if (mem[i].operation[strlen(mem[i].operand1) - 1] == ':') // is label?
		{
			if (countLabelOccurances(mem, size, mem[i].operation) == 0)
			{
				mem[i].operation[0] = '\0';
				continue;
			}
			if (strcmp(mem[i].operand2, "LOOP GOBACK") == 0)
				for (const_hashmap_elem *ptr = head->next; ptr != NULL; ptr = ptr->next)
				{
					ptr->is_const = 0;
				}
		}
		if (mem[i].operation[0] == 'j' && flags_const)
		{
			if (strncmp(mem[i].operation, "jne", 3) == 0)
			{
				if (flags & (G_FLAG | L_FLAG))
				{
					strcpy(mem[i].operation, "jmp");
					remove_until_label(mem, size, i + 1);
					size_t prev = i;
					i = indexOfLabel(mem, i, size, mem[i].operand1);
					size_t next = nextOp(mem, size, prev);
					size_t label_length = strlen(mem[prev].operand1);
					if (strncmp(mem[prev].operand1, mem[next].operation, label_length) == 0 && strlen(mem[next].operation) == label_length + 1)
					{
						mem[prev].operation[0] = '\0';
					}
				}
				else
					mem[i].operation[0] = '\0';
				continue;
			}
			if (strncmp(mem[i].operation, "je", 2) == 0)
			{
				if (flags & E_FLAG)
				{
					strcpy(mem[i].operation, "jmp");
					remove_until_label(mem, size, i + 1);
					size_t prev = i;
					i = indexOfLabel(mem, i, size, mem[i].operand1);
					size_t next = nextOp(mem, size, prev);
					size_t label_length = strlen(mem[prev].operand1);
					if (strncmp(mem[prev].operand1, mem[next].operation, label_length) == 0 && strlen(mem[next].operation) == label_length + 1)
					{
						mem[prev].operation[0] = '\0';
					}
				}
				else
					mem[i].operation[0] = '\0';
				continue;
			}
			if (strncmp(mem[i].operation, "jg", 2) == 0)
			{
				if (flags & G_FLAG)
				{
					strcpy(mem[i].operation, "jmp");
					remove_until_label(mem, size, i + 1);
					size_t prev = i;
					i = indexOfLabel(mem, i, size, mem[i].operand1);
					size_t next = nextOp(mem, size, prev);
					size_t label_length = strlen(mem[prev].operand1);
					if (strncmp(mem[prev].operand1, mem[next].operation, label_length) == 0 && strlen(mem[next].operation) == label_length + 1)
					{
						mem[prev].operation[0] = '\0';
					}
				}
				else
					mem[i].operation[0] = '\0';
				continue;
			}
			if (strncmp(mem[i].operation, "jl", 2) == 0)
			{
				if (flags & L_FLAG)
				{
					strcpy(mem[i].operation, "jmp");
					remove_until_label(mem, size, i + 1);
					size_t prev = i;
					i = indexOfLabel(mem, i, size, mem[i].operand1);
					size_t next = nextOp(mem, size, prev);
					size_t label_length = strlen(mem[prev].operand1);
					if (strncmp(mem[prev].operand1, mem[next].operation, label_length) == 0 && strlen(mem[next].operation) == label_length + 1)
					{
						mem[prev].operation[0] = '\0';
					}
				}
				else
					mem[i].operation[0] = '\0';
				continue;
			}
			if (strncmp(mem[i].operation, "jge", 3) == 0)
			{
				if (flags & (G_FLAG | E_FLAG))
				{
					strcpy(mem[i].operation, "jmp");
					remove_until_label(mem, size, i + 1);
					size_t prev = i;
					i = indexOfLabel(mem, i, size, mem[i].operand1);
					size_t next = nextOp(mem, size, prev);
					size_t label_length = strlen(mem[prev].operand1);
					if (strncmp(mem[prev].operand1, mem[next].operation, label_length) == 0 && strlen(mem[next].operation) == label_length + 1)
					{
						mem[prev].operation[0] = '\0';
					}
				}
				else
					mem[i].operation[0] = '\0';
				continue;
			}
			if (strncmp(mem[i].operation, "jle", 3) == 0)
			{
				if (flags & (L_FLAG | E_FLAG))
				{
					strcpy(mem[i].operation, "jmp");
					remove_until_label(mem, size, i + 1);
					size_t prev = i;
					i = indexOfLabel(mem, i, size, mem[i].operand1);
					size_t next = nextOp(mem, size, prev);
					size_t label_length = strlen(mem[prev].operand1);
					if (strncmp(mem[prev].operand1, mem[next].operation, label_length) == 0 && strlen(mem[next].operation) == label_length + 1)
					{
						mem[prev].operation[0] = '\0';
					}
				}
				else
					mem[i].operation[0] = '\0';
				continue;
			}
			if (strncmp(mem[i].operation, "jmp", 3) == 0)
			{
				remove_until_label(mem, size, i + 1);
				size_t prev = i;
				i = indexOfLabel(mem, i, size, mem[i].operand1);
				size_t next = nextOp(mem, size, prev);
				size_t label_length = strlen(mem[prev].operand1);
				if (strncmp(mem[prev].operand1, mem[next].operation, label_length) == 0 && strlen(mem[next].operation) == label_length + 1)
				{
					mem[prev].operation[0] = '\0';
				}
			}
		}
		if (mem[i].operand2[0] == '\0')
			continue;
		is_const_answer answer = is_const(mem[i].operand1);
		if (answer.value)
		{
			if (answer.elem != NULL)
				replace_with_const(mem[i].operand1, answer.elem);
		}
		if (strncmp(mem[i].operation, "mov", 3) == 0)
		{
			if (answer.value == 0)
				set_const(mem[i].operand2, mem[i].operand1, 0);
			else if (answer.elem != NULL)
				set_const(mem[i].operand2, answer.elem->value, answer.value);
			else
				set_const(mem[i].operand2, mem[i].operand1, answer.value);
		}
		if (strncmp(mem[i].operation, "cmp", 3) == 0)
		{
			is_const_answer second = is_const(mem[i].operand2);
			print_const_map();
			if (answer.value && second.value)
			{
				flags_const = 1;
				flags = 0;
				char *op1;
				if (answer.elem != NULL)
					op1 = answer.elem->value;
				else
					op1 = mem[i].operand1;
				char *op2 = second.elem->value;
				int cmp_result = strcmp(op1, op2);
				if (cmp_result < 0)
					flags |= G_FLAG;
				if (cmp_result == 0)
					flags |= E_FLAG;
				if (cmp_result > 0)
					flags |= L_FLAG;
				mem[i].operation[0] = '\0';
			}
			else
				flags_const = 0;
		}
		if (strncmp(mem[i].operation, "add", 3) == 0 || strncmp(mem[i].operation, "sub", 3) == 0 || strncmp(mem[i].operation, "or", 2) == 0 || strncmp(mem[i].operation, "xor", 3) == 0)
		{
			set_const(mem[i].operand2, "", 0);
		}
	}
	for (size_t i = 0; i < size; i++)
	{
		if (mem[i].operation[0] == '\0')
			continue;
		if (mem[i].operation[strlen(mem[i].operation) - 1] == ':')
		{
			if (countLabelOccurances(mem, size, mem[i].operation) == 0)
				mem[i].operation[0] = '\0';
		}
	}
}

void remove_until_label(ASMOP *mem, size_t size, size_t i)
{
	while (i < size)
	{
		if (mem[i].operation[0] != '\0' && mem[i].operation[strlen(mem[i].operation) - 1] == ':' && countLabelOccurances(mem, size, mem[i].operation) >= 1)
		{
			break;
		}
		else
			mem[i].operation[0] = '\0';
		i++;
	}
}

size_t indexOfLabel(ASMOP *mem, size_t i, size_t size, char *label)
{
	size_t label_length = strlen(label);
	while (i < size)
	{
		if (strncmp(mem[i].operation, label, label_length) == 0 && mem[i].operation[label_length] == ':')
			return i;
		i++;
	}
	return -1;
}

size_t countLabelOccurances(ASMOP *mem, size_t size, char *label)
{
	char label_name[64] = {0};
	strncpy(label_name, label, strlen(label) - 1);
	size_t t = 0;
	for (size_t i = 0; i < size; i++)
	{
		if (mem[i].operation[0] != '\0' && strcmp(label_name, mem[i].operand1) == 0)
			t++;
	}
	return t;
}

size_t nextOp(ASMOP *mem, size_t size, size_t i)
{
	i++;
	while (i < size)
	{
		if (mem[i].operation[0] != '\0')
			break;
		i++;
	}
	return i;
}

size_t prevOp(ASMOP *mem, size_t size, size_t i)
{
	i--;
	while (i < size)
	{ // because it is unsigned we cannot test < 0
		if (mem[i].operation[0] != '\0')
			break;
		i--;
	}
	return i;
}

is_const_answer is_const(char *operand)
{
	if (isImmediate(operand))
	{
		is_const_answer ans;
		ans.value = 1;
		ans.elem = NULL;
		return ans;
	}
	const_hashmap_elem *ptr;
	for (ptr = head->next; ptr != NULL; ptr = ptr->next)
	{
		if (strcmp(operand, ptr->key) == 0)
		{
			is_const_answer ans;
			ans.value = ptr->is_const;
			ans.elem = ptr;
			return ans;
		}
	}
	is_const_answer ans;
	ans.value = 0;
	ans.elem = NULL;
	return ans;
}

void print_const_map()
{
	const_hashmap_elem *ptr;
	for (ptr = head->next; ptr != NULL; ptr = ptr->next)
	{
		printf("'%s': '%s' %d\n", ptr->key, ptr->value, ptr->is_const);
	}
}

void replace_with_const(char *operand, const_hashmap_elem *elem)
{
	strcpy(operand, elem->value);
}

void set_const(char *operand, char *value, unsigned is_const_v)
{
	is_const_answer ans = is_const(operand);
	if (ans.elem == NULL)
	{
		const_hashmap_elem *ptr = (const_hashmap_elem *)malloc(sizeof(const_hashmap_elem));
		const_hashmap_elem *next = head->next;
		head->next = ptr;
		ptr->next = next;
		ptr->is_const = is_const_v;
		strcpy(ptr->key, operand);
		strcpy(ptr->value, value);
	}
	else
	{
		const_hashmap_elem *ptr = ans.elem;
		ptr->is_const = is_const_v;
		strcpy(ptr->key, operand);
		strcpy(ptr->value, value);
	}
}

unsigned isImmediate(char *operand)
{
	return operand[0] == '$';
}

void init_const_optimize()
{
	head = (const_hashmap_elem *)malloc(sizeof(const_hashmap_elem));
	head->next = NULL;
	head->is_const = 0;
	head->key[0] = head->value[0] = '\0';
}

void finish_const_optimize()
{
	while (head != NULL)
	{
		const_hashmap_elem *next = head->next;
		free(head);
		head = next;
	}
}

typedef struct needed_hashmap_elem
{
	char key[OPERAND_SIZE];
	unsigned value;
	struct needed_hashmap_elem *next;
} needed_hashmap_elem;

needed_hashmap_elem *needed_head;

void init_usage_optimize()
{
	needed_head = (needed_hashmap_elem *)malloc(sizeof(needed_hashmap_elem));
	needed_head->next = NULL;
	needed_head->key[0] = needed_head->value = 0;
}

void finish_usage_optimize()
{
	while (needed_head != NULL)
	{
		needed_hashmap_elem *next = needed_head->next;
		free(needed_head);
		needed_head = next;
	}
}

typedef struct
{
	unsigned value;
	needed_hashmap_elem *ptr;
} is_needed_answer;

is_needed_answer is_needed(char *operand)
{
	needed_hashmap_elem *ptr;
	for (ptr = needed_head->next; ptr != NULL; ptr = ptr->next)
	{
		if (strcmp(operand, ptr->key) == 0)
		{
			is_needed_answer ans;
			ans.value = ptr->value;
			ans.ptr = ptr;
			return ans;
		}
	}
	is_needed_answer ans;
	ans.value = 0;
	ans.ptr = NULL;
	return ans;
}

void set_needed(char *operand, unsigned needed)
{
	if (isImmediate(operand))
		return;
	is_needed_answer ans = is_needed(operand);
	if (ans.ptr == NULL)
	{
		ans.ptr = (needed_hashmap_elem *)malloc(sizeof(needed_hashmap_elem));
		needed_hashmap_elem *next = needed_head->next;
		needed_head->next = ans.ptr;
		ans.ptr->next = next;
		strcpy(ans.ptr->key, operand);
		ans.ptr->value = needed;
	}
	else
	{
		strcpy(ans.ptr->key, operand);
		ans.ptr->value = needed;
	}
}

void usage_optimize(ASMOP *mem, size_t size)
{
	for (int i = size - 1; i >= 0; i--)
	{
		if (mem[i].operation[0] == '\0')
			continue;
		if (mem[i].operand1[0] == '\0')
			continue;
		if (strncmp(mem[i].operation, "call", 3) == 0)
		{
			if (strcmp(mem[i].operand1, "_print_number@PLT") == 0)
			{
				set_needed("%rax", 1);
				set_needed(STREAM_REGISTER, 1);
			}
			if (strcmp(mem[i].operand1, "_print_string@PLT") == 0)
			{
				set_needed("%rax", 1);
				set_needed("%rbx", 1);
				set_needed(STREAM_REGISTER, 1);
			}
			if (strcmp(mem[i].operand1, "_load_str_to_pool@PLT") == 0)
			{
				set_needed("%rax", 1);
				set_needed("%rbx", 1);
				set_needed("%rdi", 1);
			}
		}
		if (strncmp(mem[i].operation, "mul", 3) == 0 || strncmp(mem[i].operation, "div", 3) == 0)
		{
			set_needed("%rax", 1);
			set_needed("%rdx", 1);
			set_needed(mem[i].operand1, 1);
		}
		if (mem[i].operand2[0] == '\0')
			continue;
		if (strncmp(mem[i].operation, "mov", 3) == 0)
		{
			is_needed_answer ans = is_needed(mem[i].operand2);
			if (!ans.value)
			{
				mem[i].operation[0] = '\0'; // delete instruction
				continue;
			}
			set_needed(mem[i].operand1, 1);
			set_needed(mem[i].operand2, 0);
		}
		if (strncmp(mem[i].operation, "cmp", 3) == 0)
		{
			set_needed(mem[i].operand1, 1);
			set_needed(mem[i].operand2, 1);
		}
		if (strncmp(mem[i].operation, "add", 3) == 0 || strncmp(mem[i].operation, "sub", 3) == 0 || strncmp(mem[i].operation, "and", 3) == 0 || strncmp(mem[i].operation, "or", 2) == 0 || strncmp(mem[i].operation, "xor", 3) == 0)
		{
			set_needed(mem[i].operand1, 1);
			set_needed(mem[i].operand2, 1);
		}
	}
}

void compiler_optimize(ASMOP *mem, size_t size)
{
	init_const_optimize();
	const_optimize(mem, size);
	finish_const_optimize();

	init_usage_optimize();
	usage_optimize(mem, size);
	finish_usage_optimize();
}