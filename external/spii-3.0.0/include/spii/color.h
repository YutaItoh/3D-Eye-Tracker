// Petter Strandmark 2002, 2006, 2013

#ifndef SPII_COLOR_H
#define SPII_COLOR_H

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <windows.h>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <spii/spii.h>

namespace spii
{
	namespace
	{
		class Color
		{
			friend std::ostream& operator<<(std::ostream& stream,const Color& c);

		public:
		#ifdef _WIN32
			Color(unsigned short c): color(c) {}
			unsigned short color;
		#else
			Color(const char* s): str(s) {}
			const char* str;
		#endif

		};

		std::ostream& operator<<(std::ostream& stream,const Color& c)
		{
			stream.flush();
			#ifdef WIN32
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),c.color);
			#else
				stream << "\033[0m" << c.str;
			#endif
			stream.flush();

			return stream;
		}

		#ifdef _WIN32
			const Color NORMAL  = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
			const Color WHITE   = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			const Color RED  = FOREGROUND_RED|FOREGROUND_INTENSITY;
			const Color DKRED   = FOREGROUND_RED;
			const Color BLUE	= FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			const Color DKBLUE  = FOREGROUND_BLUE|FOREGROUND_GREEN;
			const Color GREEN   = FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			const Color DKGREEN   = FOREGROUND_GREEN;
			const Color YELLOW  = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			const Color BROWN   = FOREGROUND_RED|FOREGROUND_GREEN;
		#else
			const Color NORMAL  = "";
			const Color WHITE   = "\033[37;1m";
			const Color RED     = "\033[31;1m";
			const Color DKRED   = "\033[31m";
			const Color BLUE	= "\033[34;1m";
			const Color DKBLUE  = "\033[34m";
			const Color GREEN   = "\033[32;1m";
			const Color DKGREEN = "\033[32m";
			const Color YELLOW  = "\033[33;1m";
			const Color BROWN   = "\033[33m";
		#endif

		class Timer
		{
			double start_time;
			double elapsed_time;
			bool active = true;

		 public:
			template<typename String>
			Timer(const String& string)
				: elapsed_time(-1)
			{
				std::stringstream sout;
				sout << string << "...";
				std::cerr << std::left << std::setw(40) << sout.str() << " [ " << YELLOW << "WAIT" << NORMAL << " ] ";
				start_time = wall_time();
			}

			~Timer()
			{
				if (active) {
					fail();
				}
			}

			void OK()
			{
				if (active) {
					elapsed_time = wall_time() - start_time;
					std::cerr << "\b\b\b\b\b\b\b\b" << GREEN << "  OK  " << NORMAL << "]   ";
					std::cerr <<  elapsed_time << " s." << std::endl;
					active = false;
				}
			}

			double get_elapsed_time() const
			{
				spii_assert(elapsed_time >= 0);
				return elapsed_time;
			}

			void fail()
			{
				if (active) {
					std::cerr << "\b\b\b\b\b\b\b\b" << RED << "FAILED" << NORMAL << "]" << std::endl;
					active = false;
				}
			}
		};

		template<typename String, typename Function>
		void timed_block(const String& string, Function& function)
		{
			Timer timer(string);
			function();
			timer.OK();
		}
	}

}



#endif //ifndef

