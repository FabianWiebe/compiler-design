#ifndef VALUE_H
#define	VALUE_H


#include <memory>
#include <string>
#include <algorithm>

struct BaseStore {
	virtual ~BaseStore() = default;
};
template <typename T>
struct Store : public BaseStore {
	Store(const T & v) : BaseStore(), value(v) {};
	const T value;
};

class Value {
public:
	enum class Type {BOOL, INT, DOUBLE, STRING};
	int as_int() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value ? 1 : 0;
			}
			case Type::INT: {
				auto ptr = std::dynamic_pointer_cast<Store<int>>(value);
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
		}
		throw std::invalid_argument( "Unkwon value type" );
	}
	double as_double() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value ? 1. : 0.;
			}
			case Type::INT: {
				auto ptr = std::dynamic_pointer_cast<Store<int>>(value);
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
		}
		throw std::invalid_argument( "Unkwon value type" );
	}
	std::string as_string() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value ? "true" : "false";
			}
			case Type::INT: {
				auto ptr = std::dynamic_pointer_cast<Store<int>>(value);
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
		}
		throw std::invalid_argument( "Unkwon value type" );
	}
	bool as_bool() const {
		switch(type) {
			case Type::BOOL: {
				auto ptr = std::dynamic_pointer_cast<Store<bool>>(value);
				return ptr->value;
			}
			case Type::INT: {
				auto ptr = std::dynamic_pointer_cast<Store<int>>(value);
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
		}
		throw std::invalid_argument( "Unkwon value type" );
	}
	Value(bool v) : type(Type::BOOL), value(std::make_shared<Store<bool>>(v)) {}
	Value(int v) : type(Type::INT), value(std::make_shared<Store<int>>(v)) {}
	Value(double v) : type(Type::DOUBLE), value(std::make_shared<Store<double>>(v)) {}
	Value(std::string v) : type(Type::STRING), value(std::make_shared<Store<std::string>>(v)) {}
	Type type;

static Value parse_string(const std::string& s) {
	if (s.find(".") != std::string::npos) {
		try {
	        return Value(std::stod(s));
	    } catch(...) {}
	}
    try {
    	return Value(std::stoi(s));
    } catch(...) {}
   	std::string lower_case_str(s);
   	std::transform(lower_case_str.begin(), lower_case_str.end(), lower_case_str.begin(), ::tolower);
   	if (lower_case_str == "true") return Value(true);
   	if (lower_case_str == "false") return Value(false);
   	return Value(s);
}
private:
	std::shared_ptr<BaseStore> value;
};

#endif
