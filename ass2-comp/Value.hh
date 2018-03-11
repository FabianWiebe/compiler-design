#ifndef VALUE_H
#define	VALUE_H

#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>

class Node;

struct BaseStore {
	virtual std::ostream& to_stream(std::ostream& stream) const = 0;
	friend std::ostream& operator<< (std::ostream& stream, const BaseStore& store) {
		return store.to_stream(stream);
	}
	virtual ~BaseStore() = default;
	virtual bool operator== (const std::shared_ptr<BaseStore> &store) const = 0;
};
template <typename T>
struct Store : public BaseStore {
	Store(const T & v) : BaseStore(), value(v) {};
	const T value;
	virtual std::ostream& to_stream(std::ostream& stream) const {
		return stream << std::boolalpha << value;
	}
	virtual bool operator== (const std::shared_ptr<BaseStore> &store) const {
		auto ptr = std::dynamic_pointer_cast<Store<T>>(store);
		if (ptr) {
			return value == ptr->value;
		}
		return false;
	}
};

	class Value;

struct ArrayContainer : public BaseStore {
	ArrayContainer(std::vector<Value> array) : value(array) {}
	virtual std::ostream& to_stream(std::ostream& stream) const;
	virtual bool operator== (const std::shared_ptr<BaseStore> &store) const {
		auto ptr = std::dynamic_pointer_cast<ArrayContainer>(store);
		if (ptr) return this == ptr.get();
		return false;
	}
	std::vector<Value> value;
};

struct FunctionContainer : public BaseStore {
	FunctionContainer(Node& node) : value(node) {}
	virtual std::ostream& to_stream(std::ostream& stream) const {
		return stream << "function";
	}
	virtual bool operator== (const std::shared_ptr<BaseStore> &store) const {
		auto ptr = std::dynamic_pointer_cast<FunctionContainer>(store);
		if (ptr) return this == ptr.get();
		return false;
	}
	Node& value;
};


enum class Type {UNDEFINED, BOOL, LONG, DOUBLE, STRING, ARRAY, FUNCTION};
const std::string & type_as_string(const Type& type);


class Value {
public:
	long as_long() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value ? 1 : 0;
			}
			case Type::LONG: {
				auto ptr = std::dynamic_pointer_cast<Store<long>>(value);
				return ptr->value;
			}
			case Type::DOUBLE: {
				auto ptr = std::dynamic_pointer_cast<Store<double>>(value);
				return ptr->value;
			}
			case Type::STRING: {
				auto ptr = std::dynamic_pointer_cast<Store<std::string>>(value);
				return std::stoi(ptr->value);
			}
			default:
				throw std::invalid_argument( "Unkwon value type" );
		}
	}
	double as_double() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value ? 1. : 0.;
			}
			case Type::LONG: {
				auto ptr = std::dynamic_pointer_cast<Store<long>>(value);
				return ptr->value;
			}
			case Type::DOUBLE: {
				auto ptr = std::dynamic_pointer_cast<Store<double>>(value);
				return ptr->value;
			}
			case Type::STRING: {
				auto ptr = std::dynamic_pointer_cast<Store<std::string>>(value);
				return std::stod(ptr->value);
			}
			default:
				throw std::invalid_argument( "Unkwon value type" );
		}
	}
	std::string as_string() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value ? "true" : "false";
			}
			case Type::LONG: {
				auto ptr = std::dynamic_pointer_cast<Store<long>>(value);
				return std::to_string(ptr->value);
			}
			case Type::DOUBLE: {
				auto ptr = std::dynamic_pointer_cast<Store<double>>(value);
				return std::to_string(ptr->value);
			}
			case Type::STRING: {
				auto ptr = std::dynamic_pointer_cast<Store<std::string>>(value);
				return ptr->value;
			}
			default:
				throw std::invalid_argument( "Unkwon value type" );
		}
	}
	bool as_bool() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value;
			}
			case Type::LONG: {
				auto ptr = std::dynamic_pointer_cast<Store<long>>(value);
				return ptr->value;
			}
			case Type::DOUBLE: {
				auto ptr = std::dynamic_pointer_cast<Store<double>>(value);
				return ptr->value;
			}
			case Type::STRING: {
				auto ptr = std::dynamic_pointer_cast<Store<std::string>>(value);
				return !ptr->value.empty();
			}
			default:
				throw std::invalid_argument( "Unkwon value type" );
		}
	}
	std::vector<Value>& as_array() const {
		switch(type) {
			case Type::ARRAY: {
				auto ptr = std::dynamic_pointer_cast<ArrayContainer>(value);
				return ptr->value;
			}
			default:
				throw std::invalid_argument( "Cannot convert to array from type " + type_as_string(type));
		}
	}
	Node& as_function() const {
		switch(type) {
			case Type::FUNCTION: {
				auto ptr = std::dynamic_pointer_cast<FunctionContainer>(value);
				return ptr->value;
			}
			default:
				throw std::invalid_argument( "Cannot convert to node from type " + type_as_string(type));
		}
	}
	Value(bool v) : type(Type::BOOL), value(std::make_shared<Store<bool>>(v)) {}
	Value(long v) : type(Type::LONG), value(std::make_shared<Store<long>>(v)) {}
	Value(double v) : type(Type::DOUBLE), value(std::make_shared<Store<double>>(v)) {}
	Value(std::string v) : type(Type::STRING), value(std::make_shared<Store<std::string>>(v)) {}
	Value(std::vector<Value> v) : type(Type::ARRAY), value(std::make_shared<ArrayContainer>(v)) {}
	Value(Node& v) : type(Type::FUNCTION), value(std::make_shared<FunctionContainer>(v)) {}
	//Value(std::shared_ptr<BaseStore> v, Type t) : type(t), value(v) {}
	friend std::ostream& operator<< (std::ostream& stream, const Value& value) {
		return stream << *value.value;
	}

	bool is_array() const {
		return type == Type::ARRAY;
	}

	bool is_function() const {
		return type == Type::FUNCTION;
	}

	bool operator!= (const Value &value2) const {
		return !(value2 == *this);
	}
	bool operator== (const Value &value2) const {
		if (type == value2.type) {
			if (type == Type::DOUBLE) {
				return fabs(as_double() - value2.as_double()) < 0.0001;
			}
			return *value == value2.value;
		}
		if (type == Type::BOOL || type == Type::STRING ||
					value2.type == Type::BOOL || value2.type == Type::STRING) {
			throw std::invalid_argument( "Cannot compare string or bool with different type for == or !=" );
		}
		return fabs(as_double() - value2.as_double()) < 0.0001;
	}

	bool operator> (const Value &value2) const {
		if (type == Type::FUNCTION || type == Type::ARRAY ||
					value2.type == Type::FUNCTION || value2.type == Type::ARRAY) {
			throw std::invalid_argument( "Cannot compare function or array with > or <=" );
		}
		if (type == Type::BOOL || type == Type::STRING ||
					value2.type == Type::BOOL || value2.type == Type::STRING) {
			throw std::invalid_argument( "Cannot compare string or bool with > or <=" );
		}
		return as_double() > value2.as_double();
	}
	bool operator< (const Value &value2) const {
		return value2 > *this;
	}
	bool operator>= (const Value &value2) const {
		if (type == Type::FUNCTION || type == Type::ARRAY ||
					value2.type == Type::FUNCTION || value2.type == Type::ARRAY) {
			throw std::invalid_argument( "Cannot compare function or array with > or <=" );
		}
		if (type == Type::BOOL || type == Type::STRING ||
					value2.type == Type::BOOL || value2.type == Type::STRING) {
			throw std::invalid_argument( "Cannot compare string or bool with < or >=" );
		}
		return as_double() >= value2.as_double();
	}
	bool operator<= (const Value &value2) const {
		return value2 >= *this;
	}

	Type type;

	static Value parse_string(const std::string& s) {
		if (s.find(".") != std::string::npos) {
			try {
		        return Value(std::stod(s));
		    } catch(...) {}
		}
	    try {
	    	return Value(std::stol(s));
	    } catch(...) {}
	   	std::string lower_case_str(s);
	   	std::transform(lower_case_str.begin(), lower_case_str.end(), lower_case_str.begin(), ::tolower);
	   	if (lower_case_str == "true") return Value(true);
	   	if (lower_case_str == "false") return Value(false);
	   	return Value(s);
	}
	friend ArrayContainer;
private:
	std::shared_ptr<BaseStore> value;
};

#endif
