#include "Time.h"

Time::Time()
{
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

bool Time::Start()
{
	if (isRunning)
		return false;
	else
	{
		start = std::chrono::high_resolution_clock::now();
		isRunning = true;
		return true;
	}
}

bool Time::Stop()
{
	if (!isRunning)
		return false;
	else
	{
		stop = std::chrono::high_resolution_clock::now();
		isRunning = false;
		return true;
	}
}

void Time::Restart()
{
	isRunning = true;
	start = std::chrono::high_resolution_clock::now();
}

double Time::GetMillisecondsElapsed()
{
	if (this->isRunning)
	{
		auto t = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - this->start);
		this->deltaTime = t.count();
		return t.count();
	}
	else
	{
		auto t = std::chrono::duration<double, std::milli>(this->stop - this->start);
		this->deltaTime = t.count();
		return t.count();
	}
}
