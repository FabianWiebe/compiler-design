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
