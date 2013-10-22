/*
 * pi.h
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#ifndef PI_H
#define PI_H

#include <atomic>
#include <iostream>
#include <cstdarg>
#include <gmp.h>
#include <gmpxx.h>
#include "util.h"
#include "tsio.h"

namespace picalc
{
	struct run_info
	{
		mp_bitcnt_t precision;
		unsigned long phase;
		unsigned long threads;
		run_info() \
			: precision(0),  phase(0),  threads(0)
		{ }
		run_info(unsigned long pr, unsigned long ph, unsigned long t) \
			: precision(pr), phase(ph), threads(t)
		{ }
	};

	class calculator
	{
	private:
		calculator();
	public:
					calculator(run_info)			{ }
		virtual 		~calculator()				{ }
		virtual double		get_progress() const			= 0;
		virtual void		calculate(const unsigned long)		= 0;
		virtual const mpf_class	actual()				= 0;
	};

	class euler : public calculator
	{
	private:
		const unsigned int dividend = 1;
		run_info info;
		mpf_class sum;
		unsigned long i;
		unsigned long runs;
		std::mutex m;
		std::atomic<unsigned long> finished_threads;
		std::vector<std::thread> t;
		void add_sum(const mpf_class origin)
		{
			std::unique_lock<std::mutex> lock (m);
			sum += origin;
		}
		mpf_class do_calculate(const unsigned long phase)
		{
			mpz_class n(phase + 1);
			mpf_class ret(0, info.precision);
			for (; i <= runs; i++)
			{
				ret += (dividend / pow(n, 2));
				n += info.threads;
				//print_percent(i, info.runs);
			}
			return ret;
		}
	public:
		const mpf_class actual()
		{
			join_all(t);
			return sqrt(6 * sum);
		}
		double get_progress() const
		{
			double rel = (double)i / (double)runs;
			return rel;
		}
		void calculate(const unsigned long _runs)
		{
			runs = _runs;
			for (; info.phase < info.threads; info.phase++)
			{
				t.push_back(std::thread(
					[&] ()
					{
						add_sum(do_calculate(info.phase));
						++finished_threads;
						(info.threads >= 10) ? \
						(ts.lprintf("\r%2u/%2u threads are finished.",	finished_threads.load(), info.threads)) : \
						(ts.lprintf("\r%u/%u threads are finished.", 	finished_threads.load(), info.threads));
					}));
			}

			//(info.threads >= 10) ? (ts.lprintf("\r%2u/%2u threads are finished.\n", finished_threads.load(), info.threads)) : \
			//	(ts.lprintf("\r%u/%u threads are finished.\n", finished_threads.load(), info.threads));
		}
		euler(const run_info r) : calculator(r), info(r), i(0), runs(0), finished_threads(0)
		{
		}
		~euler()
		{
		}
	};

	class pi
	{
	private:
		mpf_class actual;
		calculator& calc;
	public:
		std::ostream& operator<<(std::ostream& out) const noexcept
		{
			std::terminate();
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual.get_mpf_t());
			out << outstr << "e" << exponent;
			return out;
		}
		friend std::ostream& operator<<(std::ostream& out, const pi& p) noexcept
		{
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, p.actual.get_mpf_t());
			out << outstr << "e" << exponent;
			return out;
		}
		size_t digits() const noexcept
		{
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual.get_mpf_t());
			return outstr.size();
		}
		pi(calculator& _calc) : calc(_calc)
		{
		}
		~pi()
		{
		}
		void calculate(const unsigned long runs);
	};
};

#endif
