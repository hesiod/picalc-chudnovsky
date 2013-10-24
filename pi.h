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
#include <stdexcept>
#include <gmp.h>
#include <gmpxx.h>
#include "util.h"
#include "tsio.h"

namespace picalc
{
	struct run_info
	{
		mp_bitcnt_t precision;
		unsigned long threads;
		run_info() \
			: precision(0), threads(0)
		{ }
		run_info(unsigned long p, unsigned long t) \
			: precision(p), threads(t)
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
		virtual run_info	get_run_info() const			= 0;
	};

	class euler : public calculator
	{
	private:
		run_info info;
		mpf_class sum;
		unsigned long i;
		unsigned long gruns;
		//std::atomic<unsigned long> finished_threads;
		std::vector<std::thread> t;

		std::mutex m;
		void add_sum(const mpf_class origin)
		{
			std::unique_lock<std::mutex> lock (m);
			sum += origin;
			std::cout << "added...." << std::endl;
		}
		const unsigned int dividend = 1;
		void do_calculate(const run_info in, unsigned long phase)
		{
			phase--;
			phase++;
			thread_local mpz_class n(phase + dividend);
			thread_local mpf_class ret(0, in.precision);
			//std::cout << "I have phase " << phase << " and prec " << info.precision << " and threads " << info.threads << std::endl;
			for (thread_local unsigned long j = 0; j <= gruns; j++)
			{
				ret += (dividend / (n * n));
				n += in.threads;
				if (phase == 0)
					i++;
			}
			//add_sum(ret);
			thread_local std::unique_lock<std::mutex> lock (m);
			sum += ret;
		}
	public:
		run_info get_run_info() const
		{
			return info;
		}
		const mpf_class actual()
		{
			join_all(t);
			mpf_class tmp(sum, info.precision);
			tmp *= 6;
			tmp = sqrt(tmp);
			return tmp;
		}
		double get_progress() const
		{
			double rel = (double)i / (double)gruns;
			return rel;
		}
		void calculate(const unsigned long runs)
		{
			gruns = runs;
			/*t[0] = std::thread(
				[&] ()
				{
					do_calculate_verbose(info, 0);
				});*/
			
			for (unsigned long phase = 0; phase < info.threads; phase++)
			{
				t[phase] = std::thread(
				[&] ()
				{
					do_calculate(info, phase);
					//++finished_threads;
					//(info.threads >= 10) ? \
					//(ts.lprintf("\r%2u/%2u threads are finished.",	finished_threads.load(), info.threads)) : \
					//(ts.lprintf("\r%u/%u threads are finished.", 	finished_threads.load(), info.threads));
				});
				//std::this_thread::sleep_for((std::chrono::milliseconds)100);
			}

			join_all(t);

			//(info.threads >= 10) ? (ts.lprintf("\r%2u/%2u threads are finished.\n", finished_threads.load(), info.threads)) : \
			//	(ts.lprintf("\r%u/%u threads are finished.\n", finished_threads.load(), info.threads));
		}
		euler(const run_info r) : calculator(r), info(r), sum(0, r.precision), i(0), gruns(0), t(info.threads) //, finished_threads(0)
		{
		}
		~euler()
		{
		}
	};

	class pi
	{
	private:
		calculator* calc;
		mpf_class actual;
		bool finished;
	public:
		std::ostream& operator<<(std::ostream& out) const noexcept
		{
			if (finished == false)
				throw std::logic_error("Run picalc::calculate() before trying to output pi!");
			std::terminate();
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual.get_mpf_t());
			out << outstr << "e" << exponent;
			return out;
		}
		friend std::ostream& operator<<(std::ostream& out, const pi& p) noexcept
		{
			if (p.finished == false)
				throw std::logic_error("Run picalc::calculate() before trying to output pi!");
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, p.actual.get_mpf_t());
			out << outstr << "e" << exponent;
			return out;
		}
		size_t digits() const noexcept
		{
			if (finished == false)
				throw std::logic_error("Run picalc::calculate() before trying to output pi!");
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual.get_mpf_t());
			return outstr.size();
		}
		pi(calculator* _calc) : calc(_calc), actual(0, calc->get_run_info().precision), finished(false)
		{
		}
		~pi()
		{
		}
		void calculate(const unsigned long runs);
	};
};

#endif
