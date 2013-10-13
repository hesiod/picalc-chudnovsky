import os

env = Environment()

env['ENV']['TERM'] = os.environ['TERM']

env['CPPFLAGS'] = ['-stdlib=libc++', '-ansi', '-std=c++11',  '-O3', '-fcaret-diagnostics',
'-W', '-Wall', '-Wextra', '-Wpedantic', 
'-Winit-self', '-Wold-style-cast', '-Woverloaded-virtual', '-Wuninitialized', '-Wmissing-declarations']
#env['LINKFLAGS'] = ['-static']
#env['LINK'] = 'ld'
env['CXX'] = 'clang++'

env.Program(target = 'pi', source = ["main.cpp", "pi.cpp", "tsio.cpp", "util.cpp"], LIBS = ['c++', 'supc++', 'pthread', 'gmpxx', 'gmp'])
