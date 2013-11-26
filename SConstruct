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
