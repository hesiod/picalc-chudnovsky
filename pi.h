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

class pi
{
private:
	void add_sum(const mpf_class origin)
	{
		static std::mutex m;
		m.lock();
		sum += origin;
		m.unlock();
	}
	void do_calculate(const unsigned phase, const unsigned runs);
	unsigned int precision;
	mpf_class sum;
	mpf_class actual;
	unsigned int threads;
	const unsigned dividend = 1;
	std::atomic<unsigned int> finished_threads;
public:
	std::ostream& operator<<(std::ostream& out)
	{
		(mpf_class)actual = sqrt(sum * 6);
		mp_exp_t exponent;
		std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual.get_mpf_t());
		out << outstr << "e" << exponent;
		return out;
	}
	friend std::ostream& operator<<(std::ostream& out, const pi& p)
	{
		(mpf_class)p.actual = sqrt(p.sum * 6);
		mp_exp_t exponent;
		std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, p.actual.get_mpf_t());
		out << outstr << "e" << exponent;
		return out;
	}
	size_t digits()
	{
		(mpf_class)actual = sqrt(sum * 6);
		mp_exp_t exponent;
		std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual.get_mpf_t());
		return outstr.size();
	}
	pi() : precision(100000), sum(0, precision), threads(1), finished_threads(0)
	{
	}
	pi(mp_bitcnt_t _precision) : precision(_precision), sum(0, precision), threads(1), finished_threads(0)
	{
	}
	pi(mp_bitcnt_t _precision, unsigned thread_count) : precision(_precision), sum(0, _precision), threads(thread_count), finished_threads(0)
	{
		if (threads < 1)
			throw std::invalid_argument("At least one thread is needed!");
	}
	~pi()
	{
	}
	void calculate(unsigned runs);
};

#endif
