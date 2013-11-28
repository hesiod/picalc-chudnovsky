#ifndef TSIO_H
#define TSIO_H

#include <iostream>
#include <thread>
#include <future>
#include <sstream>
#include <cstdio>

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

//extern std::mutex p;

template<std::istream& I, std::ostream& O> //, class charT = char, class traits = std::char_traits<charT>>
class tsio  //: std::basic_ostream<charT, traits>
{
private:
	std::mutex printlock, buflock;
	std::stringstream buf;
	//using ostr = std::basic_ostream<charT, traits>;
protected:
public:
	template <typename T>
	std::ostream& operator<<(const T& t)
	{
		std::unique_lock<std::mutex> lock (buflock);
		buf << t;
		return O;
	}
	std::ostream& operator<<(std::ostream& (*f)(std::ostream&))
	{
		std::unique_lock<std::mutex> lock (printlock);
		O << f;
		O << buf.str();
		buf.str("");
		return O;
	}
	/*std::ostream& flush()
	{
		std::unique_lock<std::mutex> lock (printlock);
		O << buf.str();
		buf.str("");
		return O;
	}*/
	/*int lprintf(const char* format, ...) noexcept
	{
		int ret;
		va_list l;
		va_start(l, format);
		std::unique_lock<std::mutex> lock (printlock);
		ret = vprintf(format, l);
		va_end(l);
		return ret;
	}*/
	std::mutex& mutex()
	{
		return printlock;
	}
	tsio() noexcept
	{
	}
	~tsio() noexcept
	{
	}
};

//template <class charT, class traits>
/*template <std::istream& I, std::ostream& O>
tsio& sync(tsio& os)
{
}*/

//template <std::istream& I, std::ostream& O>
//std::mutex tsio<I, O>::p;


//#define ts 	cout
//extern tsio<std::cin, std::cout> ts;

/*template <typename T>
	ostr& operator<<(const T& t)
	{
		buf << t;
		return O;
	}
	ostr& operator<<(ostr& (*func)(ostr&))
	{
		func(*this);
		return O;
	}
	std::ostream& flush()
	{
		std::unique_lock<std::mutex> lock (p);
		O << buf.str();
		buf.str("");
		return O;
	}
	tsio& endl(tsio& os)
	{
		os << '\n';
		os.flush();
		return O;
	}
	template <typename T>
	std::istream& operator>>(T& t)
	{
		std::unique_lock<std::mutex> lock (p);
		I >> t;
		return I;
	}*/
	/*int lprintf(const char* format, ...) noexcept
	{
		char tbuf[1024];
		int ret;
		va_list l;
		va_start(l, format);
		ret = vsnprintf(tbuf, 1024, format, l);
		va_end(l);
		buf << tbuf;
		return ret;
	}*/

#endif
