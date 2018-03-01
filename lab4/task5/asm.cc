#include <iostream>

int main(int argc, char **argv)
{
  long _t0, _t1, _t2, x, y;
  asm(
/* BBlock @ 0xcdf4a0 */
"blk0:\n\t"
/* Expand: x := 27 c 27 */
" movq $27, %%rax\n\t"
" movq $27, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[x]\n\t"

/* Expand: _t0 := x = 28 */
" movq %[x], %%rax\n\t"
" movq $28, %%rbx\n\t"
" subq %%rbx, %%rax\n\t"
" movq %%rax, %[_t0]\n\t"

/* True:    0xcdf660 (blk1) */
" jz blk1\n\t"
/* False:   0xcdf8f0 (blk2) */
" jmp blk2\n\t"

/* BBlock @ 0xcdf660 */
"blk1:\n\t"
/* Expand: _t2 := 2 * 3 */
" movq $2, %%rax\n\t"
" movq $3, %%rbx\n\t"
" mulq %%rbx\n\t"
" movq %%rax, %[_t2]\n\t"

/* Expand: _t1 := x + _t2 */
" movq %[x], %%rax\n\t"
" movq %[_t2], %%rbx\n\t"
" addq %%rbx, %%rax\n\t"
" movq %%rax, %[_t1]\n\t"

/* Expand: y := _t1 c _t1 */
" movq %[_t1], %%rax\n\t"
" movq %[_t1], %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[y]\n\t"

/* True:    0xcdf8f0 (blk2) */
" jmp blk2\n\t"
/* False:   0 */

/* BBlock @ 0xcdf8f0 */
"blk2:\n\t"
/* True:    0 */
/* False:   0 */

: [_t0] "+g" (_t0),
  [_t1] "+g" (_t1),
  [_t2] "+g" (_t2),
  [x] "+g" (x),
  [y] "+g" (y)
:
: "rax", "rbx", "rdx", "cc"
  );
  std::cout << "_t0: " << _t0 << std::endl;
  std::cout << "_t1: " << _t1 << std::endl;
  std::cout << "_t2: " << _t2 << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
}
