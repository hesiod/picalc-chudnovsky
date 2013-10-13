/*
 * pi.cpp
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <gmp.h>
#include <gmpxx.h>
#include "pi.h"
#include "tsio.h"
#include "util.h"

using namespace std;

void pi::do_calculate(const unsigned phase, const unsigned runs)
{
	thread_local mpf_class local_sum(0, precision);
	thread_local mpf_class n(phase + 1, precision);
	if (phase == 0)
	{
		ts.lock();
		print_percent(0, 100);
		chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
		for (thread_local unsigned i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
			print_percent(i, runs);
		}
		chrono::time_point<chrono::high_resolution_clock> end = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed_seconds = end-start;
		//time_t end_time = chrono::system_clock::to_time_t(end);
		ts.lprintf("\n");
		ts << "Calculation took " << fixed << setprecision(10) << elapsed_seconds.count() << "s" << endl;
		ts.unlock();
	}
	else
	{
		for (thread_local unsigned i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
		}
	}
	add_sum(local_sum);
	ts.lprintf("Thread number %llu \thas finished!\n", phase + 1);
}

void pi::calculate(const unsigned runs)
{
	vector<thread> t (threads);

	for (unsigned phase = 0; phase < threads; phase++)
	{
		//ts << "Starting thread number " << phase + 1 << "." << endl;
		t[phase] = thread([&] (const unsigned _phase, const unsigned _runs) { this->do_calculate(_phase, _runs); }, phase, runs);
	}

	join_all(t);

	ts << "All threads are finished." << endl;
}
