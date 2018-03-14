.data # gcc test.s -no-pie
z:      .double 12.4 
		.double 123
l:		.quad 27
str:     .string "value: %u %f %ld %ld\n"
array:	.quad 3
		.quad 4
		.quad 5
.text
.globl main
test:
	movq   $3,  %rax # Arg 2
	movq   %rax,  %rcx # Arg 2
	movq   $8,  %r8 # Arg 2
loop:
	cmp $1, %r8
	jbe continue
	dec %r8
	mul %rcx
	jmp loop
continue:
	ret
main: 
	call test
	#
	movq	l, %rdx
	lea array, %r8
	movq $8, %r9
	movq (%r8, %r9), %rcx
      movq   %rax,  %rsi # Arg 2
	  lea    str, %rdi # Arg 1
      movq   $1,  %rax # Vec args
      subq   $8,  %rsp # Alignment
      lea  z,   %r11
		movq $8, %r10
		movsd (%r11, %r10), %xmm0 
      call   printf
      addq   $8, %rsp  # Alignment
      ret
