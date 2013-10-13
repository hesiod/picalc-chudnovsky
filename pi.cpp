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
#include <ncurses.h>

using namespace std;

void pi::do_calculate(const unsigned int phase, const unsigned int runs)
{
	thread_local mpf_class local_sum(0, precision);
	thread_local mpf_class n(phase + 1, precision);
	if (phase == 0)
	{
		ts.lock();
		print_percent(0, 100);
		chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
		for (thread_local unsigned int i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
			print_percent(i, runs);
			chrono::time_point<chrono::high_resolution_clock> middle = chrono::high_resolution_clock::now();
			chrono::duration<double> runs_took = middle - start;
			double rel = 1.0f / ((double)i / (double)runs);
			chrono::duration<double> remaining = (runs_took * rel) - runs_took;
			cout.unsetf(ios_base::floatfield);
			initscr();
			ts << "\tEstimated remaining time: " << setprecision(2) << remaining.count() << "s";
			clrtoeol();
			endwin();
			// i / runs = 0.1
			// runs_took = 1s
		}
		chrono::time_point<chrono::high_resolution_clock> end = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed_seconds = end - start;
		//time_t end_time = chrono::system_clock::to_time_t(end);
		ts.lprintf("\n");
		ts << "Calculation took " << fixed << setprecision(10) << elapsed_seconds.count() << "s" << endl;
		(threads >= 10) ? (ts.lprintf("\r 0/%2u threads are finished.", threads)) : \
			(ts.lprintf("\r0/%u threads are finished.", threads));
		ts.unlock();
	}
	else
	{
		for (thread_local unsigned int i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
		}
	}
	add_sum(local_sum);
	++finished_threads;
	(threads >= 10) ? (ts.lprintf("\r%2u/%2u threads are finished.", finished_threads.load(), threads)) : \
		(ts.lprintf("\r%u/%u threads are finished.", finished_threads.load(), threads));
}

void pi::calculate(const unsigned int runs)
{
	vector<thread> t (threads);

	for (unsigned int phase = 0; phase < threads; phase++)
	{
		//ts << "Starting thread number " << phase + 1 << "." << endl;
		t[phase] = thread([&] (const unsigned int _phase, const unsigned int _runs) { this->do_calculate(_phase, _runs); }, phase, runs);
	}

	join_all(t);

	(threads >= 10) ? (ts.lprintf("\r%2u/%2u threads are finished.\n", finished_threads.load(), threads)) : \
		(ts.lprintf("\r%u/%u threads are finished.\n", finished_threads.load(), threads));

	ts << "All threads are finished." << endl;
}
