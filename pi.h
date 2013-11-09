/*
 * pi.h
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#ifndef PI_H
#define PI_H

#include <atomic>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdarg>
#include <cmath>
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
			/*mpfr::mpreal a = mpfr::pow(-1.0, k) *				\
			(mpfr::fac_ui(6.0 * k) * (13591409.0 + (545140134.0 * k)))	\
			/								\
			(mpfr::fac_ui(3.0 * k) * mpfr::pow(mpfr::fac_ui(k), 3.0) *	\
			mpfr::pow(640320.0, 3.0 * k));*/

			mpfr::mpreal a = mpfr::pow(-1.0, k) *				\
			mpfr::fac_ui(6.0 * k) /						\
			(mpfr::fac_ui(3.0 * k) * mpfr::pow(mpfr::fac_ui(k), 3.0))	\
			*								\
			((13591409.0 + (545140134.0 * k)) / mpfr::pow(640320.0, 3.0 * k));

			return a;
		}
		static inline const mpfr::mpreal pi_for(const mpfr::mpreal sum)
		{
			return mpfr::pow((mpfr::sqrt(10005.0) / 4270934400.0) * sum, (-1.0));
		}
		static inline const mpfr::mpreal exp_mod(const mpfr::mpreal b, mpfr::mpreal n, const mpfr::mpreal k)
		{
			mpfr::mpreal r = 1.0;
			mpfr::mpreal t = 0.0;
			unsigned int i;
			for (i = 0; t <= n; i++)
			{
				t = mpfr::pow(2.0, i);
			}
			i--;
			t = mpfr::pow(2.0, i);
			while (1)
			{
				if (n >= t)
				{
					r = mpfr::mod(b * r, k);
					n -= t;
				}
				t /= 2.0;
				if (t >= 1.0)
				{
					r = mpfr::mod(mpfr::pow(r, 2.0), k);
				}
				else
					break;
			}
			return r;
		}
		static inline const mpfr::mpreal sum_for(const mpfr::mpreal n, const mpfr::mpreal j)
		{
			mpfr::mpreal sum;

			for (mpfr::mpreal k = 0; k <= n; k++)
			{
				//sum += mpfr::mod(mpfr::pow(16.0, n - k), (8.0 * k) + j) / ((8.0 * k) + j);
				sum += exp_mod(16.0, n - k, (8.0 * k) + j) / ((8.0 * k) + j);
			}
			for (mpfr::mpreal k = n + 1.0; k <= n + 32.0; k++)
			{
				sum += mpfr::pow(16.0, n - k) / (8.0 * k + j);
			}

			return sum;
		}
		static inline const mpfr::mpreal bbp_for(const mpfr::mpreal n)
		{
			mpfr::mpreal a =	(4.0 * sum_for(n, 1)) \
					-	(2.0 * sum_for(n, 4)) \
					-	(1.0 * sum_for(n, 5)) \
					-	(1.0 * sum_for(n, 6));
			a = mpfr::mod(a, 1.0);
			return a;
		}
		std::mutex m;
		run_info info;
		unsigned int threadc;
		std::vector<std::thread> t;

		static inline unsigned long long hex_at(mpfr::mpreal r, const unsigned int k, const unsigned int len = 8)
		{
			if (len > 15 && std::numeric_limits<signed long long>::max() == pow(2, 64) / 2)
			{
				throw std::out_of_range("The limit for hex_at is  std::numeric_limits<signed long long>::max()  , usually (2 ^ 64) / 2. Please do not use lengths greater than 15.");
			}

			r -= mpfr::trunc(r);
			mpfr::mpreal tmp = r;
			std::string result;
			for (;;)
			{
				r = ldexp(r, 4); // *= 16
				result.append(to_string<unsigned int>((unsigned long)mpfr::trunc(r).toULong(), std::hex));
				r -= mpfr::trunc(r);
				if (r == 0.0)
					break;
			}

			std::cout.precision(8);
			//std::cout << "a ### " << std::hex << result << std::endl;
			//std::cout << "b ### " << result.substr(k, len) << " at index " << k << " for len of " << len << std::endl;

			unsigned long long ret;
			try
			{
				ret = std::stoll(result.substr(k, len), nullptr, 16);
			}
			catch (std::out_of_range oor)
			{
				std::cerr << "The limit for hex_at is  std::numeric_limits<unsigned long long>::max()  , usually (2 ^ 64) / 2." << std::endl;
				throw;
			}
			return ret;
		}
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
						for (unsigned int k = phase; k < runs; k += threadc)
						{
							mpfr::mpreal tmp = for_k(k);
							std::unique_lock<std::mutex> lock (m);
							//std::cout << tmp << std::endl;
							sum += tmp;
						}
					}, ph);
			}

			join_all(t);
			mpfr::mpreal pi = pi_for(sum);

			std::cout << std::hex;

			for (unsigned int k = 0; k < 120; k += 1)
			{
				std::cout << hex_at(bbp_for(k), 0, 1);
			}
			std::cout << std::endl;

			for (unsigned int k = 0; k < 120; k += 1)
			{
				std::cout << hex_at(pi, k, 1);
			}

			std::cout << std::endl << "##############" << std::endl;

			unsigned long correct_digits = 0;
			for (unsigned int k = 0; k < pi.toString().size(); k++)
			{
				//std::cout << std::dec << "k == " << k << std::endl;
				//std::cout << std::hex << hex_at(bbp_for(k), 0, 1) << std::endl << std::hex << hex_at(pi, k, 1) << std::endl;
				if (hex_at(bbp_for(k), 0, 1) == hex_at(pi, k, 1))
					correct_digits++;
				else
					break;
			}
			std::cout.precision(14);
		//	std::cout << std::hex << (unsigned long int)(bbp_for(0).toLDouble() * 72057594037927936.0) << std::dec << std::endl;
			std::cout << std::dec << "Correct digits: " << correct_digits << std::endl;
			std::cout.precision(1024);
			std::stringstream ss;
			ss.precision(1024);
			ss << pi;
			std::cout << ss.str() << std::endl;
			std::cout << "Buffer Length: " << ss.str().size() << std::endl;
		//	std::cout << std::hex << hex_at(pi, 0) << std::endl;
		//	printf("%014lx\n", (unsigned long int)(bbp_for(0).toLDouble() * 72057594037927936.0));
		}
		chudnovsky(const run_info r) : info(r), threadc(r.threads), t(threadc)
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
