#pragma once

#include <chrono>

class Time
{
public:
	Time();

	bool Start();
	bool Stop();

	void Restart();

	double GetElapsedMilliseconds();

	// The elasped time between each frame in seconds
	double deltaTime = 0.0;

private:
	bool isRunning = false;

	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
};

