/*
 * tsio.h
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#ifndef TSIO_H
#define TSIO_H

#include <iostream>
#include <thread>
#include <future>

/*template<std::ostream& O>
class tso : tsio
{
	std::ostream& operator<<(const T& t) = delete;
};

template<std::istream& I>
class tsi
{
	std::istream& operator>>(T& t) = delete;
};*/

template<std::istream& I, std::ostream& O>
class tsio final
{
private:
protected:
	static std::mutex p;
	static std::thread::id id;
	static bool locked;
public:
	template<typename T>
	std::ostream& operator<<(const T& t)
	{
		if (std::this_thread::get_id() == id && locked)
		{
			O << t;
		}
		else
		{
			p.lock();
			O << t;
			p.unlock();
		}
		return O;
	}
	template<typename T>
	std::istream& operator>>(T& t)
	{
		if (std::this_thread::get_id() == id && locked)
		{
			I >> t;
		}
		else
		{
			p.lock();
			I >> t;
			p.unlock();
		}
		return I;
	}
	void lprintf(const char* format, ...) noexcept
	{
		va_list l;
		va_start(l, format);
		if (std::this_thread::get_id() == id && locked)
		{
			vprintf(format, l);
		}
		else
		{
			p.lock();
			vprintf(format, l);
			p.unlock();
		}
		va_end(l);
	}
	void lock() noexcept
	{
		p.lock();
		id = std::this_thread::get_id();
		locked = true;
	}
	void unlock() noexcept
	{
		locked = false;
		p.unlock();
	}
	tsio() noexcept
	{
	}
	~tsio() noexcept
	{
	}
};

template<std::istream& I, std::ostream& O>
std::mutex tsio<I, O>::p;

template<std::istream& I, std::ostream& O>
bool tsio<I, O>::locked = false;

template<std::istream& I, std::ostream& O>
std::thread::id tsio<I, O>::id;

extern tsio<std::cin, std::cout> ts;

#endif
