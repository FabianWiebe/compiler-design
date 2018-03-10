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
	void set(std::string& name, Type type) {
		auto & mapping = stack.top();
		auto it = mapping.find(name);
		if (it != mapping.end()) {
			it->second = type;
		} else {
			mapping.insert(std::make_pair(name, type));
		}
	}
	void add_function(std::string& name, Value& value) {
		auto it = functions.find(name);
		if (it != functions.end()) {
			it->second = value;
		} else {
			functions.insert(std::make_pair(name, value));
		}	}
	Type get(std::string name) {
		auto mapping = stack.top();
		auto it = mapping.find(name);
		if (it != mapping.end()) {
			return it->second;
		}
		throw std::invalid_argument( "Variable name \"" + name + "\" not found in current context" );
	}

	std::list<std::string> get_all_of_type(Type type) {
		auto mapping = stack.top();
		std::list<std::string> result;
		for (auto & pair : mapping) {
			if (pair.second == type) {
				result.push_back(pair.first);
			}
		}
		return result;
	}
	Value get_function(std::string name) {
		auto mapping = stack.top();
		auto it = functions.find(name);
		if (it != functions.end()) {
			return it->second;
		}
		throw std::invalid_argument( "Variable name \"" + name + "\" not found in current context" );
	}
	friend std::ostream& operator<< (std::ostream& stream, const Environment& env) {
		auto & mapping = env.stack.top();
		stream << "Current context: (Size: " << mapping.size() << ")" << std::endl;
		for (auto & pair : mapping) {
			
		}
		return stream;
	}
	bool returning = false;
private:
	std::stack<std::map<std::string, Type>> stack;
	std::map<std::string, Value> functions;
};

#endif
