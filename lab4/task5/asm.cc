#include <iostream>

int main(int argc, char **argv)
{
  long _t0, _t1, _t2, _t3, x, y;
  asm(
/* BBlock @ blk0 */
"blk0:\n\t"
/* Expand: x := 0 c 0 */
" movq $0, %%rax\n\t"
" movq $0, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[x]\n\t"

/* Expand: y := 0 c 0 */
" movq $0, %%rax\n\t"
" movq $0, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[y]\n\t"

/* Expand: _t0 := x + 1 */
" movq %[x], %%rax\n\t"
" movq $1, %%rbx\n\t"
" addq %%rbx, %%rax\n\t"
" movq %%rax, %[_t0]\n\t"

/* Expand: x := _t0 c _t0 */
" movq %[_t0], %%rax\n\t"
" movq %[_t0], %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[x]\n\t"

/* Expand: _t1 := y + 1 */
" movq %[y], %%rax\n\t"
" movq $1, %%rbx\n\t"
" addq %%rbx, %%rax\n\t"
" movq %%rax, %[_t1]\n\t"

/* Expand: y := _t1 c _t1 */
" movq %[_t1], %%rax\n\t"
" movq %[_t1], %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[y]\n\t"

/* Expand: _t2 := x = 0 */
" movq %[x], %%rax\n\t"
" movq $0, %%rbx\n\t"
" subq %%rbx, %%rax\n\t"
" movq %%rax, %[_t2]\n\t"

/* True:    blk1 */
" jz blk1\n\t"
/* False:   blk5 */
" jmp blk5\n\t"

/* BBlock @ blk1 */
"blk1:\n\t"
/* Expand: _t3 := y = 0 */
" movq %[y], %%rax\n\t"
" movq $0, %%rbx\n\t"
" subq %%rbx, %%rax\n\t"
" movq %%rax, %[_t3]\n\t"

/* True:    blk2 */
" jz blk2\n\t"
/* False:   blk3 */
" jmp blk3\n\t"

/* BBlock @ blk2 */
"blk2:\n\t"
/* Expand: x := 1 c 1 */
" movq $1, %%rax\n\t"
" movq $1, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[x]\n\t"

/* True:    blk4 */
" jmp blk4\n\t"
/* False:   0 */

/* BBlock @ blk3 */
"blk3:\n\t"
/* Expand: y := 2 c 2 */
" movq $2, %%rax\n\t"
" movq $2, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[y]\n\t"

/* True:    blk4 */
" jmp blk4\n\t"
/* False:   0 */

/* BBlock @ blk4 */
"blk4:\n\t"
/* True:    blk6 */
" jmp blk6\n\t"
/* False:   0 */

/* BBlock @ blk5 */
"blk5:\n\t"
/* Expand: y := 3 c 3 */
" movq $3, %%rax\n\t"
" movq $3, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[y]\n\t"

/* True:    blk6 */
" jmp blk6\n\t"
/* False:   0 */

/* BBlock @ blk6 */
"blk6:\n\t"
/* True:    0 */
/* False:   0 */

: [_t0] "+g" (_t0),
  [_t1] "+g" (_t1),
  [_t2] "+g" (_t2),
  [_t3] "+g" (_t3),
  [x] "+g" (x),
  [y] "+g" (y)
:
: "rax", "rbx", "rdx", "cc"
  );
  std::cout << "_t0: " << _t0 << std::endl;
  std::cout << "_t1: " << _t1 << std::endl;
  std::cout << "_t2: " << _t2 << std::endl;
  std::cout << "_t3: " << _t3 << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
}
