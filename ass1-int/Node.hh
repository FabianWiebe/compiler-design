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
	void dump(std::ostream& stream=std::cout, int depth=0)
	{
		if (depth == 0) std::cout << "Built a parse-tree:" << std::endl;
	 	for (int i=0; i<depth; i++)
	 		std::cout << "  ";
	  	stream << tag << ":" << value << std::endl;
	  	for (auto & i : children)
	    	i->dump(stream, depth+1);
	}
	void dump_as_graph(std::ostream& stream=std::cout, int depth=0)
	{
	  stream << "digraph {" << std::endl;
	  counter tag_counter;
	  dump_graph_node(stream, get_id(tag_counter, tag), tag_counter, depth);
	  stream << "}" << std::endl << std::endl;
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
	void dump_graph_node(std::ostream& stream, std::string id, counter& tag_counter, int depth=0)
	{
	  stream << id << " [label=\"" << tag << ":" << value << "\"];" << std::endl;
	  for(auto i=children.begin(); i!=children.end(); i++) {
	  	std::string child_id = get_id(tag_counter, (*i)->tag);
	  	(*i)->dump_graph_node(stream, child_id, tag_counter, depth + 1);
	  	stream << id << " -> " << child_id << ";" << std::endl;
	  }
	}
};

class AssignmentNode : public Node {
public:
	AssignmentNode(std::shared_ptr<Node> name, std::shared_ptr<Node> value) :
		Node("assignment", "=") {
			children.push_back(name);
			children.push_back(value);
	}
	virtual Value execute(Environment & e) {
		auto itr = children.begin();
		auto name = *itr;
		auto value_node = *++itr;
		auto computed_value = value_node->execute(e);
		e.set(name->value, computed_value);
		if (!children.empty()) {
			children.front()->execute(e);
		}
		return computed_value;
	}
};

class WordNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		auto retrieved_value = e.get(value);
		return retrieved_value;
	}
};


class IncrementNode : public Node {
public:
	IncrementNode(std::string variable_name) :
		Node("Increment", variable_name) {}
	virtual Value execute(Environment & e) {
		auto retrieved_value = e.get(value);
		Value incremented_value(retrieved_value.as_int() + 1);
		e.set(value, incremented_value);
		return incremented_value;
	}
};

class CommandNode : public Node {
public:
	CommandNode(std::string & command) : Node("command", command) {}
	virtual Value execute(Environment & e) {
		if (value == "print" || value == "io.write") {
			bool first = true;
			if (!children.empty()) {
				for (auto & child : children) {
					if (!first) std::cout << '\t';
					first = false;
					std::cout << child->execute(e);
				}
			}
			if (value == "print") {
				std::cout << std::endl;
			}
		} else if (value == "io.read") {
			int value;
			std::cin >> value;
			return value;
		}
		return 0;
	}
};


class LoopNode : public Node {
public:
	LoopNode(std::shared_ptr<Node> condition, std::shared_ptr<Node> body, bool while_loop = true) :
		Node("Loop", while_loop ? "while" : "do while") {
			children.push_back(condition);
			children.push_back(body);
	}
	virtual Value execute(Environment & e) {
		auto itr = children.begin();
		auto condition = *itr;
		auto body = *++itr;
		if (value == "while") {
			while (condition->execute(e).as_bool()) {
				body->execute(e);
			}
		} else {
			do {
				body->execute(e);
			} while (condition->execute(e).as_bool());
				
		}
		return 0;
	}
};

class IfNode : public Node {
public:
	IfNode(std::shared_ptr<Node> condition, std::shared_ptr<Node> body) :
		Node("If", "") {
			children.push_back(condition);
			children.push_back(body);
	}
	virtual Value execute(Environment & e) {
		for (auto itr = children.begin(); itr != children.end(); ++itr) {
				auto condition = *itr;
				auto body = *++itr;
				if (condition->execute(e).as_bool()) {
					body->execute(e);
					break;
				}
		}
		return 0;
	}
};

class MathNode : public Node {
public:
	MathNode(const std::string & op, std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
		Node("math", op) {
			children.push_back(left);
			children.push_back(right);
		}
	virtual Value execute(Environment & e) {
		if (children.size() < 2) {
			throw std::invalid_argument( "Not two children present" );
		}
		auto itr = children.begin();
		auto left = (*itr)->execute(e);
		auto right = (*++itr)->execute(e);
		double result;
		if (value == "+") {
			result = left.as_double() + right.as_double();
		} else if (value == "-") {
			result = left.as_double() - right.as_double();
		} else if (value == "*") {
			result = left.as_double() * right.as_double();
		} else if (value == "/") {
			result = left.as_double() / right.as_double();
		} else {
			throw std::invalid_argument( "Unkwon math operation" );
		}
		if (left.type == Value::Type::INT && right.type == Value::Type::INT && result == floor(result)) {
			return static_cast<int>(result);
		}
		return result;
	}
};

class CompNode : public Node {
public:
	CompNode(const std::string & op, std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
		Node("compare", op) {
			children.push_back(left);
			children.push_back(right);
		}
	virtual Value execute(Environment & e) {
		if (children.size() < 2) {
			throw std::invalid_argument( "Not two children present" );
		}
		auto itr = children.begin();
		auto left = (*itr)->execute(e);
		auto right = (*++itr)->execute(e);
		if (value == "==") {
			return left == right;
		} else if (value == "!=") {
			return left != right;
		} else if (value == ">") {
			return left > right;
		} else if (value == "<") {
			return left < right;
		} else if (value == ">=") {
			return left >= right;
		} else if (value == "<=") {
			return left <= right;
		} else {
			throw std::invalid_argument( "Unkwon compare" );
		}
	}
};

class ValueNode : public Node {
public:
	ValueNode(bool val) : Node("bool", val ? "true" : "false"), value{val} {}
	ValueNode(int val) : Node("int", std::to_string(val)), value{val} {}
	ValueNode(double val) : Node("double", std::to_string(val)), value{val} {}
	ValueNode(std::string val) : Node("string", val), value{val} {}
	virtual Value execute(Environment & e) {
		return value;
	}
private:
	Value value;
};

#endif
