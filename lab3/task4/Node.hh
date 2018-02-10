#ifndef NODE_H
#define	NODE_H

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <utility>
#include <memory>
#include <map>
#include <stdexcept>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "Value.hh"
#include "Environment.hh"

class Node {
	using counter = std::map<std::string, size_t>;
public:
	std::string tag, value;
	std::list<std::shared_ptr<Node>> children;
	Node(std::string t = "uninitialised", std::string v = "uninitialised") : tag(t), value(v) {}
	//Node() { tag="uninitialised"; value="uninitialised"; }   // Bison needs this.
	void dump(int depth=0)
	{
		if (depth == 0) std::cout << "Built a parse-tree:" << std::endl;
	 	for (int i=0; i<depth; i++)
	 		std::cout << "  ";
	  	std::cout << tag << ":" << value << std::endl;
	  	for (auto & i : children)
	    	i->dump(depth+1);
	}
	void dump_as_graph(int depth=0)
	{
	  std::cout << "digraph {" << std::endl;
	  counter tag_counter;
	  dump_graph_node(get_id(tag_counter, tag), tag_counter, depth);
	  std::cout << "}" << std::endl;
	}
	friend std::ostream& operator<< (std::ostream& stream, const Node& node) {
		stream << node.tag << " " << node.value;
	}
	virtual Value execute(Environment & e) {
		for (auto & child : children) {
			child->execute(e);
		}
		return Value(value);
	}
	virtual ~Node() = default;
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
	  	std::string child_id = get_id(tag_counter, (*i)->tag);
	  	(*i)->dump_graph_node(child_id, tag_counter, depth + 1);
	  	std::cout << id << " -> " << child_id << ";" << std::endl;
	  }
	}
};

class ConcatNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		std::stringstream ss;
		for (auto & child : children) {
			ss << child->execute(e).as_string();
		}
		auto str = ss.str();
		std::cout << "Concat to " << str << std::endl;
		return Value(str);
	}
};

class QuoteNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		return Value(value);
	}
};

class AssignmentNode : public Node {
public:
	AssignmentNode(std::shared_ptr<Node> n, std::shared_ptr<Node> v) :
		Node("assignment", "="), name(n), value_node(v) {}
	virtual Value execute(Environment & e) {
		auto computed_value = value_node->execute(e);
		e.set(name->value, computed_value);
		std::cout << "Execute assignment: " << name->value << "=" << computed_value.as_string() << std::endl;
		if (!children.empty()) {
			children.front()->execute(e);
		}
		return computed_value;
	}
private:
	std::shared_ptr<Node> name;
	std::shared_ptr<Node> value_node;
};

class VarNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		auto retrieved_value = e.get(value);
		std::cout << "From " << value << " retrieved " << retrieved_value.as_string() << std::endl;
		return retrieved_value;
	}
};

class WordNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		return Value::parse_string(value);
	}
};

class ShellNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		std::cout << "Executing shell, creating new context" << std::endl;
		e.new_context();
		auto result = children.front()->execute(e);
		std::cout << "Exiting shell, clearing context" << std::endl;
		e.clear_context();
		return result;
	}
};

class CommandNode : public Node {
public:
	CommandNode(std::shared_ptr<Node> cmd) : Node("command",""), command(cmd) {}
	virtual Value execute(Environment & e) {
		std::stringstream ss;
		ss << command->value;
		for (auto & child : children) {
			ss << " " << child->execute(e).as_string();
		}
		auto str = ss.str();
		std::cout << "Executing on bash: " << str << std::endl;
		std::system(str.c_str());
		return str;
	}
private:
	std::shared_ptr<Node> command;
};

class MathNode : public Node {
public:
	enum class Op {PLUS, MIN, MUL, DIV};
	MathNode(Op op, std::shared_ptr<Node> l, std::shared_ptr<Node> r) :
		Node("math", ""), operation(op), left(l), right(r) {}
	virtual Value execute(Environment & e) {
		switch(operation) {
			case Op::PLUS:
				return check_for_int(left->execute(e).as_double() + right->execute(e).as_double());
			case Op::MIN:
				return check_for_int(left->execute(e).as_double() - right->execute(e).as_double());
			case Op::MUL:
				return check_for_int(left->execute(e).as_double() * right->execute(e).as_double());
			case Op::DIV:
				return check_for_int(left->execute(e).as_double() / right->execute(e).as_double());
			default:
				throw std::invalid_argument( "Unkwon math operation" );
		}
	}
private:
	Value check_for_int(double value) {
		if (value == floor(value)) return static_cast<int>(value);
		return value;
	}
	Op operation;
	std::shared_ptr<Node> left;
	std::shared_ptr<Node> right;
};

#endif
