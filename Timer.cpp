#include "Timer.h"

Timer::Timer()
{
	// initialize variables
	m_dElapsedTime = 0.0;
	m_StartTime.QuadPart = 0;
	m_EndTime.QuadPart = 0;

	// get ticks per second
	::QueryPerformanceFrequency(&m_frequency);
}

Timer::~Timer()
{
	// stop the timer
	Stop();
}

void Timer::Start()
{
	// start the timer
	::QueryPerformanceCounter(&m_StartTime);
}

void Timer::Stop()
{
	// stop the timer
	::QueryPerformanceCounter(&m_EndTime);
}

double Timer::GetElaspedTime()
{
	// return the total elasped time in ms
	return ((m_EndTime.QuadPart - m_StartTime.QuadPart) * 1000.0 / m_frequency.QuadPart);
}