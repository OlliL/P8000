"""Corpus of cas (host/recon/corpusrun.py); the original /bin/cas against the shipped build and the host build. The cases
(nothing is random; the same every time):
  c PATH        a C source of src/cmd or src/lib, made into assembler by the original /bin/cc -S -O (n.s) and /bin/scc -S -O (s.s)
  op_MODE_K     block K (3000 instructions) of the opcodes z8k.py decodes: MODE ns, ss, sl (test/casgen.py)
  fz_MODE_K     block K of the operand patterns of the mnemonics z8k.py does not decode: MODE ns, sl
  rel_SEG_D     jumps and calls at the distance D (words), or the chains of jpr (D = chain), nonsegmented (SEG 0) and segmented (1)
  jpr_NAME      the fixed sources test/jpr/NAME.s (the decision of jpr and callr), assembled nonsegmented and with a .seg in front
  opt_NAME      the options -l -d -r -t -i -m -v -^ on the fixed sources test/jpr/NAME.s
Each run is a step `cas OPTIONS -o B.o B.s`. What the original leaves undefined is cut away as the original leaves it:
after an error (exit status other than 0) the files hold what happened to be in memory and a warning behind an error depends
on leftover values: only the messages without the warnings count, B.o, B.l*, B.i are removed; the intermediate file (-i) holds
addresses of the tables of the program: only its size counts. The settings of the reconstruction tools stay in progcfg.py."""
import functools
import glob
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TESTDIR = os.path.join(HERE, 'test')
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
sys.path.insert(0, TESTDIR)
import corpusc  # noqa: E402

NAME = 'cas'
ORIG = 'bin/cas'
PROG = 'cas'
CANDIDATES = {'wega': 'src/cmd/cas/build/cas', 'host': 'host/cas/cas'}
BLOCK = 3000
OPTS = ['-u -l', '-u -d', '-u -r', '-u -t', '-u -i', '-u -m', '-u -v', '-u -^', '-l', '-u -l -d']
PREP_FILES = ['bin/cc', 'bin/scc', 'lib/cpp', 'lib/cparse', 'lib/codgen', 'lib/popt', 'lib/scparse', 'lib/scodgen', 'lib/spopt',
              'usr/include']
GEN = [os.path.join(TESTDIR, 'casgen.py'), os.path.join(HERE, '..', '..', 'host', 'tools', 'z8k.py'),
       os.path.join(HERE, '..', '..', 'src', 'cmd', 'cas', 'pstdata.h')]
# old_max.s hits the limit of 255 undecided entries of the old cas: the shipped cas stops with "jump optimization: Not enough
# core", the host cas has no such limit (the case is in the corpus of casold)
EXCLUDE = ('jpr_old_max', 'opt_old_max')
EXPECT = {'c src/lib/libc/common/gen/abs.c': 'expect: exit 0\nexpect: missing n.l\n'}


def after_step(argv, rc, out, err, rd):
    src = [w for w in argv if w.endswith('.s')]
    if rc != 0:
        err = ''.join(l for l in err.splitlines(True) if not l.startswith('Warning'))
        for b in src:
            b = b[:-2]
            for f in [b + '.o', b + '.i'] + glob.glob(os.path.join(rd, glob.escape(b) + '.l*')):
                f = os.path.join(rd, f)
                if os.path.exists(f):
                    os.remove(f)
    return out, err


def normalize(what, data):
    if what.endswith('.i') and isinstance(data, bytes):
        return bytes(len(data))
    return data


def _write(path, text):
    with open(path, 'w') as f:
        f.write(text)


def prepare(kind, arg, src, work):
    """the starting files of a case (kind as in the head comment)"""
    import casgen
    if kind == 'c':
        for cc, out in (('/bin/cc', 'n'), ('/bin/scc', 's')):
            if corpusc.prepare_s(src, work, cc, ['-O']) is None:
                return None
            os.rename(os.path.join(work, 'in.s'), os.path.join(work, out + '.s'))
    elif kind in ('op', 'fz'):
        mode, k = arg
        items = casgen.opcodes(mode) if kind == 'op' else casgen.fuzz(mode)
        lines = [t for _, t, _ in items[k * BLOCK:(k + 1) * BLOCK]]
        _write(os.path.join(work, 'x.s'), ('.seg\n' if mode != 'ns' else '') + '.code\n' + ''.join('\t' + t + '\n' for t in lines))
    elif kind == 'rel':
        seg, d = arg
        _write(os.path.join(work, 't.s'), casgen.reljump(seg, [], True) if d == 'chain' else casgen.reljump(seg, [d], False))
    else:
        text = open(src).read()
        _write(os.path.join(work, 'n.s'), text)
        _write(os.path.join(work, 's.s'), '.seg\n' + text)
    return True


def _steps(runs):
    return ''.join('step: cas %s -o %s.o %s\n' % (o, s[:-2], s) for s, o in runs)


def generate(g, exclude=EXCLUDE):
    import casgen
    for src in g.c_sources():
        name = 'c ' + g.name(src)
        yield g.case(src, functools.partial(prepare, 'c', None), _steps([('n.s', '-u'), ('s.s', '-u')]) + EXPECT.get(name, ''),
                     prep_files=PREP_FILES, name=name)
    for kind, modes, items in (('op', ('ns', 'ss', 'sl'), casgen.opcodes), ('fz', ('ns', 'sl'), casgen.fuzz)):
        for mode in modes:
            for k in range((len(items(mode)) + BLOCK - 1) // BLOCK):
                name = '%s_%s_%d' % (kind, mode, k)
                yield g.case(None, functools.partial(prepare, kind, (mode, k)), _steps([('x.s', '-u')]), name=name, sources=GEN)
    for seg in (0, 1):
        for d in list(casgen.ALL) + ['chain']:
            name = 'rel_%d_%s' % (seg, d)
            yield g.case(None, functools.partial(prepare, 'rel', (seg, d)), _steps([('t.s', '-u')]), name=name, sources=GEN)
    for f in sorted(glob.glob(os.path.join(TESTDIR, 'jpr', '*.s'))):
        n = os.path.basename(f)[:-2]
        for kind in ('jpr', 'opt'):
            name = '%s_%s' % (kind, n)
            if name in exclude:
                continue
            if kind == 'jpr':
                runs = [('n.s', '-u'), ('s.s', '-u'), ('n.s', '-u -l'), ('s.s', '-u -l')]
            else:
                runs = [(s, o) for o in OPTS for s in ('n.s', 's.s')]
            yield g.case(f, functools.partial(prepare, 'jpr', None), _steps(runs), name=name, sources=[f])
