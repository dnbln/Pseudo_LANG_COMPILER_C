/*
 * statements.h
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_STATEMENTS_H_
#define INCLUDE_STATEMENTS_H_

#include "tokens.h"

#define NO_STATEMENT 0U
#define FUNCTION_CALL_STATEMENT 1U
#define COMPOSITE_STATEMENT 2U
#define ASSIGNMENT_STATEMENT 3U
#define CONDITIONAL_STATEMENT 4U

#define MAX_NESTED_STATEMENTS

typedef struct {
	int type;
	int line;
	void *data; // some data about this statement
} Statement;

typedef struct {
	unsigned inside_statements_count;
	Statement *statements[128];
} Composite_Statement;

typedef struct {
	Token* ptr;
	int tn;
} Condition;

typedef struct {
	Condition condition;
	Statement onTrue;
	Statement onFalse;
	unsigned has_false;
} Conditional_Statement;

#endif /* INCLUDE_STATEMENTS_H_ */
