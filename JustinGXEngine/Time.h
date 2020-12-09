#pragma once

#include <chrono>

class Time
{
public:
	Time();

	bool Start();
	bool Stop();

	void Restart();

	double GetMillisecondsElapsed();

private:
	bool isRunning = false;

	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
};

