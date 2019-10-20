/*
 * functions.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */
#include "../include/functions.h"
#include <stdlib.h>
#include <string.h>

#define PREDEFINED_FUNCTIONS 0

Function declared_functions[PREDEFINED_FUNCTIONS];
Function NO_FUNCTION;

void GetFunction(const char *name, Function *f, int *found) {
	for (int i = 0; i < PREDEFINED_FUNCTIONS; i++) {
		if (strcmp(name, declared_functions[i].name) == 0) {
			*f = declared_functions[i];
			*found = 1;
			return;
		}
	}
	*found = 0;
}
