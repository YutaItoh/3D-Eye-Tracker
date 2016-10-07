// Petter Strandmark 2012–2013.
#ifndef SPII_H
#define SPII_H

#include <sstream>
#include <stdexcept>
#include <string>

#include <spii/string_utils.h>
#include <spii/error_utils.h>

#ifdef _WIN32
#	ifdef spii_EXPORTS
#		define SPII_API __declspec(dllexport)
#		define SPII_API_EXTERN_TEMPLATE
#	else
#		define SPII_API __declspec(dllimport)
#		define SPII_API_EXTERN_TEMPLATE extern
#	endif
#else
#	define SPII_API
#	define SPII_API_EXTERN_TEMPLATE
#endif // WIN32

namespace spii
{

double SPII_API wall_time();

#define spii_assert(expr, ...) (expr) ? ((void)0) : spii::verbose_error(#expr, __FILE__, __LINE__, spii::to_string(__VA_ARGS__))

}

#endif
