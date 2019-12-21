/*
 * main.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#include <string.h>
#include <stdio.h>
#include "../include/parser.h"
#include "../include/errors.h"
#include "../include/utils.h"
#include "../include/functions.h"
#include "../include/compiler.h"

#define DEBUGGING

COMPILER_INTERNAL internal_state;

int main(int argc, char **argv) {
#ifndef DEBUGGING
	char *source_file = NULL;
	char *assembly_file = "pseudo.s";
	for (int i = 0; i < argc; i++) {
		if (strcmp("--source", argv[i]) == 0 || strcmp("-s", argv[i]) == 0)
			source_file = argv[++i];
		if (strcmp("--assembly", argv[i]) == 0 || strcmp("-a", argv[i]) == 0)
			assembly_file = argv[++i];
	}
	if (source_file == NULL){
		fprintf(stderr, "error: no source files given");
		return NO_FILES_GIVEN;
	}
#else
	char *source_file = "test.psl";
	char *assembly_file = "test.s";
#endif
	FILE *f = fopen(source_file, "r");
	if (f == NULL){
		fprintf(stderr, "error: could not open file '%s'.\n", source_file);
		perror("Reason");
		return COULD_NOT_OPEN_FILE;
	}
	Init_Utils();
	Init_Functions(&compiler_varname, &compiler_value_instructions);
	int success = 1;
	Parse(f, &internal_state, &success);
	fclose(f);
	if (success)
		Compile(&internal_state, &success);
	if (success) {
		f = fopen(assembly_file, "w");
		print_warnings(stdout);
		Write(f, &internal_state);
		return COMPILATION_SUCCESSFULL;
	} else {
		print_warnings(stdout);
		print_errors(stderr);
		return COMPILATION_FAILED;
	}
}
