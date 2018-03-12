#include <string>
#include <iostream>

#include "Value.hh"

// cannot be defined in header due to linker issues
std::ostream& ArrayContainer::to_stream(std::ostream& stream) const {
	stream << "{";
	if (!value.empty()) {
		auto itr = value.begin();
		stream << *itr;
		for (++itr; itr != value.end(); ++itr) {
			stream << ", " << *itr;
		}
	}
	stream << "}";
	return stream;
}

const std::string & type_as_string(const Type& type) {
	using TypeMap = std::map<Type, std::string>;
	TypeMap m{TypeMap::value_type(Type::UNDEFINED, "undefined"), TypeMap::value_type(Type::VOID, "void"),
		TypeMap::value_type(Type::BOOL, "bool"), TypeMap::value_type(Type::LONG, "long"), TypeMap::value_type(Type::DOUBLE, "double"),
		TypeMap::value_type(Type::STRING, "string"), TypeMap::value_type(Type::ARRAY, "array"), TypeMap::value_type(Type::FUNCTION, "function")};
	auto itr = m.find(type);
	if (itr != m.end()) {
		return itr->second;
	}
	throw std::invalid_argument( "Unkwon type" );
}
