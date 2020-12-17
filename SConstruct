#!/usr/bin/env python
import os
import fnmatch
import multiprocessing
env = Environment(tools=['default'], toolpath='.')
Decider('MD5-timestamp')
Help("""
'scons' builds the debug version
'scons --release' the release version
""")

# use ncpu jobs
SetOption('num_jobs', multiprocessing.cpu_count())
#print("Using %d parallel jobs" % GetOption('num_jobs'))

# compile all .cc files
src = []
for R,D,F in os.walk('.'):
	for f in fnmatch.filter(F, '*.cc'): src.append(os.path.join(R, f))

# less verbose output
env['GCHCOMSTR']  = "HH $SOURCE"
env['CXXCOMSTR']  = "CC $SOURCE"
env['LINKCOMSTR'] = "LL $TARGET"

# g++ flags
env.Append(CXXFLAGS='-std=c++17 -DUSE_PTHREADS'.split())
env.Append(CXXFLAGS=('-Wall -Wextra -Wno-sign-compare -Wno-deprecated-declarations ' +
	'-Wno-parentheses -Wno-misleading-indentation -Wno-reorder -fstrict-enums ' +
	'-Wno-variadic-macros -Wno-unused-parameter -Wno-unknown-pragmas ' +
	'-Wno-implicit-fallthrough -Wno-missing-field-initializers').split())

# release/debug build
AddOption('--release', dest='release', action='store_true', default=False)
release = GetOption('release')
print(("Release" if release else "Debug")+" Build");
frel = '-O3 -s -DNDEBUG'
fdbg = '-Og -DDEBUG -D_DEBUG -g'
env.Append(CCFLAGS=Split(frel if release else fdbg))

# libs
libs = ('m')
env.Append(LIBS=libs.split());

# target
chords = env.Program(target='chords', source=src)
Default(chords)
