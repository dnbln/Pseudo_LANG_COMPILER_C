/*
 * main.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#include <string.h>
#include <stdio.h>
#include "../include/parser.h"
#include "../include/errorcodes.h"
#include "../include/utils.h"

#define DEBUGGING

COMPILER_INTERNAL internal_state;

int main(int argc, char **argv) {
#ifndef DEBUGGING
	char *source_file = NULL;
	for (int i = 0; i < argc; i++) {
		if (strcmp("--source", argv[i]) == 0 || strcmp("-s", argv[i]) == 0)
			source_file = argv[++i];
	}
	if (source_file == NULL)
		return NO_FILES_GIVEN;
#else
	char *source_file = "test.psl";
#endif
	FILE *f = fopen(source_file, "r");
	if (f == NULL)
		return COULD_NOT_OPEN_FILE;
	Init_Utils();
	Parse(f, &internal_state);
	fclose(f);
	return COMPILATION_SUCCESSFULL;
}
