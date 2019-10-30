/*
 * utils.h
 *
 *  Created on: Oct 20, 2019
 *      Author: dinu
 */

#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

typedef struct {
	int (*belongs_to_class)(const char c);
} CHARACTER_CLASS;

CHARACTER_CLASS DIGIT_CLASS;
CHARACTER_CLASS IDENTIFIER_FIRST_CHARS;
CHARACTER_CLASS IDENTIFIER_OTHER_CHARS;

// Returns 1 if a the first characters in a are b, 0 otherwise
int starts_with(const char *a, const char *b);

// Returns 1 if the first character in a belongs to the character class b, 0 otherwise
int starts_class(const char *a, CHARACTER_CLASS b);

// Returns 1 if a is ' ' or '\t'
int is_whitespace(const char a);

// Returns 1 if a is a digit
int is_digit(const char a);

void Init_Utils();

char* num_to_str(long long, char*);

#endif /* INCLUDE_UTILS_H_ */
