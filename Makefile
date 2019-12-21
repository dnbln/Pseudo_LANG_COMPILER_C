.PHONY: all COMPILE_PRG

CC=gcc
CC_OPTS=-Wall
OBJ_FILES=compile/main.o compile/compiler.o compile/errors.o compile/functions.o compile/parser.o compile/tokens.o compile/utils.o

all: bin/binary

bin/binary: $(OBJ_FILES)
	$(CC) -o $@ $(CC_OPTS) $^

compile/%.o: src/%.c
	$(CC) -o $@ -c $(CC_OPTS) $<

COMPILE_PRG: bin/binary $(FILE)
	bin/binary $(FILE)
	as test.s -o test.o
	ld --dynamic-linker /lib64/ld-linux-x86-64.so.2 -L lib -lstd test.o -o test
	rm test.o