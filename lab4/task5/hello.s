.data
z:       .double 123.125
str:     .string "value: %u %f\n"
.text
.globl main
main: lea    str, %rdi # Arg 1
      movq   $7,  %rsi # Arg 2
      movq   $1,  %rax # Vec args
      subq   $8,  %rsp # Alignment
      movsd  z,   %xmm0
      call   printf
      addq   $8, %rsp  # Alignment
      ret
