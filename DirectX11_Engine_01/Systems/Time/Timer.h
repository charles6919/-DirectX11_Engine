#pragma once
#include <chrono>
class Timer
{
public:
	Timer();
	double GetMilisecondsElapsed();
	void Restart();
	bool Stop();
	bool Start();

private:
	bool isRunning = false;
#ifdef _WIN32 //윈도우 환경
	chrono::time_point<chrono::steady_clock> start;
	chrono::time_point<chrono::steady_clock> stop;
#else //윈도우 외의 환경(리눅스)
	chrono::time_point<chrono::system_clock> start;
	chrono::time_point<chrono::system_clock> stop;
#endif
};

