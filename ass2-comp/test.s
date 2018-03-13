.data # gcc test.s -no-pie
z:       .double 123.125
str:     .string "value: %u %f\n"
.text
.globl main
main: 
	movq   $2,  %rax # Arg 2
	movq   $2,  %rcx # Arg 2
	movq   $8,  %rbx # Arg 2
loop:
	cmp $1, %rbx
	jbe continue
	dec %rbx
	mul %rcx
	jmp loop
continue:
      movq   %rax,  %rsi # Arg 2
	  lea    str, %rdi # Arg 1
      movq   $1,  %rax # Vec args
      subq   $8,  %rsp # Alignment
      movsd  z,   %xmm0
      call   printf
      addq   $8, %rsp  # Alignment
      ret
