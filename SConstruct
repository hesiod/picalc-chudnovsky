import os

env = Environment()

env['ENV']['TERM'] = os.environ['TERM']

env['CPPFLAGS'] = ['-stdlib=libc++', '-W', '-Wall', '-Wextra', '-pedantic', '-Winit-self', '-Wold-style-cast', '-Woverloaded-virtual', '-Wuninitialized', '-Wmissing-declarations', '-ansi', '-std=c++11', '-fno-caret-diagnostics', '-O3']
#env['LINKFLAGS'] = ['-static']
#env['LINK'] = 'ld'
env['CXX'] = 'clang++'

env.Program(target = 'pi', source = ["pi.cpp", "tsio.cpp"], LIBS = ['c++', 'supc++', 'pthread', 'gmpxx', 'gmp'], LIBPATH = '.')
