#pragma once
#include <chrono>

class Timer
{
public:
	Timer() noexcept : now(std::chrono::high_resolution_clock::now()) {}

	float mark() noexcept {
		float old = getTime();
		now = std::chrono::high_resolution_clock::now();
		return old;
	}

	float getTime() const noexcept {
		return std::chrono::duration<float>(std::chrono::steady_clock::now() - now).count();
	}
private:
	std::chrono::steady_clock::time_point now;
};
