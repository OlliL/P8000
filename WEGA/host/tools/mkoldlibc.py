#!/usr/bin/env python3
"""mkoldlibc.py [--write TSV] [LIB]: the modules of src/lib/oldlibc that are made from C.

--write TSV takes the output of libctable.py and writes LIB/oldlibc/SOURCES, one line per module: NAME VERSION SOURCE COMPILER
HEADERS (guest, or cparse for the stdio.h of src/cmd/cparse). SOURCE is relative to LIB: a source of libc, one in oldlibc, or a
System III source, which is then copied to oldlibc/NAME/NAME.c. Without --write: compile every module into LIB/oldlibc/NAME/VERSION.o.
LIB (default src/lib of this tree). Environment: WEGA, OLDROOT.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
WEGA = os.environ.get('WEGA', os.path.normpath(os.path.join(REPO, '..', 'tmp', 'wega31_root')))
OLDROOT = os.environ.get('OLDROOT', os.path.normpath(os.path.join(REPO, '..', 'tmp', 'oldroot')))
EMU = os.path.join(REPO, 'host', 'z8k-emu', 'z8k-emu')
ORDER = {'ccv0p': 0, 'ccv1': 1, 'ccv0': 2, 'cc': 3}
# the cut module names _main where the original compares with 0x2a: the address of main in cc, not a name of the module
# system 0_2: the cut module has no names for csystem and _system, its code and data are those of the source
MANUAL = [('doprnt', '0_4', 'oldlibc/doprnt/0_4.c', 'cc', 'guest'), ('system', '0_2', 'libc/stdio/system.c', 'ccv1', 'guest')]


def write(tsv, lib):
    best = {}
    for line in open(tsv):
        src, c, fl, h, kind, m, bc, bd = line.rstrip('\n').split('\t')
        if kind != 'old' or fl != '-O' or int(bc) + int(bd):
            continue
        inrepo = os.path.abspath(src).startswith(REPO + '/')
        rank = 2 if not inrepo else 0 if 'src/lib/oldlibc/' in src else 1
        k = (rank, ORDER[c], h != 'guest', src)
        if m not in best or k < best[m][0]:
            best[m] = (k, src, c, h)
    rows = []
    for m in sorted(best):
        _, src, c, h = best[m]
        n, v = m.split('/')
        if os.path.abspath(src).startswith(REPO + '/'):
            s = src.split('src/lib/')[1]
        else:
            s = 'oldlibc/%s/%s.c' % (n, n)
            shutil.copy(src, os.path.join(lib, s))
        rows.append((n, v, s, c, h))
    rows += MANUAL
    with open(os.path.join(lib, 'oldlibc', 'SOURCES'), 'w') as f:
        f.write('# module version source (relative to src/lib) compiler headers; made by host/tools/mkoldlibc.py from host/tools/libctable.py\n')
        for r in sorted(rows):
            f.write('%-10s %-4s %-28s %-6s %s\n' % r)
    print(len(rows), 'modules from C')


def build(lib):
    bad = 0
    for line in open(os.path.join(lib, 'oldlibc', 'SOURCES')):
        if line.startswith('#'):
            continue
        n, v, s, c, h = line.split()
        src = os.path.join(lib, s)
        w = os.path.join(lib, 'oldlibc', n, '.w' + v)
        os.makedirs(w, exist_ok=True)
        d = os.path.dirname(src)
        for f in os.listdir(d):
            if f.endswith('.h'):
                shutil.copy(os.path.join(d, f), w)
        shutil.copy(src, os.path.join(w, 'x.c'))
        if h == 'cparse':
            shutil.copy(os.path.join(lib, '..', 'cmd', 'cparse', 'stdio.h'), w)
        root, cc = (WEGA, '/bin/cc') if c == 'cc' else (OLDROOT, '/bin/' + c)
        r = subprocess.run([EMU, '-r', root, cc, '-O', '-I.', '-c', 'x.c'], cwd=w, capture_output=True, text=True)
        o = os.path.join(w, 'x.o')
        if not os.path.exists(o):
            print('FAIL', n, v, r.stdout + r.stderr)
            bad = 1
        else:
            shutil.move(o, os.path.join(lib, 'oldlibc', n, v + '.o'))
        shutil.rmtree(w)
    return bad


def main():
    a = sys.argv[1:]
    if a and a[0] == '--write':
        write(a[1], a[2] if len(a) > 2 else os.path.join(REPO, 'src', 'lib'))
        return 0
    return build(a[0] if a else os.path.join(REPO, 'src', 'lib'))


if __name__ == '__main__':
    sys.exit(main())
