#include <iostream>

int main(int argc, char **argv)
{
  long _t0, _t1, _t2, x, y;
/* BBlock @ blk0 */
blk0:
  /* Expand: x :=   27 c 27 */
  x = 27;
  /* Expand: _t0 :=   x = 27 */
  if (x == 27) /* True:    blk1 */
  goto blk1;
/* False:   blk2 */
  else goto blk2;

/* BBlock @ blk1 */
blk1:
  /* Expand: _t2 :=   2 * 3 */
  _t2 = 2 * 3;
  /* Expand: _t1 :=   x + _t2 */
  _t1 = x + _t2;
  /* Expand: y :=   _t1 c _t1 */
  y = _t1;
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
