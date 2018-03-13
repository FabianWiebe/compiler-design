#include "stdio.h"
#include "math.h"
#include "stdlib.h"

long _t1, _t11, _t13, _t14, _t15, _t16, _t17, _t2, _t3, _t6, hasChanged, i, itemCount;
double _t10, _t12, _t18, _t4, _t5, _t7, _t8, _t9;
double list[] = {5, 6, 1, 2.4, 2.3, 2.5, 9, 14, 2, 15, 6, 7, 8, 97};

int main(int argc, char **argv)
{
  /* Expand: _t4 := list c[] i */
  i = 8;
  asm(
" movq %[i], %%rbx\n\t"
" lea %[list], %%rax\n\t"
" movsd (%%rax, %%rbx), %%xmm0\n\t"
" movsd %%xmm0, %[_t4]\n\t"

: [_t4] "+x" (_t4),
  [i] "+g" (i),
  [list] "+g" (list)
:
: "rax", "rbx", "rdx", "cc", "xmm0", "xmm1", "rdi", "rsi", "rsp"
  );

  /* Expand:  := print callS _t18 */
  printf("%g\n", _t4);
  /* Expand: i := i ++ i */
}
