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

typedef struct {
	unsigned int code;
	unsigned int line;
} WARNING;

void load_warning(WARNING);

typedef struct {
	unsigned int code;
	unsigned int line;
} ERROR;

void load_error(ERROR);

void print_warnings(FILE*);
void print_errors(FILE*);

void compiler_perror(FILE*,ERROR);
void compiler_pwarining(FILE*, WARNING);

#endif /* INCLUDE_ERRORCODES_H_ */
