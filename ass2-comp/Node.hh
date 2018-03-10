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
#include <vector>

#include "Value.hh"
#include "Environment.hh"

class Node {
	using counter = std::map<std::string, size_t>;
public:
	std::string tag, value;
	std::list<std::shared_ptr<Node>> children;
	Node(std::string t = "uninitialised", std::string v = "uninitialised") : tag(t), value(v) {}
	void dump(std::ostream& stream=std::cout, int depth=0)
	{
		if (depth == 0) stream << "Built a parse-tree:" << std::endl;
	 	for (int i=0; i<depth; i++)
	 		stream << "  ";
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
			auto result = child->execute(e);
			if (e.returning) {
				return result;
			}
		}
		return Value(value);
	}
	virtual Value assign(Environment & e, Value value) {
		throw std::invalid_argument( "Node is no left node" );
	}
	virtual Value call(Environment & e, Value parameter_values) {
		throw std::invalid_argument( "Node is not a function" );
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
	AssignmentNode(std::shared_ptr<Node> left, std::shared_ptr<Node> v) : Node("assignment", "=") {
		children.push_back(left);
		children.push_back(v);
	}
	virtual Value execute(Environment & e) {
		auto value = *(++children.begin());
		auto computed_value = value->execute(e);
		return children.front()->assign(e, computed_value);
	}
};

class WordNode : public Node {
public:
	WordNode(std::string v) : Node("var", v) {}
	virtual Value execute(Environment & e) {
		return e.get(value);
	}
	virtual Value assign(Environment & e, Value v) {
		e.set(value, v);
		return v;
	}
};

class NotNode : public Node {
public:
	NotNode(std::shared_ptr<Node> bool_value) : Node("not", "!") {
			children.push_back(bool_value);
	}
	virtual Value execute(Environment & e) {
		return !children.front()->execute(e).as_bool();
	}
};

class ArrayAccessNode : public Node {
public:
	ArrayAccessNode(std::shared_ptr<Node> array, std::shared_ptr<Node> position) :
														Node("array_access", "[]") {
		children.push_back(array);
		children.push_back(position);
	}
	virtual Value execute(Environment & e) {
		auto itr = children.begin();
		auto array = (*itr)->execute(e).as_array();
		auto position = (*++itr)->execute(e).as_int() - 1;
		return array[position];
	}
	virtual Value assign(Environment & e, Value value) {
		auto itr = children.begin();
		auto& array = (*itr)->execute(e).as_array();
		auto position = (*++itr)->execute(e).as_int() - 1;
		array[position] = value;
		return value;
	}
};

class FunctionNode : public Node {
public:
	FunctionNode(std::string name, std::shared_ptr<Node> parameters, std::shared_ptr<Node> body) :
																			Node("function", name) {
		children.push_back(parameters);
		children.push_back(body);
	}
	// registers the function in the environment
	virtual Value execute(Environment & e) {
		Value function_value(*this);
		e.set(value, function_value);
		return 0;
	}
	// executes the function call
	virtual Value call(Environment & e,  Value parameter_values) {
		e.new_context();
		// set paramets
		children.front()->assign(e, parameter_values);

		auto body = *(++children.begin());
		auto result = body->execute(e);
		e.clear_context();
		return result;
	}
};

class ReturnNode : public Node {
public:
	ReturnNode(std::shared_ptr<Node> expression = nullptr) : Node("return", "") {
		if (expression) {
			children.push_back(expression);
		}
	}
	virtual Value execute(Environment & e) {
		Value result(0);
		if (!children.empty()) {
			result = children.front()->execute(e);
		}
		e.returning = true;
		return result;
	}
};

class ArrayNode : public Node {
public:
	ArrayNode() : Node("array", "") {}
	virtual Value execute(Environment & e) {
		std::vector<Value> array;
		array.reserve(children.size());
		for (auto & child : children) {
			array.emplace_back(child->execute(e));
		}
		return array;
	}
	virtual Value assign(Environment & e, Value value) {
		if (!value.is_array()) {
			throw std::invalid_argument( "Value is no array" );
		}
		auto array = value.as_array();
		if (children.size() != array.size()) {
			throw std::invalid_argument( "Number of values must match number of variables" );
		}
		auto itr = array.begin();
		for (auto & child : children) {
			child->assign(e, *itr++);
		}
		return value;
	}
};


class SizeNode : public Node {
public:
	SizeNode(std::shared_ptr<Node> value) : Node("size", "#") {
		children.push_back(value);
	}
	virtual Value execute(Environment & e) {
		auto array = children.front()->execute(e).as_array();
		return static_cast<int>(array.size());
	}
};

class IncrementNode : public Node {
public:
	IncrementNode(std::string variable_name) : Node("increment", variable_name) {}
	virtual Value execute(Environment & e) {
		auto retrieved_value = e.get(value);
		Value incremented_value(retrieved_value.as_int() + 1);
		e.set(value, incremented_value);
		return incremented_value;
	}
};

class CommandNode : public Node {
public:
	CommandNode(std::string & command, std::shared_ptr<Node> parameters = std::make_shared<ArrayNode>()) :
																					Node("command", command) {
		children.push_back(parameters);
	}
	virtual Value execute(Environment & e) {
		if (value == "print" || value == "io.write") {
			bool first = true;
			auto result_value = children.front()->execute(e);
			if (result_value.is_array()) {
				auto& array = result_value.as_array();
				bool first = true;
				auto seperator = value == "print" ? "\t" : "";
				for (auto itr = array.begin(); itr != array.end(); ++itr) {
					if (first) {
						first = false;
					} else {
						std::cout << seperator;
					}
					auto value = *itr;
					// replace \n with std::endl;
					if (value.type == Value::Type::STRING) {
						std::string str = value.as_string();
						size_t position = 0;
						auto next_position = str.find("\\n", position);
						while (next_position != std::string::npos) {
							std::cout << str.substr(position, next_position - position) << std::endl;
							position = next_position + 2;
							next_position = str.find("\\n", position);
						}
						std::cout << str.substr(position);
					} else {
						std::cout << value;
					}
				}
			} else {
				std::cout << result_value;
			}
			if (value == "print") {
				std::cout << std::endl;
			}
		} else if (value == "io.read") {
			int value;
			std::cin >> value;
			return value;
		} else {
			auto result = e.get(value);
			if (!result.is_function()) {
				throw std::invalid_argument( "Name is not a function" );
			}
			Node& function_node = result.as_function();
			return function_node.call(e, children.front()->execute(e));
		}
		return 0;
	}
};


class LoopNode : public Node {
public:
	LoopNode(std::shared_ptr<Node> condition, std::shared_ptr<Node> body, bool while_loop = true) :
													Node("loop", while_loop ? "while" : "do while") {
		children.push_back(condition);
		children.push_back(body);
	}
	virtual Value execute(Environment & e) {
		auto itr = children.begin();
		auto condition = *itr;
		auto body = *++itr;
		if (value == "while") {
			while (condition->execute(e).as_bool()) {
				auto result = body->execute(e);
				if (e.returning) {
					return result;
				}
			}
		} else { // do while
			do {
				auto result = body->execute(e);
				if (e.returning) {
					return result;
				}
			} while (condition->execute(e).as_bool());
				
		}
		return 0;
	}
};

class IfNode : public Node {
public:
	IfNode(std::shared_ptr<Node> condition, std::shared_ptr<Node> body) : Node("if", "") {
		children.push_back(condition);
		children.push_back(body);
	}
	virtual Value execute(Environment & e) {
		auto itr = children.begin();
		auto condition = *itr;
		auto body = *++itr;
		if (condition->execute(e).as_bool()) {
			return body->execute(e);
		} else {
			if (children.size() > 2) {
				auto else_part = *++itr;
				return else_part->execute(e);
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
		} else if (value == "^") {
			result = std::pow(left.as_double(), right.as_double());
		} else if (value == "%") {
			result = left.as_int() % right.as_int();
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
	ValueNode(std::string val) : Node("string", "\\\"" + val + "\\\""), value{val} {}
	virtual Value execute(Environment & e) {
		return value;
	}
private:
	Value value;
};

#endif
