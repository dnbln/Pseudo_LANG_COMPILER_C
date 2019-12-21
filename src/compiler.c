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

void Compile(COMPILER_INTERNAL *internal_state, int *success)
{
	internal_state->asmop_memptr = 0;
	*success = 1;
	for (int i = 0; *success == 1 && i < internal_state->STATEMENT_POINTER; i++)
		Compile_Statement(internal_state, internal_state->statements[i],
						  success);
}

void Compile_Statement(COMPILER_INTERNAL *internal_state, Statement s,
					   int *success)
{
	const int type = s.type;
	*success = 1;
	if (type == FUNCTION_CALL_STATEMENT)
	{
		long long *statement_data = s.data;
		ASM_FUNC_GENERATE *generate_assembly =
			(ASM_FUNC_GENERATE *)statement_data[0];
		Token *tokens = (Token *)statement_data[1];
		generate_assembly(tokens, internal_state->asmop_mem,
						  &(internal_state->asmop_memptr), success);
	}
	else if(type == COMPOSITE_STATEMENT){
		Composite_Statement* data = (Composite_Statement*) s.data;
		for(int i = 0; i < data->inside_statements_count; i++)
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
		else if (t->type == STRING_TYPE)
		{
			char* name = compiler_loadString((char*) t->data);
			strcpy(memory[*ptr].operation, "movq");
			memory[*ptr].operand1[0] = '$';
			strcpy(memory[*ptr].operand1+1, name);
			strcpy(memory[*ptr].operand2, "%rax");
			memory[*ptr].operand3[0] = '\0';
			(*ptr)++;
			type.typeid = STRING_TYPE;
			return type;
		}
	}
	if (n == 3)
	{
		return callOperator(t[0], (long long int)t[1].data, t[2], memory, ptr, success);
	}
	return type;
}

OPERATOR find_operator(int op, int type1, int type2)
{
	OPERATOR opr;
	return opr;
}

TYPE callOperator(Token a, int op, Token b, ASMOP *memory, int *ptr, int *success)
{
	OPERATOR opr = find_operator(op, a.type, b.type);
	return opr.call(a, b, memory, ptr, success);
}

char *compiler_varname(const char *identifier_name)
{
	return NULL;
}

char *compiler_loadString(const char *stringValue){
	int index = compiler_lookUpString(stringValue);
	if(index != -1){
		return compiler_stringName(index);
	}else{
		return compiler_addString(stringValue);
	}
}

STRINGS_LIST string_list;

int compiler_lookUpString(const char *stringVal){
	for(size_t i = 0; i < string_list.string_count; i++){
		if(strcmp(string_list.strings[i], stringVal) == 0)
		{
			return i;
		}
	}
	return -1;
}

char* compiler_stringName(int index){
	return string_list.strName[index];
}

char* compiler_addString(const char *stringVal){
	strcpy(string_list.strings[string_list.string_count], stringVal);
	strcpy(string_list.strName[string_list.string_count], "str");
	num_to_str(string_list.string_count, string_list.strName[string_list.string_count] + 3);
	return string_list.strName[string_list.string_count++];
}

void compiler_init(){
	string_list.string_count = 0;
}


void compiler_writeDataAndVariables(FILE* f){
	if(string_list.string_count){
		fprintf(f, ".section .rodata\n");
		for(size_t i = 0; i < string_list.string_count; i++){
			fprintf(f, "%s:\n\t.asciz \"%s\"\n\t.byte 0\n\t.byte 0\n", string_list.strName[i], string_list.strings[i]);
		}
	}
}