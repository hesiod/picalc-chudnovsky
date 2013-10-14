# picalc-euler

picalc-euler is an arbitrary precision pi calculator (Euler zeta-2 algorithm) using C++ and the GNU multiple precision library (GMP).

## Prerequisites
* [llvm/clang](http://clang.llvm.org), tested with 3.3
* [libc++]() (the Standard C++ Library of the , tested with 3.3
* [gmp](), tested with 5.1.3
* [tclap](http://tclap.sourceforge.net/) (the Templatized C++ Command Line Parser), tested with 1.2.1

### Why clang/libc++ and not gcc/libstdc++?
Sadly, there appears to be a bug in libstdc++ that causes compile/link errors when using a `std::vector<std::thread>`. libc++ does not have this bug. There should be a bug report somewhere at the libstdc++ page, but I'm too lazy to look for it right now.

## Compiling

Just run `scons`, it should do the job.

## Running

Run `pi`.
By just running `pi` without any arguments, picalc-euler will assume some default precision settings. These may or may not be desirable for you.
Try `pi --help` for some more advanced settings.

### What do all those settings mean?

* `--jobs`: Number of concurrent execution units (threads) picalc-euler will use. This is the level of hardware concurrency by default. (e.g. 4 on a Quadcore, 8 on an Intel i7, etc.)
* `--runs`: Number of calculation runs picalc-euler will perform. This is 'n'. (THe higher, the better)
* `--prec`: Precision of all GMP floats. This will determine how accurate the result can possibly be.

> Please note that currently, you don't pass picalc-euler the number of digits you want, but rather those two obscure values. This will be improved in the future.

