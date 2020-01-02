#include "../include/labels.h"
#include "../include/utils.h"
#include <string.h>

size_t label_ind;

void labels_init()
{
    label_ind = 0;
}

LABEL make_label()
{
    LABEL l;
    strcpy(l.label_name, ".l");
    num_to_str(label_ind, l.label_name + 2);
    label_ind++;
    return l;
}