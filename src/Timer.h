#pragma once

#include "Base.h"

class Timer
{
private:
	int startTicks, pausedTicks;
	bool paused, started;
public:
	Timer();
	void start();
	void stop();
	void pause();
	void resume();
	bool isStarted() const;
	bool isPaused() const;
	int get() const;
};
