#include "clock.h"
#include <windows.h>
#include <timeapi.h>

UINT GetMinTimer(){
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(tc));
	return tc.wPeriodMin;
};
LONGLONG GetFreq(){
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}
auto frequency = GetFreq();
UINT minTimer = GetMinTimer(); 
LARGE_INTEGER startClockHr{};
void Clock::SleepUntilNextFrame()
{
	timeBeginPeriod(minTimer);
	ULONGLONG targetCount = frequency*(targetSpf);
	ULONGLONG dif; 

	LARGE_INTEGER nowTicks;
	QueryPerformanceCounter(&nowTicks);
	if((dif = nowTicks.QuadPart - startClockHr.QuadPart) < targetCount)
	{
		LONG sleepTime = ((targetCount-dif)/(frequency/1000));
		if(sleepTime>0)
			Sleep(sleepTime);
	}

	QueryPerformanceCounter(&startClockHr);
	timeEndPeriod(minTimer);
}