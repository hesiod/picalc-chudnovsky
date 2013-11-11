/*
 * main.cpp
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#include <thread>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cmath>
#include "pi.h"
#include "tsio.h"
#include "util.h"
#include <tclap/CmdLine.h>

using namespace std;

class VerificationModeString : public TCLAP::Constraint<std::string>
{
public:
	std::string description() const
	{
		return "Valid verification modes are: full, normal (recommended), none";
	}
	std::string shortID() const
	{
		return "verification mode";
	}
	bool check(const std::string& value) const
	{
		if (value == "full" || value == "normal" || value == "none")
			return true;
		else
			return false;
	}
};

int main(int argc, char* argv[])
{
	atexit([] () { enable_cursor(); cout << endl; });
	//signal(SIGINT, static_cast<__sighandler_t>( [] (int) { enable_cursor(); cout << endl; exit(1); } ));
	//signal(SIGSEGV, static_cast<__sighandler_t>( [] (int) { enable_cursor(); cout << endl << "Recieved SIGSEGV" << endl; exit(1); } ));

	picalc::run_info r;
	unsigned long runc;
	std::string verification_mode;
	unsigned long digits;

	try
	{
		TCLAP::CmdLine cmd("An arbitrary precision pi calculator using C++(11) and the multiple-precision floating-point library (MPFR).", '=', "0.1");

		TCLAP::ValueArg<unsigned long> threadc_arg("j", "jobs", "number of threads to use", \
			false, thread::hardware_concurrency(), "number", cmd);

		TCLAP::ValueArg<unsigned long> digits_arg("d", "digits", "number of digits to calculate", \
			false, 1000, "number", cmd);

		TCLAP::ValueArg<std::string> verification_arg("v", "verify", "verification mode", \
			false, "normal", new VerificationModeString(), cmd);

	//	TCLAP::SwitchArg reverseSwitch("r","reverse","Print name backwards", cmd, false);

// Log(151931373056000) / Log(10) = 14.181647462725477655..

		// Parse the argv array.
		cmd.parse(argc, argv);

		// Get the value parsed by each arg.
		r.threads = threadc_arg.getValue();
		digits = digits_arg.getValue();
		runc = ceil(digits / 14.181647462725477655 * 1.2);
		r.precision = mpfr::digits2bits(digits + 200);
		verification_mode = verification_arg.getValue();
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "Error while parsing arguments: " << e.error() << " for argument " << e.argId() << std::endl;
		return EXIT_FAILURE;
	}

	mpfr::mpreal::set_default_prec(r.precision);

	//disable_cursor();

	cout << "Using " << r.threads << " thread(s)!" << endl;
	cout << "Doing " << runc << " runs with a precision of " << r.precision << " for " << digits << " digits." << endl;

	picalc::pi p(r, runc, verification_mode);

	//p.calculate(runc);

	//cout << p << endl;

	//cout << p.digits() << endl;

	return EXIT_SUCCESS;
}
