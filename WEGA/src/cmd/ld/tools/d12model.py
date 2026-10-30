#!/usr/bin/env python3
"""d12model.py - data check of the rebuilt ld under the data layout of the old compiler (D12).

Usage: d12model.py           (work directory: WORK, default tmp/tmp/d12model, kept for strictcmp.py)

The shipped cc emits the string literals of a function in front of the function and pads each one to an even
address. The original /bin/ld shows the layout of the old cc (measured on its data, HANDOFF_ld.md, step 4 model):

  - initialised variables, arrays and switch tables in source order (each item even aligned);
  - all string literals of the file at the end of the file's data, in the order of their first use in the source,
    NOT padded (the pool starts and ends even).

This script compiles ld.c, copyname.c, printname.c, symbols.c with the shipped cc (-O -S), rewrites each .s into
that layout (a label Lnnn whose body is only .byte lines ending in a zero byte AND that is referenced in the file is
a string literal; an unreferenced one, like the static sccsid, is an array and stays), assembles with cas -u, links
like the Makefile and compares the data of the result with /bin/ld byte for byte. The code is not affected (string
addresses are immediates in both).

Assumption (guess, tested by the result): a static char array referenced by code would be taken for a literal; ld has
none (only the unreferenced sccsid arrays).
"""
import difflib
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
LD = os.path.normpath(os.path.join(HERE, '..'))
HOST = os.path.normpath(os.path.join(LD, '..', '..', '..', 'host'))
sys.path.insert(0, os.path.join(HOST, 'tools'))
from sout import SOut  # noqa: E402

WEGA = os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root')
CC = os.environ.get('CC', os.path.join(HOST, 'cc', 'cc'))
CAS = os.environ.get('CAS', os.path.join(HOST, 'cas', 'cas'))
LDBIN = os.environ.get('LDBIN', os.path.join(HOST, 'ld', 'ld'))
WORK = os.environ.get('WORK', '/mnt/share/cvs/P8000/tmp/tmp/d12model')
MODS = ['ld', 'copyname', 'printname', 'symbols']


def rewrite(lines):
    refs = set()
    for l in lines:
        if not re.match(r'^L\d+:', l):
            refs.update(re.findall(r'\bL\d+\b', l))
    out, pool, i, sect = [], [], 0, 'data'
    while i < len(lines):
        l = lines[i]
        s = l.strip()
        if s in ('.data', '.code'):
            sect = s[1:]
        m = re.match(r'^(L\d+):\s*$', l)
        if sect == 'data' and m and m.group(1) in refs:
            j = i + 1
            body = []
            while j < len(lines) and lines[j].lstrip().startswith('.byte'):
                body.append(lines[j])
                j += 1
            if body and re.search(r'%0\s*$', body[-1]):
                pool.append(l)
                pool.extend(body)
                if j < len(lines) and lines[j].strip() == '.even':
                    j += 1
                i = j
                continue
        out.append(l)
        i += 1
    if pool:
        out += ['.data', '.even'] + pool + ['.even']
    return out


def run(cmd, cwd):
    r = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    if r.returncode:
        sys.exit('%s: %s%s' % (' '.join(cmd), r.stdout, r.stderr))


def main():
    shutil.rmtree(WORK, ignore_errors=True)
    os.makedirs(WORK)
    for f in os.listdir(LD):
        if f.endswith(('.c', '.h')):
            shutil.copy(os.path.join(LD, f), WORK)
    for m in MODS:
        run([CC, '-O', '-S', m + '.c'], WORK)
        src = open(os.path.join(WORK, m + '.s')).read().split('\n')
        open(os.path.join(WORK, m + '_old.s'), 'w').write('\n'.join(rewrite(src)) + '\n')
        run([CAS, '-u', '-o', m + '.o', m + '_old.s'], WORK)
    mk = open(os.path.join(LD, 'Makefile')).read().replace('\\\n', ' ')
    objs = re.search(r'^LIBCOBJS = (.*)$', mk, re.M).group(1).split()
    objs = [o.replace('$(OLD)', os.path.join(LD, '..', '..', 'lib', 'oldlibc')) for o in objs]
    objs = [o if o.startswith('/') else os.path.join(LD, o) for o in objs]
    link = [LDBIN, '-i', '-e', 'start', os.path.join(LD, 'crt0.o')] + [m + '.o' for m in MODS] + objs + ['-o', 'ldm']
    run(link, WORK)
    o = SOut(open(os.path.join(WORK, 'ldm'), 'rb').read())
    g = SOut(open(os.path.join(WEGA, 'bin', 'ld'), 'rb').read())

    def data(x):
        c = x.segs[0]['code']
        return bytes(x.image)[c:c + x.segs[0]['data']]
    a, b = data(g), data(o)
    eq = sum(1 for k in range(min(len(a), len(b))) if a[k] == b[k])
    print('data: original %d, model %d bytes; equal at the same address: %d' % (len(a), len(b), eq))
    print('code: original %d, model %d; bss: original %d, model %d' % (
        g.segs[0]['code'], o.segs[0]['code'], g.segs[0]['bss'], o.segs[0]['bss']))
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op != 'equal':
            print('%-7s orig %04x..%04x %-40r model %04x..%04x %r' % (op, i1, i2, a[i1:i2][:24], j1, j2, b[j1:j2][:24]))


if __name__ == '__main__':
    main()
