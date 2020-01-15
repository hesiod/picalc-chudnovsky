project('picalc-chudnovsky', 'cpp', default_options : ['cpp_std=c++17'])

deps = [dependency('threads'), dependency('mpfr')]

srcs = ['main.cpp', 'util.cpp', 'pi.cpp']
executable('picalc-chudnovsky', sources: srcs, dependencies: deps)

