#pragma once

#include <chrono>
#include <string>
#include <iostream>

struct TimingSpan {
	std::chrono::steady_clock::time_point value;
};

class BenchmarkHelper {
public:
	static inline TimingSpan* StartWatch() {
		TimingSpan* tm = new TimingSpan();
		tm->value = std::chrono::high_resolution_clock::now();
		return tm;
	}

	static inline void ShowDuration(TimingSpan* span, std::string msg) {
		auto timeBefore = span->value;
		std::cout << msg << " " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeBefore).count() << std::endl;
	}

	static inline void ShowDurationFinal(TimingSpan* span, std::string msg) {
		auto timeBefore = span->value;
		std::cout << msg << " " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeBefore).count() << std::endl;
		delete span;
	}

};