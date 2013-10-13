/*
 * pi.cpp
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <future>
#include <gmp.h>
#include <gmpxx.h>
#include "pi.h"
#include "tsio.h"

using namespace std;

template<typename T>
void print_percent(const T a, const T b)
{
	double rel = (double)a / (double)b;
	rel *= 100;
	if (rel == 0.0f)
		ts << "\r  0.0%";
	else if (rel == 100.0f)
		ts << "\r100.0%";
	else
		ts << "\r" << fixed << setprecision(1) << rel;
}

mpf_class pow(const mpf_class src, const unsigned long int exp)
{
	mpf_class tmp;
	mpf_pow_ui(tmp.get_mpf_t(), src.get_mpf_t(), exp);
	return tmp;
}

void pi::do_calculate(const uint64_t phase, const uint64_t runs)
{
	thread_local mpf_class local_sum(0, precision);
	thread_local mpf_class n(phase + 1, precision);
	ts.lock();
	if (phase == 0)
	{
		print_percent(0, 100);
		for (thread_local uint64_t i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
			print_percent(i, runs);
		} 
		ts.lprintf("\n");
	}
	else
	{
		for (thread_local uint64_t i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
		}
	}
	ts.unlock();
	add_sum(local_sum);
	//ts.lprintf("Thread number %llu \thas finished!\n", phase + 1);
}

void join_all(vector<thread>& v)
{
	for (auto& t : v)
		t.join();
}

void pi::calculate(const uint64_t runs)
{
	vector<thread> t (threads);

	for (uint64_t phase = 0; phase < threads; phase++)
	{
		//ts << "Starting thread number " << phase + 1 << "." << endl;
		t[phase] = thread([&] (const uint64_t _phase, const uint64_t _runs) { this->do_calculate(_phase, _runs); }, phase, runs);
	}

	ts << "All threads are finished." << endl;

	join_all(t);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
		exit(1);

	uint64_t threadc = (uint64_t)atoi(argv[1]);
	uint64_t runc = (uint64_t)atoi(argv[2]);
	cout << "Using " << threadc << " threads!" << endl;

	pi p(100000, threadc);

	p.calculate(runc);

	//cout << p;

	return 0;
}
