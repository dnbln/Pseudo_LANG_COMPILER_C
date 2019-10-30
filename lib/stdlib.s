
.equ SYS_READ, 0
.equ SYS_WRITE, 1
.equ SYS_EXIT, 60

_print_char:
	movq %rax, %rsi
	movq $1, %rax
	movq %r15, %rdi
	movq $1, %rdx
	syscall
	retq

_NB:
	mov $-1, %rax
	jmp _error

.globl _print_number
_print_number:
	pushq %rax
	pushq %rdx
	pushq %rbx
	pushq %r8
	movq $10, %r8
	movq $0, %rdx
	div %r8
	test %rax, %rax
	je _print_number.l1
	callq _print_number@PLT
_print_number.l1:
	movq digits@GOTPCREL(%rip), %rax
	addq %rdx, %rax
	callq _print_char@PLT
	orq %rax, %rax
	js _error
	popq %r8
	popq %rbx
	popq %rdx
	popq %rax
	retq

.globl _pseudo_lib_exit
_pseudo_lib_exit:
	movq %rax, %rdi
	movq $SYS_EXIT, %rax
	syscall

_error:
	neg %rax
	movq %rax, %rdi
	movq $60, %rax
	syscall

.section .rodata
.type digits, @object
.align 16
digits:
	.byte 48
	.byte 49
	.byte 50
	.byte 51
	.byte 52
	.byte 53
	.byte 54
	.byte 55
	.byte 56
	.byte 57
