"""Corpus of the driver cc (host/recon/corpusrun.py): the command lines of cmdlines.txt, run with -:n -:z (nothing is
executed: compared are the command lines the driver builds, its messages, its exit status and the files afterwards); the
original against the shipped build and the host build. scc (compare/scc/corpus.py) uses the same lines."""
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))

NAME = 'cc'
ORIG = 'bin/cc'
PROG = 'cc'
CANDIDATES = {'wega': 'src/cmd/cc/build/nonseg/cc', 'host': 'host/cc/cc'}
# pass paths that exist in both worlds, so that the access() tests agree
PASSES = '-TO/bin/sh -TP/bin/sh'    # /bin/ld does not exist on macOS


def normalize(what, data):
    """the process id in the names of the temporary files (cc.<pid>.<x>)"""
    if isinstance(data, str):
        return re.sub(r'cc\.-?[0-9]+', 'cc.PID', data)
    return data


def lines_case(prog):
    """one case: the starting files, then for every line of cmdlines.txt the driver, then the files"""
    t = ["host: echo 'main(){}' > a.c; echo 'f(){}' > b.c; : > x.s; : > y.o; : > z.f"]
    for args in open(os.path.join(HERE, 'cmdlines.txt')).read().split('\n'):
        if args.strip():
            t += ['host: rm -f *.o', 'step: %s -:n -:z %s %s' % (prog, PASSES, args), 'host: ls']
    t += ['expect: output contains /lib/cpp']
    return '\n'.join(t) + '\n'


def generate(g):
    yield g.case(None, None, lines_case(PROG), name='cmdlines')
