#pragma once

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <utility>

class Node {
	using counter = std::map<std::string, size_t>;
public:
	std::string tag, value;
	std::list<Node> children;
	Node(std::string t = "uninitialised", std::string v = "uninitialised") : tag(t), value(v) {}
	//Node() { tag="uninitialised"; value="uninitialised"; }   // Bison needs this.
	void dump(int depth=0)
	{
		if (depth == 0) std::cout << "Built a parse-tree:" << std::endl;
	 	for(int i=0; i<depth; i++)
	    	std::cout << "  ";
	  	std::cout << tag << ":" << value << std::endl;
	  	for(auto i=children.begin(); i!=children.end(); i++)
	    	i->dump(depth+1);
	}
	void dump_as_graph(int depth=0)
	{
	  std::cout << "digraph {" << std::endl;
	  counter tag_counter;
	  dump_graph_node(get_id(tag_counter, tag), tag_counter, depth);
	  std::cout << "}" << std::endl;
	}
private:
	std::string get_id(counter& tag_counter, std::string& tag) {
		auto it = tag_counter.find(tag);
		size_t id = 1;
		if (it != tag_counter.end()) {
			id = ++it->second;
		} else {
			tag_counter.insert(std::make_pair(tag, id));
		}
		return tag + std::to_string(id);
	}
	void dump_graph_node(std::string id, counter& tag_counter, int depth=0)
	{
	  std::cout << id << " [label=\"" << tag << ":" << value << "\"];" << std::endl;
	  for(auto i=children.begin(); i!=children.end(); i++) {
	  	std::string child_id = get_id(tag_counter, i->tag);
	  	i->dump_graph_node(child_id, tag_counter, depth + 1);
	  	std::cout << id << " -> " << child_id << ";" << std::endl;
	  	//std::cout << "test  "<< std::endl;
	  }
	}
};
