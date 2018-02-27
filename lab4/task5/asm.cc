#include <stdio.h>
int main(int argc, char **argv)
{
  printf("Alpha\n");
  asm("xorl %eax, %eax\n\t");
  printf("omega\n");
}

