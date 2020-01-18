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
#include "../include/opts.h"
#include "../include/vars.h"

#define DEBUGGING

COMPILER_INTERNAL internal_state;
OPTS opts = {1048576, 1048576};

OPTS *getOpts_func()
{
	return &opts;
}

int main(int argc, char **argv)
{
#ifndef DEBUGGING
	char *source_file = NULL;
#else
	char *source_file = "test.psl";
#endif

	char *assembly_file = "pseudo.s";
	for (int i = 0; i < argc; i++)
	{
		if (strcmp("--source", argv[i]) == 0 || strcmp("-s", argv[i]) == 0)
			source_file = argv[++i];
		if (strcmp("--assembly", argv[i]) == 0 || strcmp("-a", argv[i]) == 0)
			assembly_file = argv[++i];
		if (strcmp("--strings-pool-size", argv[i]) == 0 || strcmp("-p", argv[i]) == 0)
			opts.stringPoolSize = size_str_to_num(argv[++i]);
		if (strcmp("--strings-var-mem-size", argv[i]) == 0 || strcmp("--svarmem", argv[i]) == 0)
			opts.strings_var_mem_size = size_str_to_num(argv[++i]);
	}

#ifndef DEBUGGING
	if (source_file == NULL)
	{
		fprintf(stderr, "error: no source files given");
		return NO_FILES_GIVEN;
	}
#endif

	FILE *f = fopen(source_file, "r");
	if (f == NULL)
	{
		fprintf(stderr, "error: could not open file '%s'.\n", source_file);
		perror("Reason");
		return COULD_NOT_OPEN_FILE;
	}
	Init_Utils();
	Init_Functions(&compiler_getLine, &compiler_value_instructions, &clean_str_pool_func);
	int success = 1;
	Parser_init(&getOpts_func);
	Parse(f, &internal_state, &success);
	fclose(f);
	compiler_init(&getOpts_func);
	if (success)
	{
		Compile(&internal_state, &success);
		cache_delete();
		compiler_optimize(internal_state.asmop_mem, internal_state.asmop_memptr);
	}
	if (success)
	{
		f = fopen(assembly_file, "w");
		if (f == NULL)
		{
			fprintf(stderr, "error: could not open file '%s'.\n", assembly_file);
			perror("Reason");
			return COULD_NOT_OPEN_FILE;
		}
		print_warnings(stdout);
		Write(f, &internal_state);
		return COMPILATION_SUCCESSFULL;
	}
	else
	{
		print_warnings(stdout);
		print_errors(stderr);
		clear_errors();
		return COMPILATION_FAILED;
	}
}
