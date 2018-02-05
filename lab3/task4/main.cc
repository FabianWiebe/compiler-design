#include <iostream>
#include <memory>
#include "binary.tab.hh"
#include "Node.hh"
extern std::shared_ptr<Node> root;

void yy::parser::error(std::string const&err)
{
  std::cout << "It's one of the bad ones... " << err << std::endl;
}

int main(int argc, char **argv)
{
yy::parser parser;
  if(!parser.parse()) {
    //root->dump_as_graph();
	root->dump();
	std::cout << std::endl;
	Environment e;
	root->execute(e);
	std::cout << e;
  }
  return 0;
}
