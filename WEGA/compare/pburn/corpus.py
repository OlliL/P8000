"""Corpus of pburn (host/recon/corpusrun.py): the original /bin/pburn (version 2.1, the one the build makes) against the
shipped build, on the cases of compare/pburn/tests.txt (host/recon/casefile.py). Every case runs on the console with the
8-bit part behind /dev/ud and its EPROM programmer (host/z8k-emu/models/ud8.py): the EPROM in the socket is a file of the
case. Every block runs twice, with the old programmer card (board 1) and the new one (board 2): the generator puts the
board for {board} and keeps the lines "expect1:" for board 1 and "expect2:" for board 2 (owner 2026-11-16: both cards
tested alike). A block whose name ends in "-1" or "-2" runs for that card only.
Not in the corpus: the change of the EPROM in the socket between source and destination of copy
(the operator; copy runs with one EPROM), hardware timing."""
import functools
import os
import shlex
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'pburn'
ORIG = 'bin/pburn'
PROG = 'pburn'
CANDIDATES = {'wega': 'src/cmd/pburn/2.1/pburn'}	# PBURN_VERSION of src/cmd/pburn/Makefile
TIMEOUT = 30
ROOT_FILES = ['bin/sync']	# copied by a host line: an a.out file to program


def generate(g):
    for name, files, lines, want in casefile.blocks(os.path.join(HERE, 'tests.txt'), 't.in'):
        boards = [1, 2]
        base = name
        if name.endswith('-1') or name.endswith('-2'):
            boards = [int(name[-1])]
            base = name[:-2]
        for b in boards:
            text = []
            for l in lines:
                if l.startswith('expect1:') or l.startswith('expect2:'):
                    if int(l[6]) != b:
                        continue
                    l = 'expect:' + l[8:]
                text.append(l.replace('{board}', str(b)))
            text = '\n'.join(text) + '\n'
            if want is not None:
                text += 'expect: output is %s\n' % shlex.quote(want.replace('\\', '\\\\').replace('\n', '\\n'))
            yield g.case(None, functools.partial(casefile.prepare, tuple(sorted(files.items()))), text,
                         name='%s-b%d' % (base, b), sources=[])
