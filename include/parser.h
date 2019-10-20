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


#define TEXT_READ_SIZE 65536
#define TOKENS_MAX 32768
#define DATA_FOR_TOKENS 65536

typedef struct {
	int TOKEN_PTR;
	char *FILE_PTR;
	unsigned CHARACTERS_READ;
	char *DATA_PTR;

	char text_read[TEXT_READ_SIZE];
	Token tokens[TOKENS_MAX];
	char data_for_tokens[DATA_FOR_TOKENS];
} INTERNAL_STATE;

void Parse(FILE*, INTERNAL_STATE*);

void GetNextToken(INTERNAL_STATE*, Token*);

#endif /* INCLUDE_PARSER_H_ */
