.globl _start
_start:
	movw $1, %r15w
	movq $10, %rax
	callq _print_number@PLT
	movq $0, %rax
	callq _pseudo_lib_exit@PLT
