#include "../include/vars.h"

#include <string.h>

#include "../include/types.h"
#include "../include/utils.h"

typedef struct
{
    unsigned long long type;
    char identifierName[256];
    char name[64];
} VAR;
VAR allVars[512];
size_t varCount;

char *varname(const char *identifier_name)
{
    for (size_t i = 0; i < varCount; i++)
    {
        if (strcmp(allVars[i].identifierName, identifier_name) == 0)
        {
            return allVars[i].name;
        }
    }
    return NULL;
}

unsigned long long getVarType(const char *name)
{
    for (size_t i = 0; i < varCount; i++)
    {
        if (strcmp(allVars[i].name, name) == 0)
        {
            return allVars[i].type;
        }
    }
    return VOID_TYPE;
}

void vars_print(FILE* f){
    for(size_t i = 0; i < varCount; i++){
        size_t varSize = 0;
        switch (allVars[i].type)
        {
        case NUMBER_TYPE:
            varSize = 8;
            break;
        
        case STRING_TYPE:
            varSize = 16;
            break;
        
        default:
            break;
        }
        fprintf(f, "\t.lcomm %s, %ld\n", allVars[i].name, varSize);
    }
}

void addVar(const char* name, unsigned long long type){
    strcpy(allVars[varCount].identifierName, name);
    strcpy(allVars[varCount].name, "var");
    num_to_str(varCount, allVars[varCount].name+3);
    allVars[varCount].type = type;
    varCount++;
}