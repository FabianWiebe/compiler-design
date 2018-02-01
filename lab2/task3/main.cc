#include<iostream>
#include "binary.tab.hh"
#include "Node.hh"
extern Node root;

void yy::parser::error(std::string const&err)
{
  std::cout << "It's one of the bad ones... " << err << std::endl;
}

int main(int argc, char **argv)
{
yy::parser parser;
  if(!parser.parse())
  	std::cout << "Built a parse-tree:" << std::endl;
    root.dump();
  return 0;
}
