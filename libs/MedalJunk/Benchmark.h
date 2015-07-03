#pragma once

#include <iostream>
#include <string>

class Benchmark {
	double PCFreq;
	mutable long long QPC;
	mutable double result;

public:
	Benchmark();
	void start() const;
	void stop() const;
	double time() const;
	double current() const;
	friend std::ostream& operator<<(std::ostream& out, const Benchmark& bm);
};