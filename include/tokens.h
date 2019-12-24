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
#define LOGIC_OP_TOKEN 7U // LOGIC OPERATORS
#define EQUALITY_OP_TOKEN 8U // EQUALITY OPERATORS('==', '<', '>', '<=', ...)

#define P_OPEN_TOKEN 9U // PARENTHESIS OPEN
#define P_CLOSED_TOKEN 10U // PARENTHESIS CLOSED
#define NEW_LINE_TOKEN 11U // NEW LINE
#define NUMBER_TOKEN 12U // NUMBER
#define STRING_TOKEN 13U // STRING
#define IDENTIFIER_TOKEN 14U // IDENTIFIER
#define ASSIGNMENT_TOKEN 15U // ASSIGNMENT
#define FROM_TO_TOKEN 16U // FROM/TO
#define CONSOLE_TOKEN 17U // CONSOLE
#define FILE_TOKEN 18U // FILE
#define COMMA_TOKEN 19U // COMMA(',')

// ARITHMETIC OPERATORS
#define ADD 0ULL
#define SUBTRACT 1ULL
#define MULTIPLY 2ULL
#define DIVIDE 3ULL

// LOGIC OPERATORS
#define AND 0ULL
#define OR 1ULL
#define XOR 2ULL
#define NOT 3ULL

// EQUALITY OPERATORS
#define EQ 0ULL // ==
#define LEQ 1ULL // <=
#define GEQ 2ULL // >=
#define L 3ULL // <
#define G 4ULL // >
#define NEQ 5ULL // !=

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
