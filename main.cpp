/*
 * main.cpp
 * Copyright (C) 2013 Tobias Markus <tobias@markus-regensburg.de>
 * 
 */

#include <thread>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include "pi.h"
#include "tsio.h"
#include "util.h"
#include <tclap/CmdLine.h>

using namespace std;

int main(int argc, char* argv[])
{
	atexit(enable_cursor);
	signal(SIGINT, static_cast<__sighandler_t>( [] (int) { enable_cursor(); exit(1); } ));

	unsigned int threadc;
	unsigned int runc;
	unsigned int prec;

	try
	{
		TCLAP::CmdLine cmd("An arbitrary precision pi calculator using C++ and the GNU multiple precision library (GMP).", ' ', "0.1");

		TCLAP::ValueArg<unsigned int> threadc_arg("j", "jobs", "number of threads to use", \
			false, thread::hardware_concurrency(), "A positive non-zero integral number.", cmd);

		TCLAP::ValueArg<unsigned int> runc_arg("r", "runs", "number of runs to perform (n of the equation)", \
			false, 1000, "A positive non-zero integral number.", cmd);

		TCLAP::ValueArg<unsigned int> prec_arg("p", "prec", "precision of GMP float", \
			false, 100000, "A positive non-zero integral number.", cmd);

	//	TCLAP::SwitchArg reverseSwitch("r","reverse","Print name backwards", cmd, false);

		// Parse the argv array.
		cmd.parse(argc, argv);

		// Get the value parsed by each arg.
		threadc = threadc_arg.getValue();
		runc = runc_arg.getValue();
		prec = prec_arg.getValue();
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "Error while parsing arguments: " << e.error() << " for arg " << e.argId() << std::endl;
		terminate();
	}

	disable_cursor();

	cout << "Using " << threadc << " threads!" << endl;

	pi p(prec, threadc);

	p.calculate(runc);

	//ts << p << endl;

	ts << p.digits() << endl;

	return 0;
}