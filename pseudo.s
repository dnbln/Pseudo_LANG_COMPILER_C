.globl _start
_start:
	callq _pseudo_lib_init@PLT
	movq $1048576, STRINGS_POOL_PTR
	movq $1, %rax
	addq $0, %rax
	movq %rax, var0
	movq var0, %rax
	movq %rax, CACHE_MEM+0
	movq $1, %rax
	addq CACHE_MEM+0, %rax
	movq %rax, var2
	movq $1, %r15
	movq var0, %rax
	callq _print_number@PLT
	movq $str0, %rax
	movq $1, %rbx
	callq _print_string@PLT
	movq $1, %rax
	callq _print_number@PLT
	movq $str0, %rax
	movq $1, %rbx
	callq _print_string@PLT
	movq var2, %rax
	callq _print_number@PLT
	movq $str1, %rax
	movq $1, %rbx
	callq _print_string@PLT
	movq $0, %rax
	callq _pseudo_lib_exit@PLT
.section .rodata
str0:
	.ascii " "
str1:
	.ascii "\n"
.section .bss
	.lcomm CACHE_MEM, 524288
	.lcomm STRINGS_POOL, 1048576
	.lcomm STRINGS_POOL_PTR, 8
	.lcomm var0, 8
	.lcomm var1, 8
	.lcomm var2, 8
