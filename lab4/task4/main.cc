#include "labTrees.hh"

int main(int argc, char **argv)
{
	// test->dump();
	// std::cout << "convertIf" << std::endl;
	// BBlock *first_block = new BBlock();
	// test->convert(first_block);
	// dumpCFG(first_block);
	// std::cout << std::endl << std::endl << "Test Case 2" << std::endl;
	test2->dump();
	std::cout << "convertIfs" << std::endl;
	BBlock *first_block = new BBlock();
	test2->convert(first_block);
	dumpCFG(first_block);
	return 0;
}
