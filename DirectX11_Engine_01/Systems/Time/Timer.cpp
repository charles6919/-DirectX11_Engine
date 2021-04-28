#include "../../stdafx.h"
#include "Timer.h"

Timer::Timer()
{
	start = chrono::high_resolution_clock::now();
	stop = chrono::high_resolution_clock::now();
}

double Timer::GetMilisecondsElapsed()
{
	if (isRunning)
	{
		auto elapsed = chrono::duration<double, milli>(chrono::high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = chrono::duration<double, milli>(stop - start);
		return elapsed.count();
	}
}

void Timer::Restart()
{
	isRunning = true;
	start = chrono::high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!isRunning)
		return false;

	stop = chrono::high_resolution_clock::now();
	isRunning = false;
	return true;
}

bool Timer::Start()
{
	if (isRunning)
		return false;

	start = chrono::high_resolution_clock::now();
	isRunning = true;
	return true;
}
