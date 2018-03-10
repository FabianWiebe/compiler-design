#include <iostream>
#include <memory>
#include "binary.tab.hh"
#include "Node.hh"
#include "labTrees.hh"
extern std::shared_ptr<Node> root;
extern FILE *yyin;

void yy::parser::error(std::string const&err)
{
  std::cerr << "Parsing error: " << err << std::endl;
}

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
	std::list<std::string> vars(var_names.begin(), var_names.end());
    output_end_of_asm(stream, vars);
    output_vars(stream, vars);
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
		std::ofstream parse_tree_file;
		parse_tree_file.open("cfg.dot");
		root->dump_as_graph(parse_tree_file);
		parse_tree_file.close();
		
		//root->dump();
		//std::cout << std::endl;
		Environment env;
		Statement *current_test = test3;
		current_test->dump();
		std::cout << "simple test" << std::endl;
		BBlock *first_block = new BBlock();
		current_test->convert(first_block);
		dump_asm(first_block);
		
		std::ofstream asm_file;
		asm_file.open("target.cc");
		dump_asm(first_block, asm_file);
		asm_file.close();
		
		//std::cout << e;
	}
	return 0;
}
