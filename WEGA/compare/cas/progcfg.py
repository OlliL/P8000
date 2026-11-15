"""Configuration of the recon toolkit (host/recon/progcfg.py) for cas (segmented, built by an older scc we do not have).

The corpus: compare/cas/corpus.py.
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
EXTRA_FUNCS = {'yyparse': {'file': 'parser.y', 'orig': '0x1ff6', 'status': 'open'}}    # not listed by check.sh
STATICFUNCS = True    # static functions (escape, digval) get a symbol, so that strictcmp finds them
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/home/olivleh1/git/P8000/tmp/tmp'), 'd12model_cas', 'casm')
STRICT_INPUT = MODEL_OUT


def _emu():
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/home/olivleh1/git/P8000/tmp/wega31_root'
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
