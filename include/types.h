/*
 * types.h
 *
 *  Created on: Oct 27, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#define VOID_TYPE 0ULL
#define NUMBER_TYPE 1ULL
#define STRING_TYPE 2ULL
#define CHAR_TYPE 3ULL
#define ERROR_TYPE 4ULL

typedef struct {
	unsigned long long int typeid;
	void *saved;
	unsigned string_saved_to_pool;
} TYPE;

TYPE makeType();

#endif /* INCLUDE_TYPES_H_ */
