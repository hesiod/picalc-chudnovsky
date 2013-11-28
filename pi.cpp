/*
	picalc-chudnovsky is an arbitrary precision pi calculator (Chudnovsky algorithm) using C++
	and the GNU MPFR library.
	Copyright (C) 2013 Tobias Markus

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see [http://www.gnu.org/licenses/].
*/

#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <future>
#include "pi.h"
#include "tsio.h"
#include "util.h"
#include <cstdlib>
#include <ncurses.h>

using namespace std;
using namespace picalc;

/*void pi::do_calculate(const unsigned long phase, const unsigned long runs)
{
	mpf_class local_sum(0, precision);
	mpz_class n(phase + 1);
	if (phase == 0)
	{
		cout.lock();

		print_percent(0, 100);
		chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
		for (thread_local unsigned long i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
			if ((i % 1000) > 0|| (i < 10000))
				continue;

			chrono::time_point<chrono::high_resolution_clock> middle = chrono::high_resolution_clock::now();
			chrono::duration<double> runs_took = middle - start;
			double rel = 1.0f / ((double)i / (double)runs);
			chrono::seconds remaining_s = chrono::duration_cast<chrono::seconds>((runs_took * rel) - runs_took);
			chrono::minutes remaining_m = chrono::duration_cast<chrono::minutes>(remaining_s);
			chrono::hours   remaining_h = chrono::duration_cast<chrono::hours>  (remaining_s);
			remaining_s -= remaining_h + remaining_m;
			remaining_m -= remaining_h;

			clear_line();
			cout.lprintf("\r");
			print_percent(i, runs);
			cout << fixed << setprecision(1) << "\tEstimated remaining time: " << remaining_h.count() << "h " << remaining_m.count() << "m " << remaining_s.count() << "s";
		}
		chrono::time_point<chrono::high_resolution_clock> end = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed_seconds = end - start;
		cout.lprintf("\n");
		cout << "Calculation took " << fixed << setprecision(10) << elapsed_seconds.count() << "s" << endl;
		(threads >= 10) ? (cout.lprintf("\r 0/%2u threads are finished.", threads)) : \
			(cout.lprintf("\r0/%u threads are finished.", threads));

		cout.unlock();
	}
	else
	{
		for (thread_local unsigned long i = 0; i <= runs; i++)
		{
			local_sum += (dividend / pow(n, 2));
			n += threads;
		}
	}
	add_sum(local_sum);
	++finished_threads;
	(threads >= 10) ? (cout.lprintf("\r%2u/%2u threads are finished.", finished_threads.load(), threads)) : \
		(cout.lprintf("\r%u/%u threads are finished.", finished_threads.load(), threads));
}*/
