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

void handle_sigint(int) {
    enable_cursor(); cout << endl; exit(1);
}

void handle_sigsegv(int) {
    enable_cursor(); cout << endl << "Recieved SIGSEGV" << endl; exit(1); 
}

int main(int argc, char* argv[])
{
	atexit([] () { enable_cursor(); cout << endl; });
	signal(SIGINT,  static_cast<__sighandler_t>(&handle_sigint));
	signal(SIGSEGV, static_cast<__sighandler_t>(&handle_sigsegv));

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

	cout << "picalc-chudnovsky\tCopyright (C) 2013-2014 Tobias Markus" << endl <<
		"This program comes with ABSOLUTELY NO WARRANTY." << endl <<
		"This is free software, and you are welcome to redistribute it" << endl <<
		"under certain conditions; for more information, visit" << endl <<
		"https://github.com/scogol/picalc-chudnovsky" << endl << endl;

	mpfr::mpreal::set_default_prec(r.precision);

	disable_cursor();

	cout << "Using " << r.threads << " thread(s)!" << endl;
	cout << "Doing " << runc << " runs with a precision of " << r.precision << " for " << digits << " digits." << endl;

	picalc::pi p(r, runc, verification_mode, digits);

	//p.calculate(runc);

	//cout << p << endl;

	//cout << p.digits() << endl;

	return EXIT_SUCCESS;
}
