#pragma once
#include <windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	// methods
	void Start();
	void Stop();
	double GetElaspedTime();

private:
	LARGE_INTEGER m_frequency;	// ticks per second
	LARGE_INTEGER m_StartTime;	// ticks at start
	LARGE_INTEGER m_EndTime;	// ticks at end
	double m_dElapsedTime;		// total elapsed time in ms
};
