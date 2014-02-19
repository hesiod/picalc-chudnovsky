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

#ifndef PI_H
#define PI_H

#include <algorithm>
#include <atomic>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdarg>
#include <cmath>
#include <stdexcept>
#include <map>
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

	class cache
	{
	private:
		size_t k;
		std::map<unsigned long, mpfr::mpreal> fac;
		std::mutex fac_lock;
		run_info info;
	public:
		/*friend std::ostream& operator<<(std::ostream& out, const cache& p)
		{
		}
		friend std::istream& operator>>(std::istream& out, cache& p)
		{
		}*/
		void fast_factorial(const unsigned long k)
		{
			mpfr::mpreal::set_default_prec(info.precision);
			if (fac.find(k) == fac.end())
			{
				mpfr::mpreal tmp = mpfr::fac_ui(k);
				std::unique_lock<std::mutex> lock (fac_lock);
				fac.insert(std::pair<unsigned long, mpfr::mpreal>(k, tmp));
			}
			if (fac.find(3 * k) == fac.end())
			{
				mpfr::mpreal tmp = mpfr::fac_ui(3 * k);
				std::unique_lock<std::mutex> lock (fac_lock);
				fac.insert(std::pair<unsigned long, mpfr::mpreal>(3 * k, tmp));
			}
			if (fac.find(6 * k) == fac.end())
			{
				mpfr::mpreal tmp = mpfr::fac_ui(6 * k);
				std::unique_lock<std::mutex> lock (fac_lock);
				fac.insert(std::pair<unsigned long, mpfr::mpreal>(6 * k, tmp));
			}
		}
		void caching_precalculate(const size_t num)
		{
			fac.insert(std::pair<unsigned long, mpfr::mpreal>(1, mpfr::mpreal(1)));
			for (unsigned long i = 2; i <= 6 * num; i++)
			{
				fac.insert(std::pair<unsigned long, mpfr::mpreal>(i, i * fac.at(i - 1)));
			}
		}
		void precalculate(size_t num)
		{
			mpfr::mpreal::set_default_prec(info.precision);

			num += k;
			std::vector<std::thread> t (info.threads);

			auto start = std::chrono::high_resolution_clock::now();

			for (unsigned long phase = 0; phase < info.threads; phase++)
			{
				t[phase] = std::thread( [&] (const unsigned long ph)
				{
					if (ph == 0)
					{
						for (unsigned long j = ph; j <= num; j += info.threads)
						{
							fast_factorial(j);

							auto middle = std::chrono::high_resolution_clock::now();
							std::chrono::duration<double> runs_took = middle - start;
							double rel = 1.0f / ((double)j / (double)num);
							std::chrono::seconds remaining_s = std::chrono::duration_cast<std::chrono::seconds>((runs_took * rel) - runs_took);
							std::chrono::minutes remaining_m = std::chrono::duration_cast<std::chrono::minutes>(remaining_s);
							std::chrono::hours   remaining_h = std::chrono::duration_cast<std::chrono::hours>  (remaining_s);
							remaining_s -= remaining_h + remaining_m;
							remaining_m -= remaining_h;
							std::chrono::duration<double> elapsed_seconds = middle - start;

							clear_line();
							print_percent(j, num);
							std::cout.precision(1);
							std::cout << std::fixed << "Elapsed: " << elapsed_seconds.count() << "s " << " Runs: " << j << " out of " << num;
							std::cout.flush();
						}
						print_percent(1, 1);
					}
					else
					{
						for (unsigned int j = ph; j <= num; j += info.threads)
						{
							fast_factorial(j);
						}
					}

					mpfr_free_cache();
				}, phase );
			}

			join_all(t);

			std::cout << std::endl;

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			std::cout.precision(8);
			std::cout << "Calculation took " << std::fixed << elapsed_seconds.count() << "s" << std::endl;

			k = num;
		}
		cache(run_info r) : k(0), info(r)
		{
			mpfr::mpreal::set_default_prec(info.precision);
			std::cout << std::dec << " __ Default == " << mpfr::mpreal::get_default_prec() << " __ " << std::endl;
			fac.insert(std::pair<unsigned long, mpfr::mpreal>(0, 1));
		}
		~cache()
		{
			mpfr_free_cache();
		}
		inline const mpfr::mpreal lookup(const unsigned long k) const
		{
			mpfr::mpreal::set_default_prec(info.precision);
			return fac.at(k);
		}
	};

	class chudnovsky
	{
	private:
		std::mutex m;
		run_info info;
		std::atomic<unsigned long> j;
		cache c;

		long better_exp_mod(long base, long exponent, const long modulus) const
		{
			if (base < 1 || exponent < 0 || modulus < 1)
				throw std::invalid_argument("something went wrong");

			long result = 1;
			while (exponent > 0)
			{
				if ((exponent % 2) == 1)
				{
					result = (result * base) % modulus;
				}
				base = (base * base) % modulus;
				exponent = floor(exponent / 2.0);
			}
			return std::move(result);
		}
		double exp_mod(const long b, long n, const long k) const
		{
			double r = 1.0;
			double t = 0.0;
			unsigned int i;
			for (i = 0; t <= n; i++)
			{
				t = pow(2.0, i);
			}
			i--;
			t = pow(2.0, i);
			while (1)
			{
				if (n >= t)
				{
					//r = mpfr::mod(b * r, k);
					r = fmod(b * r, k);
					n = n - t;
				}
				t /= 2.0;
				if (t >= 1.0)
				{
					//r = mpfr::mod(mpfr::pow(r, 2.0), k);
					r = fmod(pow(r, 2.0), k);
				}
				else
					break;
			}
			return r;
		}
		double sum_for(const long n, const long j) const
		{
			double sum = 0;
			//std::cout << "n " << n << " j " << j << std::endl;

			for (long k = 0; k <= n; k++)
			{
				//sum += mpfr::mod(mpfr::pow(16, n - k), (8 * k) + j) / ((8 * k) + j);
				//sum += exp_mod(16.0, n - k, (8.0 * k) + j) / ((8.0 * k) + j);
				sum += better_exp_mod(16, n - k, (8.0 * k) + j) / ((8.0 * k) + j);
				//std::cout << exp_mod(16.0, n - k, (8.0 * k) + j) / ((8.0 * k) + j) << std::endl;
			}
			//std::cout << "sum == " << sum << std::endl;
			for (long k = n + 1; k <= n + 8; k++)
			{
				sum +=  pow(16.0, n - k) / (8.0 * k + j);
			//std::cout << "pow == " << pow(16.0, n - k) << " n - k == " << (signed long)(n - k) << std::endl;
			}
			//std::cout << "sum == " << sum << std::endl;

			return sum;
		}
		double bbp_for(const long n) const
		{
			double		a =	(4.0 * sum_for(n, 1.0)) \
					-	(2.0 * sum_for(n, 4.0)) \
					-	(1.0 * sum_for(n, 5.0)) \
					-	(1.0 * sum_for(n, 6.0));
			a -= floor(a);
			//std::cout << "a == " << a << std::endl;
			return a;
		}
		static inline unsigned long long hex_at(mpfr::mpreal r, const unsigned int k, const unsigned int len = 8)
		{
			if (len > 15 && std::numeric_limits<signed long long>::max() == pow(2, 64) / 2)
			{
				throw std::out_of_range("The limit for hex_at is  std::numeric_limits<signed long long>::max()  , usually (2 ^ 64) / 2. Please do not use lengths greater than 15.");
			}
			//std::cout << "__ 1 __" << std::hex << std::endl;

			//unsigned

			r -= mpfr::trunc(r);
			mpfr::mpreal tmp = r;
			std::string result;
			for (;;)
			{
				r *= 16;
				//r = ldexp(r, 4); // *= 16
				result.append(to_string<unsigned int>((unsigned long)mpfr::trunc(r).toULong(), std::hex));
				r -= mpfr::trunc(r);
				if (r == 0.0)
					break;
			}
			//std::cout << "__ 3 __" << std::hex << std::endl;

			//std::cout.precision(8);
			//std::cout << "__ 1 __" << std::hex << std::endl;
			//std::cout << "a ### " << std::hex << result << std::endl;
			//std::cout << "__ 2 __" << std::hex << std::endl;
			//std::cout << "b ### " << std::dec << " at index " << k << " for len of " << len << std::endl;
			//std::cout << "__ 3 __" << std::hex << std::endl;
			//std::cout << "c ### " << result.substr(k, len) << " at index " << k << " for len of " << len << std::endl;
			//std::cout << "__ 4 __" << std::hex << std::endl;

			//std::cout << "__ 3 __" << std::hex << std::endl;
			unsigned long long ret;
			try
			{
			//	std::cout << std::dec << "k == " << k << " size == " << result.size() << std::endl;
				std::string str = result.substr(k, len);
			//std::cout << "__ 4 __" << std::hex << std::endl;
				ret = std::stoll(str, nullptr, 16);
			}
			catch (...) // (std::out_of_range& oor)
			{
				std::cerr << std::endl << "##############" << std::endl;
				std::cerr << "Fatal error in program logic." << std::endl;
				std::cerr << "The limit for hex_at is  >>>  std::numeric_limits<unsigned long long>::max()  <<<  , usually (2 ^ 64) / 2." << std::endl;
				std::cerr << "Debug output (If you see this, create a debug report at Github) ### <" << result.substr(k, len) << "> at index <" << k << "> for len of <" << len << "> while result size is <" << result.size() << ">" << std::endl;
				throw;
			}
			return ret;
		}
		inline mpfr::mpreal for_k(const unsigned long k)
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

			mpfr::mpreal a;
			mpfr::mpreal::set_default_prec(info.precision);

			c.fast_factorial(k);
			try
			{
				a = mpfr::pow(-1.0, k) *					\
				(c.lookup(6 * k)	/					\
				(c.lookup(3 * k) * mpfr::pow(c.lookup(k), 3.0)))		\
				*								\
				(((k * 545140134.0) + 13591409.0) / mpfr::pow(640320.0, 3.0 * k));
			}
			catch (std::out_of_range& oor)
			{
				std::cerr << std::endl << "##############" << std::endl;
				std::cerr << "Fatal error in program logic." << std::endl;
				std::cerr << "Debug output (If you see this, create a debug report at Github) ### index is <" << k << ">" << std::endl;
				throw;
			}

			return std::move(a);
		}
		static inline mpfr::mpreal pi_for(mpfr::mpreal sum)
		{
			const mpfr::mpreal c = mpfr::sqrt(10005.0) / 4270934400.0;
			sum *= c;
			sum = pow(sum, -1.0);
			return sum;
		}
	protected:
	public:
		void calculate(const unsigned int runs, const std::string verification_mode, unsigned long digits)
		{
			mpfr::mpreal::set_default_prec(info.precision);
			mpfr::mpreal sum = 0;

			std::cout << std::dec << " __ Default == " << mpfr::mpreal::get_default_prec() << " __ " << std::endl;

// Log(151931373056000) / Log(10) = 14.181647462725477655...

			std::cout << std::endl << "##############" << std::endl << "Precalculating..." << std::endl;

			c.caching_precalculate(runs);

			std::cout << "##############" << std::endl;

			auto start = std::chrono::high_resolution_clock::now();

			std::vector<std::thread> t (info.threads);
			for (unsigned int phase = 0; phase < info.threads; phase++)
			{
				t[phase] = std::thread( [&] (unsigned int ph)
				{
					if (ph == 0)
					{
						for (unsigned int k = ph; k <= runs; k += info.threads)
						{
							mpfr::mpreal tmp = for_k(k);

							auto middle = std::chrono::high_resolution_clock::now();
							std::chrono::duration<double> runs_took = middle - start;
							double rel = 1.0f / ((double)k / (double)runs);
							std::chrono::seconds remaining_s = std::chrono::duration_cast<std::chrono::seconds>((runs_took * rel) - runs_took);
							std::chrono::minutes remaining_m = std::chrono::duration_cast<std::chrono::minutes>(remaining_s);
							std::chrono::hours   remaining_h = std::chrono::duration_cast<std::chrono::hours>  (remaining_s);
							remaining_s -= remaining_h + remaining_m;
							remaining_m -= remaining_h;
							std::chrono::duration<double> elapsed_seconds = middle - start;

							clear_line();
							print_percent(k, runs);
							std::cout.precision(1);
							//<< "  Estimated remaining: " << remaining_h.count() << "h " << remaining_m.count() << "m " << remaining_s.count() << "s
							std::cout << std::fixed << "Elapsed: " << elapsed_seconds.count() << "s " << " Runs: " << k << " out of " << runs;
							std::cout.flush();

							std::unique_lock<std::mutex> lock (m);
							auto add_start = std::chrono::high_resolution_clock::now();
							sum += tmp;
							auto add_end = std::chrono::high_resolution_clock::now();
							lock.unlock();

							std::chrono::duration<double> add_elapsed_seconds = add_end - add_start;
							std::cout.precision(10);
							std::cout << " Adding: " << add_elapsed_seconds.count() << "s\r";
							std::cout.flush();
						}
						print_percent(1, 1);
					}
					else
					{
						for (unsigned int k = ph; k <= runs; k += info.threads)
						{
							mpfr::mpreal tmp = for_k(k);
							std::unique_lock<std::mutex> lock (m);
							sum += tmp;
						}
					}

					mpfr_free_cache();
				}, phase );
			}

			join_all(t);

			std::cout << std::endl;

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			std::cout.precision(8);
			std::cout << "Calculation took " << std::fixed << elapsed_seconds.count() << "s" << std::endl;

			const mpfr::mpreal pi = pi_for(sum);

			std::cout << std::hex;

			/*for (unsigned int k = 0; k < 120; k += 1)
			{
				std::cout << hex_at(bbp_for(k), 0, 1);
			}
			std::cout << std::endl;

			for (unsigned int k = 0; k < 120; k += 1)
			{
				std::cout << hex_at(pi, k, 1);
			}*/

			std::cout << "##############" << std::endl;
(void)digits;
			if (verification_mode == "full")
			{
				unsigned long correct_digits = 0;
				std::string tmp = pi.toString().substr(2);
				unsigned long sz = 1 + (mpfr::log2(mpfr::mpreal(tmp)).toULong() / 4);
				for (unsigned int k = 0; k < sz - 1; k += 1)
				{
					if (hex_at(bbp_for(k), 0, 1) == hex_at(pi, k, 1))
						correct_digits += 1;
					else
						break;
				}
				std::cout.precision(8);
				std::cout << std::dec << "Correct (hexadecimal) digits: " << correct_digits << std::endl;
			}
			else if (verification_mode == "normal")
			{
				constexpr unsigned int parallel = 4;
				unsigned long incorrect_digits = 0;
				std::string tmp = pi.toString().substr(2);
				unsigned long sz = 1 + (mpfr::log2(mpfr::mpreal(tmp)).toULong() / 4) - 2;
				sz -= sz % parallel;
				std::cout << std::dec << "sz == " << sz << std::endl;
				for (unsigned long k = sz; k > 0; k -= parallel)
				{
					std::cout << std::dec << "k == " << k << " ### " << std::hex << hex_at(bbp_for(k), 0, parallel) << " ### " << hex_at(pi, k, parallel) << std::endl;
					if (hex_at(bbp_for(k), 0, parallel) != hex_at(pi, k, parallel))
						incorrect_digits += parallel;
					else
						break;
				}
				//incorrect_digits -= pi.toString().size();
				std::cout << std::dec << "Incorrect digits (counted from the end): " << incorrect_digits << std::endl;
				std::cout << "Buffer Length: " << pi.toString().size() << std::endl;
				std::cout.precision(4);
				std::cout << std::dec << std::fixed << "That are " << (double)incorrect_digits / (double)pi.toString().size() * 100.0 << "% of all digits." << std::endl;
			}
			// Do nothing if verification_mode is "none"

			std::stringstream ss;
			ss.precision(1024);
			ss << pi;

			//std::cout.precision(1024);
			std::cout << ss.str() << std::endl;


			mpfr_free_cache();
		}
		chudnovsky(const run_info r) : info(r), j(0), c(info)
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
		pi(const run_info r, unsigned int runs, std::string verification_mode, unsigned long digits) //: finished(false)
		{
			chudnovsky ch(r);
			ch.calculate(runs, verification_mode, digits);
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
			mpfr::mpreal prog;
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
}

#endif
