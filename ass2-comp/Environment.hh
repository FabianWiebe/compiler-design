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
	void store(const std::string& name, Value& value) {
		auto it = const_values.find(name);
		if (it != const_values.end()) {
			throw std::invalid_argument( "Variable name \"" + name + "\" already used" );
		} else {
			const_values.insert(std::make_pair(name, value));
		}
	}
	void add_function(const std::string& name, Value& value) {
		auto it = functions.find(name);
		if (it != functions.end()) {
			it->second = value;
		} else {
			functions.insert(std::make_pair(name, value));
		}
	}
	Type get(const std::string& name) {
		auto itr = const_values.find(name);
		if (itr != const_values.end()) {
			return itr->second.type;
		}
		auto mapping = stack.top();
		auto it = mapping.find(name);
		if (it != mapping.end()) {
			return it->second;
		}
		throw std::invalid_argument( "Variable name \"" + name + "\" not found in current context" );
	}

	long get_size(const std::string& name) {
		return const_values.find(name)->second.as_array().size();
	}

	void update_name(const std::string& old_name, const std::string& new_name) {
		auto itr = const_values.find(old_name);
		const_values.insert(std::make_pair(new_name, itr->second));
		const_values.erase(itr);
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
	std::map<std::string, Value>& get_const_values() {
		return const_values;
	}
private:
	std::stack<std::map<std::string, Type>> stack;
	std::map<std::string, Value> const_values;
	std::map<std::string, Value> functions;
};

#endif
