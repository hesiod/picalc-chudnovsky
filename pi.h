/*
 * pi.h
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#ifndef PI_H
#define PI_H

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
	unsigned precision;
	mpf_class sum;
	unsigned threads;
	const unsigned dividend = 1;
public:
	std::ostream& operator<<(std::ostream& out)
	{
		(mpf_class)sum = sqrt(sum * 6);
		mp_exp_t exponent;
		std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, sum.get_mpf_t());
		out << outstr << "e" << exponent << std::endl;
		return out;
	}
	pi() : precision(100000), sum(0, precision), threads(1)
	{
	}
	pi(mp_bitcnt_t _precision) : precision(_precision), sum(0, precision), threads(1)
	{
	}
	pi(mp_bitcnt_t _precision, unsigned thread_count) : precision(_precision), sum(0, _precision), threads(thread_count)
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
