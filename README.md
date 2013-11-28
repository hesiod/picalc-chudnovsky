# picalc-chudnovsky

picalc-chudnovsky is an arbitrary precision pi calculator (Chudnovsky algorithm) using C++ and the Multiple Precision Library with correct rounding (GNU MPFR).

## Prerequisites
* ~~[llvm/clang](http://clang.llvm.org), tested with 3.3~~ gcc is fixed now
* ~~[libc++](http://libcxx.llvm.org) (the Standard C++ Library of clang), tested with 3.3~~ gcc is fixed now
* [gcc](http://gcc.gnu.org)
* ~~[gmp](http://gmplib.org/), tested with 5.1.3~~ not anymore, now using MPFR
* [mpfr](http://mpfr.org/), tested with 3.1.2
* [mpfr C++ header](http://www.holoborodko.com/pavel/mpfr/), header file is included
* [tclap](http://tclap.sourceforge.net/) (the Templatized C++ Command Line Parser), tested with 1.2.1

### Why clang/libc++ and not gcc/libstdc++?
~~Sadly, there appears to be a bug in libstdc++ that causes compile/link errors when using a `std::vector<std::thread>`. libc++ does not have this bug. There should be a bug report somewhere at the libstdc++ page, but I'm too lazy to look for it right now.~~
Nope, libstdc++ is OK again. You are still free to use clang++, of course.

## Compiling

Just run `scons`, it should do the job.

## Running

Run `pi`.
By just running `pi` without any arguments, picalc-euler will assume some default precision settings. These may or may not be desirable for you.
Try `pi --help` for some more advanced settings.

### What do all those settings mean?

* `--jobs`: Number of concurrent execution units (threads) picalc-euler will use. This is the level of hardware concurrency by default. (e.g. 4 on a Quadcore, 8 on an Intel i7, etc.)
* `--verify`: Which verification mode to use, can be full (this takes a really long time), normal (better one using the BBP formula) or none (do not verify at all).
