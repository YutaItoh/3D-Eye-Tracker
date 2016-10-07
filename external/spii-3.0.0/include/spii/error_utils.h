// Petter Strandmark 2013–2014.
#ifndef SPII_ERROR_UTILS_H
#define SPII_ERROR_UTILS_H

#include <stdexcept>
#include <string>

namespace spii
{

//
// Enables expressions like:
//
//    check(a == 42, a, " is not 42.");
//
// Will throw if expression is false.
//
template<typename... Args>
void check(bool everything_OK, Args&&... args)
{
	if (!everything_OK) {
		throw std::runtime_error(to_string(std::forward<Args>(args)...));
	}
}

namespace
{
	// Removes the path from __FILE__ constants and keeps the name only.
	std::string extract_file_name(const char* full_file_cstr)
	{
		using namespace std;

		// Extract the file name only.
		string file(full_file_cstr);
		auto pos = file.find_last_of("/\\");
		if (pos == string::npos) {
			pos = 0;
		}
		file = file.substr(pos + 1);  // Returns empty string if pos + 1 == length.

		return file;
	}

	void verbose_error_internal(const char* expr, const char* full_file_cstr, int line, const std::string& args)
	{
		std::stringstream stream;
		stream << "Assumption failed: " << expr << " in " << extract_file_name(full_file_cstr) << ":" << line << ". "
		       << args;
		throw std::logic_error(stream.str());
	}
}

template<typename... Args>
void verbose_error(const char* expr, const char* full_file_cstr, int line, Args&&... args)
{
	verbose_error_internal(expr, full_file_cstr, line, to_string(std::forward<Args>(args)...));
}

//#define ASSERT(expr, ...) (expr) ? ((void)0) : spii::verbose_error(#expr, __FILE__, __LINE__, spii::to_string(__VA_ARGS__))

}

#endif
