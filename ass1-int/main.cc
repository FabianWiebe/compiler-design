#include <iostream>
#include <memory>
#include "binary.tab.hh"
#include "Node.hh"
extern std::shared_ptr<Node> root;
extern FILE *yyin;

void yy::parser::error(std::string const&err)
{
  std::cerr << "Parsing error: " << err << std::endl;
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
		parse_tree_file.open("parse.txt");
		root->dump_as_graph(parse_tree_file);
		parse_tree_file.close();
		
		//root->dump();
		//std::cout << std::endl;
		Environment env;
		try {
			root->execute(env);
		} catch (std::exception& exception) {
			std::cerr << exception.what() << std::endl;
		}
		
		//std::cout << e;
	}
	return 0;
}
