#include "labTrees.hh"
#include <fstream>

void dump_asm(BBlock *first_block, std::ostream& stream = std::cout) {
        stream << R"(
#include <iostream>
int main(int argc, char **argv)
{
  long x=10,y=0,_t0=17,_t1=17,_t2=17,_t3=17,_t4=17;
  asm(
)";
	dumpCFG(first_block, stream);
        stream << R"(
" nop \n\t"
: [x] "+g" (x),
  [y] "+g" (y),
  [_t0] "+g" (_t0),
  [_t1] "+g" (_t1),
  [_t2] "+g" (_t2),
  [_t3] "+g" (_t3),
  [_t4] "+g" (_t4)
:
: "rax", "rbx", "rdx", "cc"
  );
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
  std::cout << "_t0: " << _t0 << std::endl;
  std::cout << "_t1: " << _t1 << std::endl;
  std::cout << "_t2: " << _t2 << std::endl;
  std::cout << "_t3: " << _t3 << std::endl;
  std::cout << "_t4: " << _t4 << std::endl;
}
)";
}

int main(int argc, char **argv)
{
	// test->dump();
	// std::cout << "convertIf" << std::endl;
	// BBlock *first_block = new BBlock();
	// test->convert(first_block);
	// dumpCFG(first_block);
	// std::cout << std::endl << std::endl << "Test Case 2" << std::endl;
	test3->dump();
	std::cout << "simple test" << std::endl;
	BBlock *first_block = new BBlock();
	test3->convert(first_block);
	dump_asm(first_block);
	
	std::ofstream asm_file;
	asm_file.open("asm.cc");
	dump_asm(first_block, asm_file);
	asm_file.close();
	return 0;
}
