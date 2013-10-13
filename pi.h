#include <iostream>
#include <fstream>
#include <cstdarg>
#include <thread>
#include <future>

using namespace std;

class tsio : public ostream
{
private:
	static once_flag static_flag;
	static mutex * p;
	static void init() noexcept
	{
		p = new mutex();
	}
public:
	template<typename T>
	ostream& operator<<(const T& t) noexcept
	{
		this->p->lock();
		cout << t;
		this->p->unlock();
		return cout;
	}
	void lprintf(const char* format, ...) noexcept
	{
		va_list l;
		va_start(l, format);
		this->p->lock();
		vprintf(format, l);
		this->p->unlock();
		va_end(l);
	}
	cout_ts() noexcept
	{
		call_once(static_flag, init);
	}
	~cout_ts()
	{
		delete p;
	}
};
static tsio ts;

class pi
{
private:
	void add_sum(const mpf_class origin)
	{
		static mutex m;
		m.lock();
		sum += origin;
		m.unlock();
	}
	void do_calculate(const uint64_t phase, const uint64_t runs);
	uint64_t precision;
	mpf_class sum;
	uint64_t threads;
	const uint64_t dividend = 1;
public:
	friend ostream& operator<<(ostream& out, const pi& p)
	{
		(mpf_class)p.sum = sqrt(p.sum * 6);
		mp_exp_t exponent;
		string outstr = mpf_get_str(NULL, &exponent, 10, 0, p.sum.get_mpf_t());
		out << outstr << "e" << exponent << endl;
		return out;
	}
	pi() : precision(100000), sum(0, precision), threads(1)
	{
	}
	pi(mp_bitcnt_t _precision) : precision(_precision), sum(0, precision), threads(1)
	{
	}
	pi(mp_bitcnt_t _precision, uint64_t thread_count) : precision(_precision), sum(0, _precision), threads(thread_count)
	{
		if (threads < 1)
			throw invalid_argument("At least one thread is needed!");
	}
	~pi()
	{
	}
	void calculate(uint64_t runs);
};
