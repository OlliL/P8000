"""Configuration of the recon toolkit (host/recon/progcfg.py) for cas (segmented, built by an older scc we do not have).

The corpus (script mode, case.sh): the inputs are made fresh and are the same every time (nothing is random)
  c:PATH       a C source of src/cmd or src/lib, made into assembler by the original /bin/cc -S -O (n.s) and /bin/scc -S -O (s.s)
  op:MODE:K    block K (3000 instructions) of the opcodes z8k.py decodes: MODE ns, ss, sl (test/casgen.py)
  fz:MODE:K    block K of the operand patterns of the mnemonics z8k.py does not decode: MODE ns, sl
  rel:SEG:D    jumps and calls at the distance D (words), or the chains of jpr (D = chain), nonsegmented (SEG 0) and segmented (SEG 1)
  jpr:NAME     the fixed sources test/jpr/NAME.s (the decision of jpr and callr), assembled nonsegmented and with a .seg in front
  opt:NAME     the options -l -d -r -t -i -m -v -^ on the fixed sources test/jpr/NAME.s
"""
import os

NAME = 'cas'
ORIG = 'bin/cas'
OWN_LIM = 0x9694    # the own code of /bin/cas ends here (listing.c); the members of slibc follow
ORIG_LIM = 0x10000  # the original is stripped
SEG = True
CC = '/bin/scc'
# the model (host/recon/d12model.py, strictcmp.py): the link order of src/cmd/cas/Makefile; parser and assemble are compiled without -O there
MODULES = ['ifile', 'instr', 'scanner', 'main', 'nodes', 'parser', 'pst', 'subr', 'symbols', 'ssymtab', 'table1', 'table2', 'locntr',
           'expr', 'p2loop', 'ofile', 'gen', 'assemble', 'getops', 'error', 'pstab', 'jpr', 'jpr2', 'optim', 'listing', 'stable', 'itable', 'ntable']
NOOPT = ('parser', 'assemble')
STATICFUNCS = True    # static functions (escape, digval) get a symbol, so that strictcmp finds them
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_cas', 'casm')
STRICT_INPUT = MODEL_OUT


def _emu():
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
    return [os.path.normpath(os.path.join(here, '..', '..', 'host', 'z8k-emu', 'z8k-emu')), '-r', root]


def model_prepare(work):
    """parser.c and y.tab.h from parser.y, as in src/cmd/cas/Makefile"""
    import subprocess
    subprocess.check_call(_emu() + ['/usr/bin/yacc', '-d', 'parser.y'], cwd=work, stdout=subprocess.DEVNULL)
    os.rename(os.path.join(work, 'y.tab.c'), os.path.join(work, 'parser.c'))
    here = os.path.dirname(os.path.abspath(__file__))
    import shutil
    shutil.copy(os.path.join(here, '..', '..', 'src', 'lib', 'slibc-v11.a'), work)


def model_link(work, objs):
    """the link of src/cmd/cas/Makefile: the tables of the symbol table in the segments 7, 8 and 9"""
    return _emu() + ['/bin/sld', '-i', '-e', 'start', '-z', '-Mt4', '-Md5', '-Mb5', '/lib/scrt0.o'] + objs[:-3] + \
        ['-Md7', objs[-3], '-Md8', objs[-2], '-Md9', objs[-1], '-Md5', 'slibc-v11.a', '-o', 'casm']

CANDIDATES = {'wega': 'src/cmd/cas/build/cas', 'host': 'host/cas/cas'}
CORPUS_BINARY = 'build/cas'

HERE = os.path.dirname(os.path.abspath(__file__))
TESTDIR = os.path.join(HERE, 'test')
BLOCK = 3000
OPTS = ['-u -l', '-u -d', '-u -r', '-u -t', '-u -i', '-u -m', '-u -v', '-u -^', '-l', '-u -l -d']


def CORPUS_EXCLUDE(label):
    """the sources for the early decision of the old cas run in both corpora (the tar is one); old_max.s hits the limit of 255 undecided
    entries of the old cas: the shipped cas stops with "jump optimization: Not enough core", the host cas has no such limit"""
    return label in ('jpr:old_max', 'opt:old_max')


def _jpr_names():
    d = os.path.join(TESTDIR, 'jpr')
    names = sorted(f[:-2] for f in os.listdir(d) if f.endswith('.s')) if os.path.isdir(d) else []
    return names


def _blocks(items):
    return (len(items) + BLOCK - 1) // BLOCK


def CORPUS_SOURCES():
    import corpusc
    import sys
    sys.path.insert(0, TESTDIR)
    import casgen
    out = ['c:' + s for s in corpusc.c_sources()]
    for mode in ('ns', 'ss', 'sl'):
        out += ['op:%s:%d' % (mode, k) for k in range(_blocks(casgen.opcodes(mode)))]
    for mode in ('ns', 'sl'):
        out += ['fz:%s:%d' % (mode, k) for k in range(_blocks(casgen.fuzz(mode)))]
    for seg in (0, 1):
        out += ['rel:%d:%d' % (seg, d) for d in casgen.ALL] + ['rel:%d:chain' % seg]
    for n in _jpr_names():
        out += ['jpr:' + n, 'opt:' + n]
    return out


def _write(path, text):
    with open(path, 'w') as f:
        f.write(text)


def _block_text(mode, lines):
    return ('.seg\n' if mode != 'ns' else '') + '.code\n' + ''.join('\t' + t + '\n' for t in lines)


def CORPUS_PREPARE(src, work):
    import sys
    sys.path.insert(0, TESTDIR)
    import casgen
    import corpusc
    kind, _, rest = src.partition(':')
    runs = []
    if kind == 'c':
        for cc, out in (('/bin/cc', 'n'), ('/bin/scc', 's')):
            if corpusc.prepare_s(rest, work, cc, ['-O']) is None:
                return None
            os.rename(os.path.join(work, 'in.s'), os.path.join(work, out + '.s'))
            runs.append('%s.s -u' % out)
    elif kind in ('op', 'fz'):
        mode, k = rest.split(':')
        items = casgen.opcodes(mode) if kind == 'op' else casgen.fuzz(mode)
        lines = [t for _, t, _ in items[int(k) * BLOCK:(int(k) + 1) * BLOCK]]
        _write(os.path.join(work, 'x.s'), _block_text(mode, lines))
        runs.append('x.s -u')
    elif kind == 'rel':
        seg, d = rest.split(':')
        if d == 'chain':
            text = casgen.reljump(int(seg), [], True)
        else:
            text = casgen.reljump(int(seg), [int(d)], False)
        _write(os.path.join(work, 't.s'), text)
        runs.append('t.s -u')
    elif kind in ('jpr', 'opt'):
        text = open(os.path.join(TESTDIR, 'jpr', rest + '.s')).read()
        _write(os.path.join(work, 'n.s'), text)
        _write(os.path.join(work, 's.s'), '.seg\n' + text)
        if kind == 'jpr':
            runs += ['n.s -u', 's.s -u', 'n.s -u -l', 's.s -u -l']
        else:
            for o in OPTS:
                runs += ['n.s ' + o, 's.s ' + o]
    else:
        return None
    _write(os.path.join(work, 'runs'), '\n'.join(runs) + '\n')
    return src.replace(':', '_').replace('/', '_')


def CORPUS_SCRIPT(base):
    return 'case.sh'
