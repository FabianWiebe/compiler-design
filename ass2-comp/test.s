.data # gcc test.s -no-pie
z:       .double 123.125
str:     .string "value: %u %f\n"
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
      movq   %rax,  %rsi # Arg 2
	  lea    str, %rdi # Arg 1
      movq   $1,  %rax # Vec args
      subq   $8,  %rsp # Alignment
      movsd  z,   %xmm0
      call   printf
      addq   $8, %rsp  # Alignment
      ret
