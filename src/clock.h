#ifndef CLOCK_H_GUARD
#define CLOCK_H_GUARD

#include <chrono>

class Clock
{
public:
	double targetSpf = 0.01600;

	std::chrono::time_point<std::chrono::high_resolution_clock> startClock; 
	void SleepUntilNextFrame();

	Clock():startClock(std::chrono::high_resolution_clock::now()) {}
};

#endif /* CLOCK_H_GUARD */
