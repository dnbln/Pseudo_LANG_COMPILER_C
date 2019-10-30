all: libstd.so

libstd.so: stdlib.o
	gcc -shared $< -o $@
	
%.o: %.s
	as $< -o $@