"""Corpus of gopt (host/recon/corpusrun.py): every C source of src/cmd and src/lib, made into x.I, x.D, x.S by the original
cc and by the original scc (`-S -:d`), then `gopt -1 -E x` ... `gopt -5 -E x` (scc: `gopt -Z -N -E x`, as cc and scc call it)
one after the other on the same files: its output and the files (gopt appends the floating constants to x.D) are compared;
the original against the shipped build and the host build. The settings of the reconstruction tools stay in progcfg.py."""
import functools
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'gopt'
ORIG = 'lib/gopt'
PROG = 'gopt'
CANDIDATES = {'wega': 'src/cmd/gopt/gopt', 'host': 'host/gopt/gopt'}
PASSES = {'cc': ('/bin/cc', ['bin/cc', 'lib/cpp', 'lib/cparse', 'usr/include'], ''),
          'scc': ('/bin/scc', ['bin/scc', 'lib/cpp', 'lib/scparse', 'usr/include'], '-Z ')}


def generate(g):
    for kind, (cc, files, z) in PASSES.items():
        prep = functools.partial(corpusc.prepare_ids, cc=cc)
        text = ''.join('step: gopt %s-%d -E x\n' % (z, n) for n in range(1, 6))
        for src in g.c_sources():
            yield g.case(src, prep, text, prep_files=files, name=g.name(src) + '.' + kind)
