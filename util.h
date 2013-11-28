/*
	picalc-chudnovsky is an arbitrary precision pi calculator
	(Chudnovsky algorithm) using C++ and the GNU MPFR library.
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
