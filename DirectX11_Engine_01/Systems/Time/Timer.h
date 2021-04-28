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
#ifdef _WIN32 //������ ȯ��
	chrono::time_point<chrono::steady_clock> start;
	chrono::time_point<chrono::steady_clock> stop;
#else //������ ���� ȯ��(������)
	chrono::time_point<chrono::system_clock> start;
	chrono::time_point<chrono::system_clock> stop;
#endif
};

