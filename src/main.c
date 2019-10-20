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

int main(int argc, char **argv) {
	char *source_file = NULL;
	for (int i = 0; i < argc; i++) {
		if (strcmp("--source", argv[i]) == 0 || strcmp("-s", argv[i]) == 0)
			source_file = argv[++i];
	}
	if (source_file == NULL)
		return NO_FILES_GIVEN;
	FILE *f = fopen(source_file, "r");
	if (f == NULL)
		return COULD_NOT_OPEN_FILE;
	Init_Utils();
	INTERNAL_STATE internal_state;
	Parse(f, &internal_state);
	fclose(f);
	return COMPILATION_SUCCESSFULL;
}
