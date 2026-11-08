"""Corpus of the C compiler chain (host/tools/corpus.sh cchain): the driver cc compiles every C source of src/cmd and src/lib (-c, without and with -O) and
links a few small programs (link/*.c): the original /bin/cc with the originals of the passes under the emulator, against the host driver whose
cc.conf names the host passes (cpp, cparse, codgen, popt, cas, ld). Compared: messages, exit status, the files afterwards (the objects, the programs).
The inputs are prepared once (corpus.tar); scchain (compare/scchain) is the same with scc and the segmented passes and uses this tar."""
import os
import re

NAME = 'cchain'
ORIG = 'bin/cc'
PROG_NAME = 'cc'
CANDIDATES = {'host': 'host/cc/cc'}
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))

CORPUS_VARIANTS = [{'OPT': ''}, {'OPT': '-O'}]
# the passes of the chain: host binary (relative to the WEGA directory) and the original in the tree
PASSES = {'cpp': ('host/cpp/cpp', 'lib/cpp'), 'cparse': ('host/cparse/cparse', 'lib/cparse'), 'codgen': ('host/codgen/codgen', 'lib/codgen'),
          'popt': ('host/popt/popt', 'lib/popt'), 'cas': ('host/cas/cas', 'bin/cas'), 'ld': ('host/ld/ld', 'bin/ld')}
CRT0, LIBC, CONFNAME = 'lib/crt0.o', 'lib/libc.a', 'cc.conf'
CORPUS_DEPENDS = {'host': [v[0] for v in PASSES.values()]}
CORPUS_ROOT_FILES = ['usr/include', 'bin/mv', CRT0, LIBC] + [v[1] for v in PASSES.values()]


def CORPUS_SOURCES():
    import corpusc
    links = sorted(f for f in os.listdir(os.path.join(HERE, 'link')) if f.endswith('.c'))
    return ['c:' + s for s in corpusc.c_sources()] + ['l:' + f[:-2] for f in links]


def CORPUS_PREPARE(src, work):
    import shutil
    import corpusc
    if src.startswith('l:'):
        shutil.copy(os.path.join(HERE, 'link', src[2:] + '.c'), os.path.join(work, 'lk_' + src[2:] + '.c'))
        return 'lk_' + src[2:]
    corpusc.copy_dir(src[2:], work)
    return os.path.basename(src)[:-2]


def CORPUS_SCRIPT(base):
    return os.path.join('..', 'cchain', 'case.sh')


def CORPUS_HOST_ENV(root):
    """the host driver: cc.conf next to it with the host passes and the files of the tree; cpp reads the include directory from CPP_INCLUDE"""
    conf = ''.join('%s = %s\n' % (k, os.path.join(REPO, v[0])) for k, v in PASSES.items())
    conf += 'crt0 = %s\nlibc = %s\ntmpdir = .\n' % (os.path.join(root, CRT0), os.path.join(root, LIBC))
    return {'CPP_INCLUDE': os.path.join(root, 'usr', 'include'), 'CHAIN_CONF': conf, 'CHAIN_CONFNAME': CONFNAME}


def CORPUS_NORMALIZE(name, data):
    """the process id in the names of the temporary files (cc.<pid>.<x>); the include directory of the host cpp"""
    if name != 'messages':
        return data
    import corpusc
    data = data.replace(os.path.join(corpusc.root(), 'usr', 'include').encode(), b'/usr/include')
    return re.sub(rb'cc\.-?[0-9]+', b'cc.PID', data)
