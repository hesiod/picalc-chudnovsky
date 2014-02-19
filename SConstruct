'''
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
'''

import os

env = Environment()

env['ENV']['TERM'] = os.environ['TERM']

env['CPPFLAGS'] = ['-ansi', '-std=c++11', '-march=native', '-O3', #  '-fcaret-diagnostics', '-stdlib=libstdc++',  # -flto = -O4
'-W', '-Wall', '-Wextra', '-Wpedantic', '-Werror', #'-fslp-vectorize-aggressive', #'-g', '-pg',
'-Winit-self', '-Wold-style-cast', '-Woverloaded-virtual', '-Wuninitialized', '-Wmissing-declarations', '-Wno-old-style-cast']
#env['LINKFLAGS'] = ['-flto']
#env['LINKFLAGS'] = ['-pg']
#env['CXX'] = 'clang++'
env['CXX'] = 'g++'

conf = Configure(env)
env = conf.Finish()

env.Program(target = 'pi', source = ["main.cpp", "pi.cpp", "tsio.cpp", "util.cpp"], LIBS = ['pthread', 'mpfr']) #['c++', 'supc++',

