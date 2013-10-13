/*
 * pi.h
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <thread>
#include <gmp.h>
#include <gmpxx.h>
#include "tsio.h"
#include "util.h"

mpf_class pow(const mpf_class src, const unsigned long int exp) noexcept;

template<typename T> void print_percent(const T a, const T b);

void join_all(std::vector<std::thread>& v);

void enable_cursor();
void disable_cursor();

//template<> void print_percent<int>(int, int);
//template<> void print_percent<unsigned int>(unsigned int, unsigned int);

#endif
