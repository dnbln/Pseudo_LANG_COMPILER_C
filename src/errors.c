/*
 * errorcodes.c
 *
 *  Created on: Oct 30, 2019
 *      Author: dinu
 */

#include "../include/errors.h"

#define MAX_ERRORS 10000
#define MAX_WARNINGS 10000

size_t error_count = 0,
	   warning_count = 0;
ERROR errors[MAX_ERRORS];
WARNING warnings[MAX_WARNINGS];

char WarnMessages[][64] = {"", "", "", ""};
char ErrorMessages[][64] = {"", "", "", "", "FUNCTION_NOT_FOUND", "STRING_NOT_ENDED"};

void load_error(ERROR err)
{
	errors[error_count++] = err;
}

void load_warning(WARNING warn)
{
	warnings[warning_count++] = warn;
}

void print_errors(FILE *f)
{
	if (error_count == 0)
		return;
	fprintf(f, "%ld errors:\n", error_count);
	for (size_t i = 0; i < error_count; i++)
		compiler_perror(f, errors[i]);
}

void print_warnings(FILE *f)
{
	if (warning_count == 0)
		return;
	fprintf(f, "%ld warnings:\n", warning_count);
	for (size_t i = 0; i < warning_count; i++)
		compiler_pwarning(f, warnings[i]);
}

void compiler_perror(FILE *f, ERROR e)
{
	if (e.extra == NULL)
		fprintf(f, "Error: %d\n%s on line %lld\n", e.code, getErrorMessage(e.code), e.line);
	else
		fprintf(f, "Error: %d\n%s:'%s' on line %lld\n", e.code, getErrorMessage(e.code), e.extra, e.line);
}

void compiler_pwarning(FILE *f, WARNING w)
{
	fprintf(f, "Warning: %d\n%s\n", w.code, getWarnMessage(w.code));
}

char *getWarnMessage(int code)
{
	return WarnMessages[code];
}

char *getErrorMessage(int code)
{
	return ErrorMessages[code];
}
