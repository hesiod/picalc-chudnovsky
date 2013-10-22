import os

env = Environment()

env['ENV']['TERM'] = os.environ['TERM']

env['CPPFLAGS'] = ['-stdlib=libc++', '-ansi', '-std=c++11', '-g', '-O0', '-fcaret-diagnostics', '-march=native', '-flto',
'-W', '-Wall', '-Wextra', '-Wpedantic', '-Werror', 
'-Winit-self', '-Wold-style-cast', '-Woverloaded-virtual', '-Wuninitialized', '-Wmissing-declarations']
env['LINKFLAGS'] = ['-flto']
#env['LINK'] = 'ld'
env['CXX'] = 'clang++'

conf = Configure(env)
env = conf.Finish()

env.Program(target = 'pi', source = ["main.cpp", "pi.cpp", "tsio.cpp", "util.cpp"], LIBS = ['c++', 'supc++', 'pthread', 'gmpxx', 'gmp', 'ncurses'])
