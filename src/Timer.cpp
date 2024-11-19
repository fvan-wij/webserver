#include <Timer.hpp>

Timer::Timer()
{
	_start_time = Clock::now();
}

Timer::~Timer()
{

}

void	Timer::reset()
{
	_start_time = Clock::now();
}

Timer::Result Timer::elapsed_time() const
{
	return std::chrono::duration_cast<Ms>(Clock::now() - _start_time);
}
