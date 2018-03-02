#include "labTrees.hh"
#include <fstream>
#include <string>

void dump_asm(BBlock *first_block, std::ostream& stream = std::cout) {
        stream << R"(#include <iostream>

int main(int argc, char **argv)
{
)";
	const auto& var_names = Expression::var_names;
	if (!var_names.empty()) {
		auto itr = var_names.begin();
		stream << "  long " << *itr;
		for (++itr; itr != var_names.end(); ++itr) {
			stream << ", " << *itr;
		}
		stream << ";" << std::endl;
	}
	output_start_of_asm(stream);
	dumpCFG(first_block, stream);
    output_end_of_asm(stream, std::list<std::string>(var_names.begin(), var_names.end()));
}

int main(int argc, char **argv)
{
	// test->dump();
	// std::cout << "convertIf" << std::endl;
	// BBlock *first_block = new BBlock();
	// test->convert(first_block);
	// dumpCFG(first_block);
	// std::cout << std::endl << std::endl << "Test Case 2" << std::endl;
	Statement *current_test = test2;
	current_test->dump();
	std::cout << "simple test" << std::endl;
	BBlock *first_block = new BBlock();
	current_test->convert(first_block);
	dump_asm(first_block);
	
	std::ofstream asm_file;
	asm_file.open("asm.cc");
	dump_asm(first_block, asm_file);
	asm_file.close();
	return 0;
}
