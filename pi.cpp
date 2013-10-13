/*
	bigfloat();
	~bigfloat();

	bigfloat(const bigfloat& old);
	bigfloat(bigfloat&& old);

	bigfloat operator=(const bigfloat& old);

	bigfloat operator+(bigfloat a, bigfloat b);
	bigfloat operator-(bigfloat a, bigfloat b);
	bigfloat operator*(bigfloat a, bigfloat b);
	bigfloat operator/(bigfloat a, bigfloat b);
	bigfloat operator==(const bigfloat a) const;
	*/

// sqrt(6*sum(1/(n^2)))
// each thread:

#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <future>
#include <gmp.h>
#include <gmpxx.h>
#include "pi.h"

using namespace std;

mutex* tsio::p = nullptr;
once_flag tsio::static_flag = {};

template<typename T>
void print_percent(const T a, const T b)
{
	double rel = (double)a / (double)b;
	rel *= 100;
	if (rel == 0.0f)
		ts << "\r  0.0%";
	else if (rel == 100.0f)
		ts << "\r100.0%";
	else
		ts << "\r" << fixed << setprecision(1) << rel;
}

void back(void)
{
	printf("\b\b\b\b\b");
}

mpf_class pow(const mpf_class src, const unsigned long int exp)
{
	mpf_class tmp;
	mpf_pow_ui(tmp.get_mpf_t(), src.get_mpf_t(), exp);
	return tmp;
}

void pi::do_calculate(const uint64_t phase, const uint64_t runs)
{
	thread_local mpf_class local_sum(0, precision);
	thread_local mpf_class n(phase + 1, precision);
	if (phase == 0) {
	print_percent(0, 100);
	for (thread_local uint64_t i = 0; i <= runs; i++)
	{
		local_sum += (dividend / pow(n, 2));
		n += threads;
		print_percent(i, runs);
	} 
	ts.lprintf("\n");
	}
	else {
	for (thread_local uint64_t i = 0; i <= runs; i++)
	{
		local_sum += (dividend / pow(n, 2));
		n += threads;
	} }
	add_sum(local_sum);
	ts.lprintf("Thread number %llu \thas finished!\n", phase + 1);
}

void join_all(vector<thread>& v)
{
	for (auto& t : v)
		t.join();
}

void pi::calculate(const uint64_t runs)
{
	vector<thread> t (threads);

	for (uint64_t phase = 0; phase < threads; phase++)
	{
		ts << "Starting thread number " << phase + 1 << "." << endl;
		t[phase] = thread([&] (const uint64_t _phase, const uint64_t _runs) { this->do_calculate(_phase, _runs); }, phase, runs);
	}

	join_all(t);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
		exit(1);

	uint64_t threadc = (uint64_t)atoi(argv[1]);
	uint64_t runc = (uint64_t)atoi(argv[2]);
	cout << "Using " << threadc << " threads!" << endl;

	pi p(100000, threadc);

	p.calculate(runc);

	//cout << p;

	return 0;
}
