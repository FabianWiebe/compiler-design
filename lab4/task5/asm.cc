#include <iostream>
int main(int argc, char **argv)
{
  long _t0, _t1, x, y;
  asm(
/* BBlock @ 0x1b5e380 */
"blk0:\n\t"
/* Expand: x := 27 c 27 */
" movq $27, %%rax\n\t"
" movq $27, %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[x]\n\t"

/* Expand: _t1 := 2 * 3 */
" movq $2, %%rax\n\t"
" movq $3, %%rbx\n\t"
" mulq %%rbx\n\t"
" movq %%rax, %[_t1]\n\t"

/* Expand: _t0 := x + _t1 */
" movq %[x], %%rax\n\t"
" movq %[_t1], %%rbx\n\t"
" addq %%rbx, %%rax\n\t"
" movq %%rax, %[_t0]\n\t"

/* Expand: y := _t0 c _t0 */
" movq %[_t0], %%rax\n\t"
" movq %[_t0], %%rbx\n\t"
/* copy is a dummy operation */
" movq %%rax, %[y]\n\t"

/* True:    0 */
/* False:   0 */
: [_t0] "+g" (_t0),
  [_t1] "+g" (_t1),
  [x] "+g" (x),
  [y] "+g" (y)
:
: "rax", "rbx", "rdx", "cc"
  );
  std::cout << "_t0: " << _t0 << std::endl;
  std::cout << "_t1: " << _t1 << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
}
