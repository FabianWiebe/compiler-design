#include <string>
#include <iostream>

#include "Value.hh"

// cannot be defined in header due to linker issues
std::ostream& ArrayContainer::to_stream(std::ostream& stream) const {
	stream << "Array:";
	int i = 0;
	for (const auto & val : value) {
		stream << std::endl << i++ << ":\t" << *val.value;
	}
	return stream;
}

const std::string & type_as_string(const Type& type) {
	using TypeMap = std::map<Type, std::string>;
	TypeMap m{TypeMap::value_type(Type::UNDEFINED, "undefined"),
		TypeMap::value_type(Type::BOOL, "bool"), TypeMap::value_type(Type::LONG, "long"), TypeMap::value_type(Type::DOUBLE, "double"),
		TypeMap::value_type(Type::STRING, "string"), TypeMap::value_type(Type::ARRAY, "array"), TypeMap::value_type(Type::FUNCTION, "function")};
	return m.find(type)->second;
}
