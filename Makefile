.PHONY: all COMPILE_PRG clean

CC=gcc
CC_OPTS=-Wall -g3
OBJ_FILES=compile/main.o compile/compiler.o compile/errors.o compile/functions.o compile/parser.o compile/tokens.o compile/utils.o compile/operators.o compile/labels.o compile/types.o

all: bin/binary

bin/binary: $(OBJ_FILES)
	$(CC) -o $@ $(CC_OPTS) $^

compile/main.o: src/main.c
	$(CC) -o $@ -c $(CC_OPTS) $<

compile/%.o: src/%.c include/%.h
	$(CC) -o $@ -c $(CC_OPTS) $<

COMPILE_PRG: bin/binary $(FILE)
	bin/binary $(COMPILER_ARGS)
	as pseudo.s -o pseudo.o
	ld --dynamic-linker /lib64/ld-linux-x86-64.so.2 -L lib -lstd pseudo.o -o pseudo
	rm pseudo.o

RUN_PRG:
	LDLP=$$LD_LIBRARY_PATH;\
	if [[ $$LD_LIBRARY_PATH != "*$$(pwd)/lib/*" ]]; then export LD_LIBRARY_PATH="$$(pwd)/lib/"; fi;\
	./pseudo;\
	LD_LIBRARY_PATH=$$LDLP;

clean:
	rm -f pseudo pseudo.s compile/* bin/binary