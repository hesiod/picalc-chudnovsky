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
		unsigned long runs;
		unsigned long precision;
		unsigned long phase;
		unsigned long threads;
		run_info(unsigned long r, unsigned long pr, unsigned long ph, unsigned long t) \
			: runs(r), precision(pr), phase(ph), threads(t)
		{ }
	};

	template <typename RT>
	class calculator
	{
	private:
		calculator();
	public:
		calculator(run_info) { }
		//virtual bool try_get() const = 0;
		//virtual RT get() const = 0;
		virtual std::future<mpf_class> get_future() = 0;
		virtual double get_progress() const = 0;
		virtual ~calculator() { }
	};

	class euler final : calculator<mpf_class>
	{
	private:
		const unsigned int dividend = 1;
		run_info info;
		mpf_class sum;
		unsigned long i;
		std::mutex m;
		void add_sum(const mpf_class origin)
		{
			std::unique_lock<std::mutex> lock (m);
			sum += origin;
		}
		std::promise<mpf_class> pi;
		mpf_class actual() const noexcept
		{
			return sqrt(6 * sum);
		}
		mpf_class do_calculate(unsigned long phase)
		{
			mpz_class n(phase + 1);
			mpf_class ret(0, info.precision);
			for (; i <= info.runs; i++)
			{
				ret += (dividend / pow(n, 2));
				n += info.threads;
				//print_percent(i, info.runs);
			}
			return ret;
		}
	public:
		std::future<mpf_class> get_future()
		{
			return pi.get_future();
		}
		double get_progress() const
		{
			double rel = (double)i / (double)info.runs;
			return rel;
		}
		void calculate()
		{
			std::vector<std::thread> t (info.threads);

			for (; info.phase < info.threads; info.phase++)
			{
				t[info.phase] = std::thread(
					[&] ()
					{
						add_sum(do_calculate(info.phase));
					},
					info);
			}

			join_all(t);

			pi.set_value(sqrt(sum * 6));
		}
		euler(const run_info r) : calculator(r), info(r), i(0)
		{
		}
		~euler()
		{
		}
	};

	class pi
	{
	private:
		std::mutex m;
		void do_calculate(const unsigned long phase, const unsigned long runs);
		unsigned long precision;
		unsigned long threads;
		const unsigned int dividend = 1;
		std::atomic<unsigned long> finished_threads;
	public:
		std::ostream& operator<<(std::ostream& out) const noexcept
		{
			std::terminate();
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual().get_mpf_t());
			out << outstr << "e" << exponent;
			return out;
		}
		friend std::ostream& operator<<(std::ostream& out, const pi& p) noexcept
		{
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, p.actual().get_mpf_t());
			out << outstr << "e" << exponent;
			return out;
		}
		size_t digits() const noexcept
		{
			mp_exp_t exponent;
			std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual().get_mpf_t());
			return outstr.size();
		}
		pi() : precision(100000), threads(1), finished_threads(0)
		{
		}
		pi(mp_bitcnt_t _precision) : precision(_precision), threads(1), finished_threads(0)
		{
		}
		pi(mp_bitcnt_t _precision, unsigned long thread_count) : precision(_precision), threads(thread_count), finished_threads(0)
		{
			if (threads < 1)
				throw std::invalid_argument("At least one thread is needed!");
		}
		~pi()
		{
		}
		void calculate(unsigned long runs);
	};
};

#endif
