"""Corpus of adb (host/recon/corpusrun.py): the original /bin/adb against the build. The cases are the blocks of
compare/adb/tests.txt (host/recon/casefile.py: the format): adb on programs of the WEGA tree with their symbols
(nonsegmented, separate I and D, segmented), the commands on the standard input: disassembly, the formats, expressions,
variables, maps, externals, the settings, the errors, ! and the command files. Running a process (:r, breakpoints)
and -k (/dev/kmem, crash dumps) are not in the corpus (no ptrace in the emulator). Every case has an expectation on
the original."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'adb'
ORIG = 'bin/adb'
PROG = 'adb'
CANDIDATES = {'wega': 'src/cmd/adb/adb'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'adb t.in')
