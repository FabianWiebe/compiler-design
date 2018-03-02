#include <iostream>

int main(int argc, char **argv)
{
  long _t0, _t1, _t2, x, y;
/* BBlock @ blk0 */
blk0:
  /* Expand: x := 27 c 27 */
  asm(
" movq $27, %%rax\n\t"
" movq $27, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[x]\n\t"

: [x] "+g" (x)
:
: "rax", "rbx", "rdx", "cc"
  );
  /* Expand: _t0 := x = 27 */
  if (x == 27) /* True:    blk1 */
  goto blk1;
/* False:   blk2 */
  else goto blk2;

/* BBlock @ blk1 */
blk1:
  /* Expand: _t2 := 2 * 3 */
  asm(
" movq $2, %%rax\n\t"
" movq $3, %%rbx\n\t"
" mulq %%rbx\n\t"
" movq %%rax, %[_t2]\n\t"

: [_t2] "+g" (_t2)
:
: "rax", "rbx", "rdx", "cc"
  );
  /* Expand: _t1 := x + _t2 */
  asm(
" movq %[x], %%rax\n\t"
" movq %[_t2], %%rbx\n\t"
" addq %%rbx, %%rax\n\t"
" movq %%rax, %[_t1]\n\t"

: [_t1] "+g" (_t1),
  [_t2] "+g" (_t2),
  [x] "+g" (x)
:
: "rax", "rbx", "rdx", "cc"
  );
  /* Expand: y := _t1 c _t1 */
  asm(
" movq %[_t1], %%rax\n\t"
" movq %[_t1], %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[y]\n\t"

: [_t1] "+g" (_t1),
  [y] "+g" (y)
:
: "rax", "rbx", "rdx", "cc"
  );
/* True:    blk2 */
  goto blk2;
/* False:   0 */

/* BBlock @ blk2 */
blk2:
/* True:    0 */
/* False:   0 */

  std::cout << "_t0: " << _t0 << std::endl;
  std::cout << "_t1: " << _t1 << std::endl;
  std::cout << "_t2: " << _t2 << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
}
