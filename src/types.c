#include "../include/types.h"
#include <stddef.h>

TYPE makeType(){
    TYPE t;
    t.typeid = VOID_TYPE;
    t.saved = NULL;
    return t;
}