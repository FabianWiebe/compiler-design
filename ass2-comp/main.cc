#include <iostream>
#include <fstream>
#include <memory>
#include "binary.tab.hh"
#include "labTrees.hh"
extern Statement* root;
extern FILE *yyin;

void yy::parser::error(std::string const&err)
{
  std::cerr << "Parsing error: " << err << std::endl;
}

void dump_asm(Environment e, BBlock *first_block, std::ostream& stream = std::cout) {
        stream << R"(#include "stdio.h"
#include "math.h"

int main(int argc, char **argv)
{
)";
	std::list<Type> types{Type::LONG, Type::DOUBLE, Type::BOOL, Type::STRING};
	for (Type type : types) {
		auto var_names = e.get_all_of_type(type);
		if (!var_names.empty()) {
			auto itr = var_names.begin();
			stream << "  " << type_as_string(type) << " " << *itr;
			for (++itr; itr != var_names.end(); ++itr) {
				stream << ", " << *itr;
			}
			stream << ";" << std::endl;
		}
	}
	//output_start_of_asm(stream);
	dumpASM(first_block, stream);
    //output_end_of_asm(stream, vars);
	for (Type type : types) {	
		auto var_names = e.get_all_of_type(type);
    	output_vars(stream, var_names, type);
    }
    stream << "}" << std::endl;
}

int main(int argc, char **argv)
{
	yy::parser parser;
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
	}
	bool result = parser.parse();
	if (argc > 1) {
		fclose(yyin);
	}
	if (!result) {
		//root->dump();
		//std::cout << std::endl;
		Environment env;
		Statement *current_test = root;
		current_test->dump();
		std::cout << "simple test" << std::endl;
		BBlock *first_block = new BBlock();
		current_test->convert(env, first_block);
		//dump_asm(first_block);
		
		std::ofstream asm_file;
		asm_file.open("target.cc");
		dump_asm(env, first_block, asm_file);
		asm_file.close();

		std::ofstream parse_tree_file;
		parse_tree_file.open("cfg.dot");
		dumpCFG(first_block, parse_tree_file);
		parse_tree_file.close();
		
		//std::cout << e;
	}
	return 0;
}
