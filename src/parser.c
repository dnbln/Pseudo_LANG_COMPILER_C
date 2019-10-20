/*
 * parser.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#include "../include/parser.h"
#include "../include/utils.h"

void Parse(FILE *f, INTERNAL_STATE *internal_state) {
	internal_state->TOKEN_PTR = 0;
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
}

void GetNextToken(INTERNAL_STATE *state, Token *token) {
	char *ptr = state->FILE_PTR;
	while (is_whitespace(*ptr))
		ptr++;
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
