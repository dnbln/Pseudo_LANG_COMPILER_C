/*
 * tokens.h
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_TOKENS_H_
#define INCLUDE_TOKENS_H_

#define FILE_END_TOKEN 0U // FILE END
#define BEGIN_TOKEN 1U // BEGIN
#define END_TOKEN 2U // END
#define DECLARE_TOKEN 3U // DECLARE
#define TYPE_TOKEN 4U // TYPE
#define ARITH_OP_TOKEN 5U // ARITHMETIC OPERATORS
#define BIT_OP_TOKEN 6U // BITWISE OPERATORS
#define P_OPEN_TOKEN 7U // PARENTHESIS OPEN
#define P_CLOSED_TOKEN 8U // PARENTHESIS CLOSED
#define NEW_LINE_TOKEN 9U // NEW LINE
#define NUMBER_TOKEN 10U // NUMBER
#define IDENTIFIER_TOKEN 11U // IDENTIFIER
#define ASSIGNMENT_TOKEN 12U // ASSIGNMENT
#define FROM_TO_TOKEN 13U // FROM/TO
#define CONSOLE_TOKEN 14U // CONSOLE
#define FILE_TOKEN 15U // FILE
#define COMMA_TOKEN 16U

// ARITHMETIC OPERATORS
#define ADD 0U
#define SUBTRACT 1U
#define MULTIPLY 2U
#define DIVIDE 3U

typedef struct {
	// the type of the token
	unsigned type;
	// data here, if any
	// this data should be processed according to the type
	void *data;
} Token;

Token Make_nodata_Token(int type); // makes a token that does not contain any data
Token Make_data_Token(int type, void *data); // makes a token that does contain data

#endif /* INCLUDE_TOKENS_H_ */
