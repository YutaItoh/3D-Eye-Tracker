// Petter Strandmark 2013.
#ifndef SPII_FUNCTION_SERIALIZER_H
#define SPII_FUNCTION_SERIALIZER_H

#include <iostream>
#include <vector>

#include <spii/spii.h>
#include <spii/function.h>

namespace spii
{

class Serialize
{
	friend SPII_API std::ostream& operator << (std::ostream& out, const Serialize& serializer);
	friend SPII_API std::istream& operator >> (std::istream& in,  const Serialize& serializer);

public:
	Serialize(const Function& function) : 
		readonly_function(&function),
		writable_function(nullptr),
		user_space(nullptr),
		factory(nullptr)
	{ }

	Serialize(Function* function,
	          std::vector<double>* input_user_space,
	          const TermFactory& input_factory) : 
		readonly_function(nullptr),
		writable_function(function),
		user_space(input_user_space),
		factory(&input_factory)
	{ }

private:
	const Function* readonly_function;
	Function* writable_function;
	std::vector<double>* user_space;
	const TermFactory* factory;
};

SPII_API std::ostream& operator << (std::ostream& out, const Serialize& serializer);
SPII_API std::istream& operator >> (std::istream& in,  Serialize& serializer);
}

#endif
