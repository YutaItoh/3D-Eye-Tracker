// Petter Strandmark 2013–2014.
#ifndef SPII_STRING_UTILS_H
#define SPII_STRING_UTILS_H

#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>
#include <vector>

namespace spii
{

// to_string converts all its arguments to a string and
// concatenates.
//
// Anonymous namespace is needed because the base case for the
// template recursion is a normal function.
//
// Works for
//
//  std::pair
//  std::tuple
//  std::vector
//  std::set
//  std::map
//
// and combinations thereof, e.g. vector<pair<int, string>>.
namespace {

	template<typename T1, typename T2>
	std::ostream& operator<<(std::ostream& stream, const std::pair<T1, T2>& p)
	{
		stream << '(' << p.first << ", " << p.second << ')';
		return stream;
	}

	//
	// Printing a std::tuple requires the following helper class.
	//
	template<typename Tuple, std::size_t i, std::size_t N>
	struct print_tuple_helper
	{
		static void print(std::ostream& stream, const Tuple& t)
		{
			stream << std::get<i>(t) << ", ";
			print_tuple_helper<Tuple, i + 1, N>::print(stream, t);
		}
	};

	template<typename Tuple, std::size_t N>
	struct print_tuple_helper<Tuple, N, N>
	{
		static void print(std::ostream& stream, const Tuple& t)
		{
			stream << std::get<N>(t);
		}
	};

	template<typename... Args>
	std::ostream& operator<<(std::ostream& stream, const std::tuple<Args...>& t)
	{
		stream << '(';
		typedef print_tuple_helper<std::tuple<Args...>, 0, sizeof...(Args) - 1> Helper;
		Helper::print(stream, t);
		stream << ')';
		return stream;
	}

	template<typename Container>
	void add_container_to_stream(std::ostream* stream, const Container& container)
	{
		bool first = true;
		for (const auto& value : container) {
			if (!first) {
				*stream << ", ";
			}
			*stream << value;
			first = false;
		}
	}

	template<typename T, typename Alloc>
	std::ostream& operator<<(std::ostream& stream, const std::vector<T, Alloc>& v)
	{
		stream << '[';
		add_container_to_stream(&stream, v);
		stream << ']';
		return stream;
	}

	template<typename T, typename Comp, typename Alloc>
	std::ostream& operator<<(std::ostream& stream, const std::set<T, Comp, Alloc>& s)
	{
		stream << '{';
		add_container_to_stream(&stream, s);
		stream << '}';
		return stream;
	}

	template<typename T1, typename T2, typename Comp, typename Alloc>
	std::ostream& operator<<(std::ostream& stream, const std::map<T1, T2, Comp, Alloc>& m)
	{
		stream << '[';
		add_container_to_stream(&stream, m);
		stream << ']';
		return stream;
	}

	void add_to_stream(std::ostream*)
	{  }

	template<typename T, typename... Args>
	void add_to_stream(std::ostream* stream, T&& t, Args&&... args)
	{
		(*stream) << std::forward<T>(t);
		add_to_stream(stream, std::forward<Args>(args)...);
	}

	std::string to_string()
	{
		return{};
	}

	// Overload for string literals.
	template<size_t n>
	std::string to_string(const char(&c_str)[n])
	{
		return{c_str};
	}

	template<typename... Args>
	std::string to_string(Args&&... args)
	{
		std::ostringstream stream;
		add_to_stream(&stream, std::forward<Args>(args)...);
		return stream.str();
	}
}

template<typename T>
T from_string(const std::string& input_string)
{
	std::istringstream input_stream(input_string);
	T t;
	input_stream >> t;
	if (!input_stream) {
		std::ostringstream error;
		error << "Could not parse " << typeid(T).name() << " from \"" << input_string << "\".";
		throw std::runtime_error(error.str());
	}
	return t;
}

template<typename T>
T from_string(const std::string& input_string, T default_value)
{
	std::istringstream input_stream(input_string);
	T t;
	input_stream >> t;
	if (!input_stream) {
		t = default_value;
	}
	return t;
}

namespace
{
	std::ostream& format_string_internal(std::ostream& stream,
	                                     const char* str,
	                                     const std::vector<std::string>& arguments)
	{
		while (*str) {
			if (*str == '%') {
				++str;
				if (*(str) == '%') {
					// OK. This will result in "%".
					stream << '%';
					++str;
				}
				else {
					int digit = *str - '0';
					++str;
					if (digit < 0 || digit > 9) {
						throw std::invalid_argument("Format specifier must be in {0, ..., 9}.");
					}
					if (digit >= arguments.size()) {
						throw std::invalid_argument("Too few arguments to format_string.");
					}

					stream << arguments.at(digit);

					// To allow format specifiers of the type "%0%".
					//if (*str == '%') {
					//	++str;
					//}
				}
			}
			else {
				stream << *str;
				++str;
			}
		}
		return stream;
	}
}


template<typename... Args>
std::ostream& format_string(std::ostream& stream, const char* str, Args&&... args)
{
	std::vector<std::string> arguments = {to_string(std::forward<Args>(args))...};
	return format_string_internal(stream, str, arguments);
}

template<typename... Args>
std::ostream& format_string(std::ostream& stream, const std::string& str, Args&&... args)
{
	return format_string(stream, str.c_str(), std::forward<Args>(args)...);
}

template<typename... Args>
std::string format_string(const char* str, Args&&... args)
{
	std::ostringstream stream;
	format_string(stream, str, std::forward<Args>(args)...);
	return stream.str();
}

template<typename... Args>
std::string format_string(const std::string& str, Args&&... args)
{
	return format_string(str.c_str(), std::forward<Args>(args)...);
}

}

#endif
