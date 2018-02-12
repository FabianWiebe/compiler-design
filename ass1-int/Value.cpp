#include <string>
#include <iostream>

#include "Value.hh"

std::ostream& ArrayContainer::to_stream(std::ostream& stream) const {
	stream << "Array:";
	int i = 0;
	for (const auto & val : value) {
		stream << std::endl << i++ << ":\t" << *val.value;
	}
	return stream;
}
