#ifndef ENVIRONMENT_H
#define	ENVIRONMENT_H

#include <stack>
#include <map>
#include <iostream>
#include <string>

#include "Value.hh"

class Environment {
public:
	Environment() {
		stack.emplace();
	}
	void new_context() {
		stack.emplace();
	}
	void clear_context() {
		returning = false;
		stack.pop();
	}
	void set(std::string& name, Value& value) {
		if (value.is_function()) {
			set_function(name, value);
		} else {
			set_variable(name, value);
		}
	}
	Value get(std::string name) {
		auto mapping = stack.top();
		auto it = mapping.find(name);
		if (it != mapping.end()) {
			return it->second;
		}
		it = functions.find(name);
		if (it != functions.end()) {
			return it->second;
		}
		throw std::invalid_argument( "Variable name \"" + name + "\" not found in current context" );
	}
	friend std::ostream& operator<< (std::ostream& stream, const Environment& env) {
		auto & mapping = env.stack.top();
		stream << "Current context: (Size: " << mapping.size() << ")" << std::endl;
		for (auto & pair : mapping) {
			if (pair.second.type == Value::Type::STRING) {
				stream << pair.first << ":\"" << pair.second << "\"" << std::endl;
			} else {
				stream << pair.first << ":" << pair.second << std::endl;
			}
		}
		return stream;
	}
	bool returning = false;
private:
	void set_variable(std::string& name, Value& value) {
		auto & mapping = stack.top();
		auto it = mapping.find(name);
		if (it != mapping.end()) {
			it->second = value;
		} else {
			mapping.insert(std::make_pair(name, value));
		}
	}
	void set_function(std::string& name, Value& value) {
		auto it = functions.find(name);
		if (it != functions.end()) {
			it->second = value;
		} else {
			functions.insert(std::make_pair(name, value));
		}
	}
	std::stack<std::map<std::string, Value>> stack;
	std::map<std::string, Value> functions;
};

#endif
