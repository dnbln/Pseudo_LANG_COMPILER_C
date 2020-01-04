#include "../include/types.h"
#include <stddef.h>

TYPE makeType(){
    TYPE t;
    t.typeid = VOID_TYPE;
    t.saved = NULL;
    t.string_saved_to_pool = -1;
    return t;
}