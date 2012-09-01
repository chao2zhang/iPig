#include "Timer.h"

Timer::Timer():startTicks(0), pausedTicks(0), paused(false), started(false){}

void Timer::start() {
	if (!started) {
		started = true;
		paused = false;
		startTicks = SDL_GetTicks();
	}
}

void Timer::stop() {
	started = false;
	paused = false;
}

void Timer::pause() {
	if (!paused) {
		paused = true;
		pausedTicks = SDL_GetTicks();
	}
}

void Timer::resume() {
	if (paused) {
		paused = false;
		startTicks += SDL_GetTicks() - pausedTicks;
	}
}

bool Timer::isStarted() const {
	return started;
}

bool Timer::isPaused() const {
	return paused;
}

int Timer::get() const {
	if (paused)
		return pausedTicks - startTicks;
	if (started)
		return SDL_GetTicks() - startTicks;
	return 0;
}
