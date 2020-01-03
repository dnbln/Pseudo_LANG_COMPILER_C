#include <string.h>
#include <stdio.h>
#include "../include/operators.h"
#include "../include/utils.h"
#include "../include/labels.h"

#define OPERATORS_DEFINED 12

struct
{
    char signature[64];
    OPERATOR *operatorPtr;
} operators_array[OPERATORS_DEFINED];

OPERATOR ADD_NUMBER_NUMBER_OP;
TYPE ADD_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR ADD_VOID_NUMBER_OP;
TYPE ADD_Void_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR MULTIPLY_NUMBER_NUMBER_OP;
TYPE MULTIPLY_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR SUBTRACT_NUMBER_NUMBER_OP;
TYPE SUBTRACT_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR SUBTRACT_VOID_NUMBER_OP;
TYPE SUBTRACT_Void_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR EQ_NUMBER_NUMBER_OP;
TYPE EQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR NEQ_NUMBER_NUMBER_OP;
TYPE NEQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR LEQ_NUMBER_NUMBER_OP;
TYPE LEQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR GEQ_NUMBER_NUMBER_OP;
TYPE GEQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR L_NUMBER_NUMBER_OP;
TYPE L_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR G_NUMBER_NUMBER_OP;
TYPE G_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

OPERATOR ADD_STRING_STRING_OP;
TYPE ADD_String_String(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success);

void operators_init()
{
    strcpy(operators_array[0].signature, "add<Number,Number>");
    ADD_NUMBER_NUMBER_OP.call = &ADD_Number_Number;
    operators_array[0].operatorPtr = &ADD_NUMBER_NUMBER_OP;

    strcpy(operators_array[1].signature, "add<Void,Number>");
    ADD_VOID_NUMBER_OP.call = &ADD_Void_Number;
    operators_array[1].operatorPtr = &ADD_VOID_NUMBER_OP;

    strcpy(operators_array[2].signature, "subtract<Number,Number>");
    SUBTRACT_NUMBER_NUMBER_OP.call = &SUBTRACT_Number_Number;
    operators_array[2].operatorPtr = &SUBTRACT_NUMBER_NUMBER_OP;

    strcpy(operators_array[3].signature, "subtract<Void,Number>");
    SUBTRACT_VOID_NUMBER_OP.call = &SUBTRACT_Void_Number;
    operators_array[3].operatorPtr = &SUBTRACT_VOID_NUMBER_OP;

    strcpy(operators_array[4].signature, "multiply<Number,Number>");
    MULTIPLY_NUMBER_NUMBER_OP.call = &MULTIPLY_Number_Number;
    operators_array[4].operatorPtr = &MULTIPLY_NUMBER_NUMBER_OP;

    strcpy(operators_array[5].signature, "eq<Number,Number>");
    EQ_NUMBER_NUMBER_OP.call = &EQ_Number_Number;
    operators_array[5].operatorPtr = &EQ_NUMBER_NUMBER_OP;

    strcpy(operators_array[6].signature, "neq<Number,Number>");
    NEQ_NUMBER_NUMBER_OP.call = &NEQ_Number_Number;
    operators_array[6].operatorPtr = &NEQ_NUMBER_NUMBER_OP;

    strcpy(operators_array[7].signature, "leq<Number,Number>");
    LEQ_NUMBER_NUMBER_OP.call = &LEQ_Number_Number;
    operators_array[7].operatorPtr = &LEQ_NUMBER_NUMBER_OP;

    strcpy(operators_array[8].signature, "geq<Number,Number>");
    GEQ_NUMBER_NUMBER_OP.call = &GEQ_Number_Number;
    operators_array[8].operatorPtr = &GEQ_NUMBER_NUMBER_OP;

    strcpy(operators_array[9].signature, "l<Number,Number>");
    L_NUMBER_NUMBER_OP.call = &L_Number_Number;
    operators_array[9].operatorPtr = &L_NUMBER_NUMBER_OP;

    strcpy(operators_array[10].signature, "g<Number,Number>");
    G_NUMBER_NUMBER_OP.call = &G_Number_Number;
    operators_array[10].operatorPtr = &G_NUMBER_NUMBER_OP;

    strcpy(operators_array[11].signature, "add<String,String>");
    ADD_STRING_STRING_OP.call = &ADD_String_String;
    operators_array[11].operatorPtr = &ADD_STRING_STRING_OP;
}

OPERATOR *getOperator(const char *signature)
{
    for (size_t i = 0; i < OPERATORS_DEFINED; i++)
    {
        if (strcmp(signature, operators_array[i].signature) == 0)
            return operators_array[i].operatorPtr;
    }
    return NULL;
}

TYPE ADD_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "addq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;
    TYPE t = makeType();
    t.typeid = NUMBER_TYPE;
    return t;
}

TYPE ADD_Void_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    // Shouldn't do anything
    TYPE t = makeType();
    t.typeid = NUMBER_TYPE;
    return t;
}

TYPE SUBTRACT_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "subq");
    strcpy(memory[*ptr].operand1, "%rax");
    strcpy(memory[*ptr].operand2, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand2 + 10);
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    TYPE t = makeType();
    t.typeid = NUMBER_TYPE;
    return t;
}

TYPE SUBTRACT_Void_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "negq");
    strcpy(memory[*ptr].operand1, "%rax");
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    TYPE t = makeType();
    t.typeid = NUMBER_TYPE;
    return t;
}

TYPE MULTIPLY_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%r10");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "mul");
    strcpy(memory[*ptr].operand1, "%r10");
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    TYPE t = makeType();
    t.typeid = NUMBER_TYPE;
    return t;
}

TYPE EQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "cmpq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$1");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    LABEL l = make_label();

    strcpy(memory[*ptr].operation, "je");
    strcpy(memory[*ptr].operand1, l.label_name);
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$0");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, l.label_name);
    strcat(memory[*ptr].operation, ":");
    memory[*ptr].operand1[0] = '\0';
    (*ptr)++;

    TYPE type = makeType();
    type.typeid = NUMBER_TYPE;
    return type;
}

TYPE NEQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "cmpq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$1");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    LABEL l = make_label();

    strcpy(memory[*ptr].operation, "jne");
    strcpy(memory[*ptr].operand1, l.label_name);
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$0");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, l.label_name);
    strcat(memory[*ptr].operation, ":");
    memory[*ptr].operand1[0] = '\0';
    (*ptr)++;

    TYPE type = makeType();
    type.typeid = NUMBER_TYPE;
    return type;
}

TYPE LEQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "cmpq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$1");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    LABEL l = make_label();

    strcpy(memory[*ptr].operation, "jge");
    strcpy(memory[*ptr].operand1, l.label_name);
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$0");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, l.label_name);
    strcat(memory[*ptr].operation, ":");
    memory[*ptr].operand1[0] = '\0';
    (*ptr)++;

    TYPE type = makeType();
    type.typeid = NUMBER_TYPE;
    return type;
}

TYPE GEQ_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "cmpq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$1");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    LABEL l = make_label();

    strcpy(memory[*ptr].operation, "jle");
    strcpy(memory[*ptr].operand1, l.label_name);
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$0");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, l.label_name);
    strcat(memory[*ptr].operation, ":");
    memory[*ptr].operand1[0] = '\0';
    (*ptr)++;

    TYPE type = makeType();
    type.typeid = NUMBER_TYPE;
    return type;
}

TYPE L_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "cmpq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$1");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    LABEL l = make_label();

    strcpy(memory[*ptr].operation, "jg");
    strcpy(memory[*ptr].operand1, l.label_name);
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$0");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, l.label_name);
    strcat(memory[*ptr].operation, ":");
    memory[*ptr].operand1[0] = '\0';
    (*ptr)++;

    TYPE type = makeType();
    type.typeid = NUMBER_TYPE;
    return type;
}

TYPE G_Number_Number(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    strcpy(memory[*ptr].operation, "cmpq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$1");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    LABEL l = make_label();

    strcpy(memory[*ptr].operation, "jl");
    strcpy(memory[*ptr].operand1, l.label_name);
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "$0");
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, l.label_name);
    strcat(memory[*ptr].operation, ":");
    memory[*ptr].operand1[0] = '\0';
    (*ptr)++;

    TYPE type = makeType();
    type.typeid = NUMBER_TYPE;
    return type;
}

TYPE ADD_String_String(CACHE_PTR a, TYPE b, ASMOP *memory, size_t *ptr, int *success)
{
    // allocate memory for second string
    strcpy(memory[*ptr].operation, "subq");
    strcpy(memory[*ptr].operand1, "%rbx");
    strcpy(memory[*ptr].operand2, "%rsp");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "%rsp");
    strcpy(memory[*ptr].operand2, "%rdi");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    // load second string
    strcpy(memory[*ptr].operation, "callq");
    strcpy(memory[*ptr].operand1, "_load_str_to_stack@PLT");
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    // save the length of second string
    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "%rbx");
    strcpy(memory[*ptr].operand2, "%rdx");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    // load first string data from CACHE_MEM
    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rax");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "CACHE_MEM+");
    num_to_str(a->cache_index+8, memory[*ptr].operand1 + 10);
    strcpy(memory[*ptr].operand2, "%rbx");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    // allocate memory for first string
    strcpy(memory[*ptr].operation, "subq");
    strcpy(memory[*ptr].operand1, "%rbx");
    strcpy(memory[*ptr].operand2, "%rsp");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    strcpy(memory[*ptr].operation, "movq");
    strcpy(memory[*ptr].operand1, "%rsp");
    strcpy(memory[*ptr].operand2, "%rdi");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    // load first string
    strcpy(memory[*ptr].operation, "callq");
    strcpy(memory[*ptr].operand1, "_load_str_to_stack@PLT");
    memory[*ptr].operand2[0] = '\0';
    (*ptr)++;

    // load total length
    strcpy(memory[*ptr].operation, "addq");
    strcpy(memory[*ptr].operand1, "%rdx");
    strcpy(memory[*ptr].operand2, "%rbx");
    memory[*ptr].operand3[0] = '\0';
    (*ptr)++;

    TYPE t = makeType();
    t.typeid = STRING_TYPE;
    return t;
}