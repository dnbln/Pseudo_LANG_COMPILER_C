/*
 * errorcodes.h
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_ERRORCODES_H_
#define INCLUDE_ERRORCODES_H_

#include <stdio.h>

#define COMPILATION_SUCCESSFULL 0
#define COULD_NOT_OPEN_FILE 1
#define NO_FILES_GIVEN 2
#define COMPILATION_FAILED 3
#define FUNCTION_NOT_FOUND 4
#define STRING_NOT_ENDED 5
#define OPERATOR_NOT_FOUND 6
#define UNKNOWN_STATEMENT_TYPE 7
#define EXPECTED_TOKEN_NOT_FOUND 8
#define INCOMPATIBLE_TYPES 9
#define VARIABLE_NOT_FOUND 10

typedef struct {
	unsigned int code;
	unsigned int line;
} WARNING;

void load_warning(WARNING);

typedef struct {
	unsigned int code;
	unsigned long long int line;
	char* extra;
	unsigned clear;
} ERROR;

void load_error(ERROR);

void print_warnings(FILE*);
void print_errors(FILE*);

void compiler_perror(FILE*,ERROR);
void compiler_pwarning(FILE*, WARNING);

char* getWarnMessage(int code);
char* getErrorMessage(int code);

void clear_errors();

#endif /* INCLUDE_ERRORCODES_H_ */
