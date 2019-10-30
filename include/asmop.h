/*
 * asmop.h
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_ASMOP_H_
#define INCLUDE_ASMOP_H_

#define OPERATION_SIZE 15
#define OPERAND_SIZE 100

typedef struct {
	char operation[OPERATION_SIZE];
	char operand1[OPERAND_SIZE];
	char operand2[OPERAND_SIZE];
	char operand3[OPERAND_SIZE];
} ASMOP;

#endif /* INCLUDE_ASMOP_H_ */
