#include "Benchmark.h"
#include <Windows.h>

Benchmark::Benchmark() {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
    PCFreq = freq.QuadPart / 1000.0;
	result = 0.0;
}

void Benchmark::start() const {
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	QPC = li.QuadPart;
}

double Benchmark::current() const {
	stop();
	return result;
}

void Benchmark::stop() const {
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	result = double(li.QuadPart - QPC) / PCFreq;
}

double Benchmark::time() const {
	return result;
}

std::ostream& operator<<(std::ostream& out, const Benchmark& bm) {
	out << bm.time();
	return out;
}