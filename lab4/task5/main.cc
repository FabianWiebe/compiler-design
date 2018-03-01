#include "labTrees.hh"
#include <fstream>
#include <string>

void dump_asm(BBlock *first_block, std::ostream& stream = std::cout) {
        stream << R"(#include <iostream>
int main(int argc, char **argv)
{
)";
	auto& var_names = Expression::var_names;
	if (!var_names.empty()) {
		auto itr = var_names.begin();
		stream << "  long " << *itr;
		for (++itr; itr != var_names.end(); ++itr) {
			stream << ", " << *itr;
		}
		stream << ";" << std::endl;
	}
  stream << R"(  asm(
)";
	dumpCFG(first_block, stream);
    stream << ":";
    if (!var_names.empty()) {
		auto itr = var_names.begin();
		stream << " [" << *itr << "] \"+g\" (" << *itr << ")";
		for (++itr; itr != var_names.end(); ++itr) {
			stream << "," << std::endl << "  [" << *itr << "] \"+g\" (" << *itr << ")";
		}
		stream << std::endl;
    }
	stream << R"(:
: "rax", "rbx", "rdx", "cc"
  );)" << std::endl;
	for (const std::string& var_name : var_names) {
		stream << "  std::cout << \"" << var_name << ": \" << " << var_name << " << std::endl;" << std::endl;
	}
	stream << R"(}
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
