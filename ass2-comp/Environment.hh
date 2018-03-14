#ifndef ENVIRONMENT_H
#define	ENVIRONMENT_H

#include <stack>
#include <map>
#include <iostream>
#include <string>

#include "Value.hh"
class Function;
class BBlock;

class Environment {
public:
	Environment() {
		stack.emplace();
	}
	void new_context(BBlock* return_block) {
		//stack.emplace();
		return_blocks.push(return_block);
		return_types.emplace();
		function_parms.emplace();
	}
	void set_return_type(Type type) {
		return_types.pop();
		return_types.push(type);
	}
	BBlock* get_return_block() {
		return return_blocks.top();
	}
	Type get_return_type() {
		return return_types.top();
	}
	Type clear_context() {
		//stack.pop();
		return_blocks.pop();
		Type type = return_types.top();
		return_types.pop();
		function_parms.pop();
		return type;
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
	void set_function_parm(std::string& name, Type type) {
		auto & parms = function_parms.top();
		auto it = parms.find(name);
		if (it != parms.end()) {
			throw std::invalid_argument( "Function parm \"" + name + "\" already set" );
		} else {
			parms.insert(std::make_pair(name, type));
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
	void add_function(const std::string& name, Function* func) {
		auto it = functions.find(name);
		if (it != functions.end()) {
			it->second = func;
		} else {
			functions.insert(std::make_pair(name, func));
		}
	}
	Type get(const std::string& name) {
		if (!function_parms.empty()) {
			auto & parms = function_parms.top();
			auto fp_it = parms.find(name);
			if (fp_it != parms.end()) {
				return fp_it->second;
			}
		}
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
	Function* get_function(std::string name) {
		auto mapping = stack.top();
		auto it = functions.find(name);
		if (it != functions.end()) {
			return it->second;
		}
		throw std::invalid_argument( "Function name \"" + name + "\" not found in current context" );
	}

	std::map<std::string, Function*>& get_functions() {
		return functions;
	}

	friend std::ostream& operator<< (std::ostream& stream, const Environment& env) {
		auto & mapping = env.stack.top();
		stream << "Current context: (Size: " << mapping.size() << ")" << std::endl;
		for (auto & pair : mapping) {
			
		}
		return stream;
	}
	std::map<std::string, Value>& get_const_values() {
		return const_values;
	}
private:
	std::stack<std::map<std::string, Type>> stack;
	std::stack<std::map<std::string, Type>> function_parms;
	std::stack<BBlock*> return_blocks;
	std::stack<Type> return_types;
	std::map<std::string, Value> const_values;
	std::map<std::string, Function*> functions;
};

#endif
