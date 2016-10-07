#ifndef __PUPIL_TRACKER_TIMER_H__
#define __PUPIL_TRACKER_TIMER_H__

#include "high_resolution_timer.hpp"

 
namespace eye_tracker {

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

class FrameRateCounter
{
public:
	FrameRateCounter()
		:frame_count_(0), kSkipFrameCount(50), fps_(0.0)	{
		timer_.pause();
	}
	~FrameRateCounter(){}
	double fps(){ return fps_; }
	size_t frame_count() { return frame_count_; }
	void count(){
		if (frame_count_++ == kSkipFrameCount){
			timer_.resume();/// Wait measuring time until the process gets stabilized
		}
		fps_ = (frame_count_ - kSkipFrameCount) / timer_.elapsed();
		return;
	}
protected:
	// Local variables initialized at the constructor
	double time_total;
	size_t frame_count_;
	double fps_;
	
	// Local variables 
	timer timer_;
	const size_t kSkipFrameCount;

private:
	// Prevent copying
	FrameRateCounter(const FrameRateCounter& other);
	FrameRateCounter& operator=(const FrameRateCounter& rhs);
};

}

#endif // __PUPIL_TRACKER_TIMER_H__
