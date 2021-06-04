#include "clock.h"
#include <thread>

void Clock::SleepUntilNextFrame()
{
	std::chrono::duration<double> targetDur(targetSpf);
	std::chrono::duration<double> dur; 

	auto now = std::chrono::high_resolution_clock::now();
	if((dur = now - startClock) < targetDur)
	{
		std::this_thread::sleep_for((targetDur-dur));
	}


	startClock = std::chrono::high_resolution_clock::now();
}