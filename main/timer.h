#ifndef __PUPIL_TRACKER_TIMER_H__
#define __PUPIL_TRACKER_TIMER_H__

#include "high_resolution_timer.hpp"


class timer
{
public:
    timer() :
      m_timer(),
      m_elapsed(0.0), m_paused(false)
    {
    }

    void pause()
    {
        if (!m_paused)
        {
            m_elapsed += m_timer.elapsed();
            m_paused = true;
        }
    }
    void resume()
    {
        if (m_paused)
        {
            m_timer.restart();
            m_paused = false;
        }
    }

    double elapsed() const
    {
        if (m_paused)
            return m_elapsed;
        else
            return m_elapsed + m_timer.elapsed();
    }

    struct pause_guard
    {
        pause_guard(timer& t) : m_t(t)
        {
            m_t.pause();
        }
        ~pause_guard()
        {
            m_t.resume();
        }
        operator bool() const { return false; }
        
        timer& m_t;
    };

protected:
    boost::high_resolution_timer m_timer;
    double m_elapsed;

    bool m_paused;
};


#define PAUSE_TIMER(t) if(timer::pause_guard const& _timer_pause_guard_(t)) {} else

#endif // __PUPIL_TRACKER_TIMER_H__
