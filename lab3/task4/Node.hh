#pragma once

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <utility>
#include <memory>
#include <stack>
#include <map>
#include <stdexcept>
#include <cassert>

struct BaseStore {
	virtual ~BaseStore() = default;
};
template <typename T>
struct Store : public BaseStore {
	Store(T v) : BaseStore(), value(v) {};
	T value;
};

class Value {
public:
	enum class Type {BOOL, INT, DOUBLE, STRING};
	int as_int() {
		return 1;
	}
	double as_double() {
		return 1.;
	}
	std::string as_string() {
		return "";
	}
	bool as_bool() {
		return true;
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value;
			}
			case Type::INT: {
				auto ptr = std::dynamic_pointer_cast<Store<int>>(value);
				return ptr->value == 1;
			}
			case Type::DOUBLE: {
				auto ptr = std::dynamic_pointer_cast<Store<double>>(value);
				return ptr->value == 1.;
			}
			case Type::STRING: {
				auto ptr = std::dynamic_pointer_cast<Store<std::string>>(value);
				return !ptr->value.empty();
			}
		}
		return false;
	}
	Value(bool v) : type(Type::BOOL), value(std::make_shared<Store<bool>>(v)) {}
	Value(int v) : type(Type::INT), value(std::make_shared<Store<int>>(v)) {}
	Value(double v) : type(Type::DOUBLE), value(std::make_shared<Store<double>>(v)) {}
	Value(std::string v) : type(Type::STRING), value(std::make_shared<Store<std::string>>(v)) {}
	Type type;
private:
	std::shared_ptr<BaseStore> value;
};


class Environment {
public:
	Environment() {
		stack.emplace();
	}
	void new_context() {
		stack.emplace();
	}
	void clear_context() {
		stack.pop();
	}
	void set(std::string name, Value value) {
		std::cout << "setting " << name << " to " << value.as_string() << std::endl;
		auto mapping = stack.top();
		auto it = mapping.find(name);
		if (it != mapping.end()) {
			it->second = value;
		} else {
			mapping.insert(std::make_pair(name, value));
		}
	}
	Value& get(std::string name) {
		auto mapping = stack.top();
		auto it = mapping.find(name);
		if (it != mapping.end()) {
			return it->second;
		}
		throw std::invalid_argument( "name not found" );
	}
	friend std::ostream& operator<< (std::ostream& stream, const Environment& env) {
		auto mapping = env.stack.top();
		stream << "Current context:" << std::endl;
		for (auto & pair : mapping) {
			stream << pair.first << " : " << pair.second.as_string() << std::endl;
		}
	}
private:
	std::stack<std::map<std::string, Value>> stack;
};

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
		std::cout << "Default " << *this << std::endl;
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
		std::cout << "Execute " << *this << std::endl;
		for (auto & child : children) {
			child->execute(e);
		}
		return Value(value);
	}
};

class QuoteNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		std::cout << "Execute " << *this << std::endl;
		for (auto & child : children) {
			child->execute(e);
		}
		return Value(value);
	}
};

class AssignmentNode : public Node {
public:
	AssignmentNode(std::shared_ptr<Node> n, std::shared_ptr<Node> v) :
		Node("assignment", "="), name(n), value_node(v) {}
	virtual Value execute(Environment & e) {
		std::cout << "*Execute assignment " << *this << std::endl;
		auto computed_value = value_node->execute(e);
		std::cout << "*before setting " << std::endl;
		e.set(name->value, computed_value);
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
		std::cout << "Execute " << *this << std::endl;
		for (auto & child : children) {
			child->execute(e);
		}
		return Value(value);
	}
};

class CommandNode : public Node {
public:
	using Node::Node;
	virtual Value execute(Environment & e) {
		std::cout << "Execute " << *this << std::endl;
		for (auto & child : children) {
			child->execute(e);
		}
		return Value(value);
	}
};