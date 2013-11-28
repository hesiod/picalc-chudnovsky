#include <iomanip>
#include <vector>
#include <thread>
#include "tsio.h"
#include "util.h"

using namespace std;

void join_all(vector<thread>& v)
{
	for (auto& t : v)
		if (t.joinable())
			t.join();
}

void enable_cursor()
{
	// (033 == escape character)
	printf("\033[?25h");
}

void disable_cursor()
{
	// (033 == escape character)
	printf("\033[?25l");
}

void cursor_up()
{
	// (033 == escape character)
	printf("\033[A");
}

void clear_line()
{
	// (033 == escape character)
	printf("\033[J");
}

template<typename T>
void print_percent(const T a, const T b) noexcept
{
	double rel = (double)a / (double)b;
	rel *= 100;
	if (rel == 0.0f)
		cout << "\r  0.0% " << std::flush;
	else if (rel == 100.0f)
		cout << "\r100.0% " << std::flush;
	else if (rel < 10.0f)
		cout << "\r  " << std::fixed << std::setprecision(1) << rel << "% " << std::flush;
	else
		cout << "\r " << std::fixed << std::setprecision(1) << rel << "% " << std::flush;
}

void print_percent(double rel) noexcept
{
	rel *= 100;
	if (rel == 0.0f)
		cout << "\r  0.0% " << std::flush;
	else if (rel == 100.0f)
		cout << "\r100.0% " << std::flush;
	else if (rel < 10.0f)
		cout << "\r  " << std::fixed << std::setprecision(1) << rel << "% " << std::flush;
	else
		cout << "\r " << std::fixed << std::setprecision(1) << rel << "% " << std::flush;
}

/*static inline void loadbar(unsigned int x, unsigned int n, unsigned int w = 50)
{
	if ( (x != n) && (x % (n/100) != 0) ) return;

	float ratio  =  x/(float)n;
	int   c      =  ratio * w;

	cout << setw(3) << (int)(ratio*100) << "% [";
	for (int x=0; x<c; x++) cout << "=";
	for (int x=c; x<w; x++) cout << " ";
	cout << "]\r" << flush;
}*/

template void print_percent<int>(int, int);
template void print_percent<unsigned int>(unsigned int, unsigned int);
template void print_percent<unsigned long>(unsigned long, unsigned long);

