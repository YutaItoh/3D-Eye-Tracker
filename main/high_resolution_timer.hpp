// Copyright (c) 2005 Hartmut Kaiser
// Copyright (c) 2005 Christopher Diggins
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// Disclaimer: Not a Boost library

#if !defined(BOOST_HIGH_RESOLUTION_TIMER_HPP)
#define BOOST_HIGH_RESOLUTION_TIMER_HPP

#include <boost/config.hpp>
#include <boost/timer.hpp>

#if !defined(BOOST_WINDOWS)

//  For platforms other than Windows, simply fall back to boost::timer
namespace boost {
    typedef boost::timer high_resolution_timer;
}

#else

#include <stdexcept>
#include <limits>
#include <windows.h>

namespace boost {

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  high_resolution_timer 
    //      A timer object measures elapsed time.
    //      CAUTION: Windows only!
    //
    ///////////////////////////////////////////////////////////////////////////////
    class high_resolution_timer
    {
    public:
        // ctor
        high_resolution_timer() 
        {
            start_time.QuadPart = 0;
            frequency.QuadPart = 0;

            if (!QueryPerformanceFrequency(&frequency))
                throw std::runtime_error("Couldn't acquire frequency");

            restart(); 
        } 

        // restart timer
        void restart() 
        { 
            t.restart();
            if (!QueryPerformanceCounter(&start_time))
                throw std::runtime_error("Couldn't initialize start_time");
        } 

        // return elapsed time in seconds
        double elapsed() const                  
        { 
            LARGE_INTEGER now;
            if (!QueryPerformanceCounter(&now))
                throw std::runtime_error("Couldn't get current time");

            // QueryPerformanceCounter() workaround
            // http://support.microsoft.com/default.aspx?scid=kb;EN-US;q274323
            double d1 = double(now.QuadPart - start_time.QuadPart) / frequency.QuadPart;
            double d2 = t.elapsed();
            return ((d1 - d2) > 0.5) ? d2 : d1;
        }

        // return estimated maximum value for elapsed()
        double elapsed_max() const   
        {
            return (double((std::numeric_limits<LONGLONG>::max)())
                - double(start_time.QuadPart)) / double(frequency.QuadPart); 
        }

        // return minimum value for elapsed()
        double elapsed_min() const            
        { 
            return 1.0 / frequency.QuadPart; 
        }

    private:
        timer t; // backup in case of QueryPerformanceCounter() bug
        LARGE_INTEGER start_time;
        LARGE_INTEGER frequency;
    }; 

} // namespace boost

#endif  // !defined(BOOST_WINDOWS)

#endif  // !defined(BOOST_HIGH_RESOLUTION_TIMER_HPP)
