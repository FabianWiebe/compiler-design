.data
x:    .quad 0
y:    .word 0
z:    .double 123.125
name: .string "blah blah\n"
      .lcomm buf, 32
.text
.globl main
main: movq x, %rax
      movw y, %bx
      lea  name, %rdi
      movq $12, %rcx
      movb %al, name(%rcx)
