.PHONY: all COMPILE_PRG clean

CC=gcc
CC_OPTS=-Wall -g3
OBJ_FILES=compile/main.o compile/compiler.o compile/errors.o compile/functions.o compile/parser.o compile/tokens.o compile/utils.o compile/operators.o

all: bin/binary

bin/binary: $(OBJ_FILES)
	$(CC) -o $@ $(CC_OPTS) $^

compile/main.o: src/main.c
	$(CC) -o $@ -c $(CC_OPTS) $<

compile/%.o: src/%.c include/%.h
	$(CC) -o $@ -c $(CC_OPTS) $<

COMPILE_PRG: bin/binary $(FILE)
	bin/binary $(FILE)
	as test.s -o test.o
	ld --dynamic-linker /lib64/ld-linux-x86-64.so.2 -L lib -lstd test.o -o test
	rm test.o

clean:
	rm -f test test.s compile/* bin/binary