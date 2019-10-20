/*
 * tokens.c
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#include <stdlib.h>
#include "../include/tokens.h"

Token Make_nodata_Token(int type) {
	Token t;
	t.type = type;
	t.data = NULL;
	return t;
}

Token Make_data_Token(int type, void *data) {
	Token t;
	t.type = type;
	t.data = data;
	return t;
}
