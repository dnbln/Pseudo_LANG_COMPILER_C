/*
 * errorcodes.c
 *
 *  Created on: Oct 30, 2019
 *      Author: dinu
 */

#include "../include/errorcodes.h"

#define MAX_ERRORS 10000
#define MAX_WARNINGS 10000

size_t error_count = 0,
			warning_count = 0;
ERROR errors[MAX_ERRORS];
WARNING warnings[MAX_WARNINGS];

void print_errors(FILE *f) {
	fprintf(f, "%ld errors:", error_count);
	for(size_t i = 0; i < error_count; i++)
		compiler_perror(f, errors[i]);
}

void print_warnings(FILE *f) {
	fprintf(f, "%ld warnings:", warning_count);
	for(size_t i = 0; i < warning_count; i++)
		compiler_pwarning(f, warnings[i]);
}

void compiler_perror(FILE* f, ERROR e){

}

void compiler_pwarning(FILE* f, WARNING w){

}
