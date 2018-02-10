#include <iostream>
#include <memory>
#include "binary.tab.hh"
#include "Node.hh"
extern std::shared_ptr<Node> root;
extern FILE *yyin;

void yy::parser::error(std::string const&err)
{
  std::cout << "It's one of the bad ones... " << err << std::endl;
}

int main(int argc, char **argv)
{
	yy::parser parser;
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
	}
	if(!parser.parse()) {
		std::ofstream parse_tree_file;
		parse_tree_file.open("parse.txt");
		root->dump_as_graph(parse_tree_file);
		parse_tree_file.close();
		root->dump();
		std::cout << std::endl;
		Environment e;
		root->execute(e);
		std::cout << e;
	}
	if (argc > 1) {
		fclose(yyin);
	}
	return 0;
}
