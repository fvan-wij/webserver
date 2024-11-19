#include <chrono>

class Timer 
{
	public:
		using Clock = std::chrono::steady_clock;
		using Ms = std::chrono::milliseconds;
		using Result = std::chrono::milliseconds;
		Timer();
		~Timer();

		void	reset();
		Result	elapsed_time() const;

	private:

		std::chrono::time_point<Clock> _start_time;
};

