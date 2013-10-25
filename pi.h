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
#include "mpreal.h"
#include "util.h"
#include "tsio.h"

namespace picalc
{
	struct run_info
	{
		size_t precision;
		unsigned int threads;
		run_info() \
			: precision(0), threads(0)
		{ }
		run_info(unsigned int p, unsigned int t) \
			: precision(p), threads(t)
		{ }
	};	

	class chudnovsky
	{
	private:
#if 0
		static inline const mpfr::mpreal for_k(const unsigned int k)
		{
			// -1 ^ k
			// (6k)!
			// /
			// (k!)^3 * (3k)!
			// *
			// (13591409 + 545140134k)
			// /
			// 640320^3k

			mpfr::mpreal a = mpfr::fac_ui(6 * k)		/ \
		(	pow(mpfr::fac_ui(k), 3) * mpfr::fac_ui(3 * k)	)	;

			mpfr::mpreal b = (13591409 + (545140134 * k))	/ \
		(	pow(640320, mpfr::fac_ui(3 * k))		)	;

			return mpfr::pow((-1), k);
		}
		static inline const mpfr::mpreal pi_for(const mpfr::mpreal sum)
		{
			const mpfr::mpreal c = mpfr::sqrt(10005) / 4270934400;
			return mpfr::pow(c * sum, (-1));
		}
#endif
		static inline const mpfr::mpreal for_k(const unsigned int k)
		{
			// -1 ^ k
			// (6k)!
			// /
			// (k!)^3 * (3k)!
			// *
			// (13591409 + 545140134k)
			// /
			// 640320^3k

			mpfr::mpreal a =						\
		(									\
			mpfr::pow(-1.0f, k) * mpfr::fac_ui(6.0f * k) * 			\
			(13591409.0f + (545140134.0f * k))				\
		)	/								\
		(									\
			mpfr::fac_ui(3.0f * k) * mpfr::pow(mpfr::fac_ui(k), 3.0f) *	\
			mpfr::pow(640320.0f, 3.0f * k + 3.0f / 2.0f)			\
		);
			// + 3.0f / 2.0f
		

			return a;
		}
		static inline const mpfr::mpreal pi_for(const mpfr::mpreal sum)
		{
			//const mpfr::mpreal c = 1 / (mpfr::sqrt(10005.0f) / 426880.0f);
			//return mpfr::pow(c * sum, (-1));
			return mpfr::pow(12 * sum, (-1));
		}
		static inline const mpfr::mpreal sum_for(const mpfr::mpreal n, const mpfr::mpreal l)
		{
			mpfr::mpreal sum;

			for (mpfr::mpreal k = 0; k <= n; k++)
			{
				sum += mpfr::mod(mpfr::pow(16.0f, n - k), (8.0f * k) + l) / \
				((8.0f * k) + l);
			}
			for (mpfr::mpreal k = n + 1; k <= n + 8; k++)
			{
				sum += mpfr::pow(16.0f, n - k) / \
				(8.0f * k + l);
			}

			return sum;
		}
		static inline const mpfr::mpreal bbp_for(const mpfr::mpreal n)
		{
			mpfr::mpreal a =	(4.0f * sum_for(n, 1)) \
					-	(2.0f * sum_for(n, 4)) \
					-	(1.0f * sum_for(n, 5)) \
					-	(1.0f * sum_for(n, 6));
			a = a - mpfr::floor(a);
			return a;
		}
		std::mutex m;
		run_info info;
		unsigned int threadc;
		std::vector<std::thread> t;
	protected:
	public:
		void calculate(const unsigned int runs)
		{
			mpfr::mpreal::set_default_prec(info.precision);
			mpfr::mpreal sum = 0;

			for (unsigned int ph = 0; ph < threadc; ph++)
			{
				t[ph] = std::thread( [&] (unsigned int phase)
					{
						for (unsigned int k = phase; k <= runs; k += threadc)
						{
							const mpfr::mpreal tmp = for_k(k);
							std::unique_lock<std::mutex> lock (m);
							sum += tmp;
						}
					}, ph);
			}
			/*for (unsigned int k = 0; k < 10; k++)
			{
				std::cout << bbp_for(k) << std::endl;
			}*/
			join_all(t);
			mpfr::mpreal pi = pi_for(sum);
			std::cout.precision(256);
			std::cout << pi << std::endl;
		}
		chudnovsky(const run_info r) : info(r), threadc(std::thread::hardware_concurrency()), t(threadc)
		{
		}
		~chudnovsky()
		{
		}
	};

	class pi
	{
	private:
		//bool finished;
	public:
		/*friend std::ostream& operator<<(std::ostream& out, const pi& p) noexcept
		{
			if (p.finished == false)
				throw std::logic_error("Run picalc::calculate() before trying to output pi!");
			mp_exp_t exponent;
			//std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, p.actual.get_mpf_t());
			//out << outstr << "e" << exponent;
			return out;
		}
		size_t digits() const noexcept
		{
			if (finished == false)
				throw std::logic_error("Run picalc::calculate() before trying to output pi!");
			//mp_exp_t exponent;
			//std::string outstr = mpf_get_str(NULL, &exponent, 10, 0, actual.get_mpf_t());
			std::stringbuffer str;
			return outstr.size();
		}*/
		pi(const run_info r, unsigned int runs) //: finished(false)
		{
			chudnovsky ch(r);
			ch.calculate(runs);
		}
		~pi()
		{
		}
		/*void calculate(const unsigned int runs)
		{
			cout << "Calculating..." << endl;
			e.calculate(runs);
			cout << "Constructing future..." << endl;
			std::future<mpf_class> f = std::async(std::launch::async,
				[&] ()
				{
					actual = calc->actual();
					return actual;
				});
			std::thread t(
				[&] ()
				{
					actual = e.actual();
					return actual;
				});
			cout << "Waiting..." << endl;
			double prog;
			do
			{
				prog = calc->get_progress();
				print_percent(prog);
			} while (prog < 1);
			print_percent(1);
			f.wait();*/
			//t.join();
			//cout << endl << "All threads are finished." << endl;
			//finished = true;
		
	};
};

#endif
