#ifndef VARS_H
#define VARS_H

#include <stdio.h>

#include "types.h"

char *varname(const char *identifier_name);

unsigned long long getVarType(const char *name);

void vars_print(FILE *f);

void addVar(const char* name, unsigned long long type);

#endif