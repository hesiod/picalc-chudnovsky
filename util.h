#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>
#include "tsio.h"
#include "util.h"

/*static inline const mpf_class pow(const mpf_class src, const unsigned long int exp) noexcept
{
	//print_percent(0,100);
	thread_local mpf_class tmp;
	mpf_pow_ui(tmp.get_mpf_t(), src.get_mpf_t(), exp);
	return tmp;
}*/

template<typename T>
std::string to_string(T t, std::ios_base & (*f)(std::ios_base&))
{
	std::ostringstream oss;
	oss << f << t;
	return oss.str();
}

template<typename T> void print_percent(const T a, const T b) noexcept;
void print_percent(double rel) noexcept;

void join_all(std::vector<std::thread>& v);

void enable_cursor();
void disable_cursor();
void cursor_up();
void clear_line();

#endif
