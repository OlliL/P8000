#!/usr/bin/env python3
"""mkspec.py PROGRAM FIT OWNOBJ.o ... > SPEC: a spec for unlink.py from the output of fitlibc.py.

FIT is the output of fitlibc.py, edited where needed: a line `cut NAME LO HI [DATA]` (hex addresses, decimal size) takes the
place of a gap: a module that no known version fits, with its name. The size of the data of a cut module may be left out
for one module, it is then what is left of the data of the program. A line `shipped NAME` or `old NAME 0_n` is a known module.
Sizes of data and the global symbols of known modules come from their objects. WEGA=dir: the original tree.
"""
import os
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sout import SOut  # noqa: E402

WEGA = os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root')
HERE = os.path.dirname(os.path.abspath(__file__))
OLD = os.path.normpath(os.path.join(HERE, '..', '..', 'src', 'lib', 'oldlibc'))
EMU = os.path.normpath(os.path.join(HERE, '..', 'z8k-emu', 'z8k-emu'))


def obj(k, n, v, work):
    if k == 'shipped':
        return os.path.join(work, n + '.o')
    p = os.path.join(OLD, n, v + '.o')
    if not os.path.exists(p):
        src = os.path.relpath(os.path.join(OLD, n, v + '.s'))
        subprocess.run([EMU, '-r', WEGA, '/bin/cas', '-u', '-o', os.path.relpath(p), src], capture_output=True)
    return p


def main():
    prog, fit = sys.argv[1], sys.argv[2]
    own = sys.argv[3:]
    g = SOut(open(prog, 'rb').read())
    work = tempfile.mkdtemp()
    subprocess.run([EMU, '-r', WEGA, '/bin/ar', 'x', '/lib/libc.a'], cwd=work, capture_output=True)
    pos = 0
    for o in own:
        pos += SOut(open(o, 'rb').read()).segs[0]['code']
    owndata = sum(SOut(open(o, 'rb').read()).segs[0]['data'] for o in own)
    rows = []
    start = pos + 0
    for line in open(fit):
        line = line.split('#')[0].strip() if not line.startswith('#') else ''
        if not line:
            continue
        f = line.split()
        if f[0] == 'cut':
            lo, hi = int(f[2], 16), int(f[3], 16)
            rows.append(['cut', f[1], None, lo, hi, int(f[4]) if len(f) > 4 else None, ['_%s=%x' % (f[1], lo)]])
            pos = hi
            continue
        k, n = f[0], f[1]
        v = f[2] if len(f) > 2 else ''
        o = SOut(open(obj(k, n, v, work), 'rb').read())
        c = o.segs[0]['code']
        syms = []
        for s in o.symbols:
            if s['type'] == 34 and s['value'] < c or (s['type'] == 34 and c == 0):
                syms.append('%s=%x' % (s['name'], pos + s['value']))
        rows.append([k, n, v, pos, pos + c, o.segs[0]['data'], syms])
        pos += c
    known = sum(r[5] for r in rows if r[5] is not None)
    left = g.segs[0]['data'] - owndata - known
    for r in rows:
        if r[5] is None:
            r[5] = left
            left = 0
    print('# libc part of %s, made by mkspec.py from the fit of fitlibc.py' % os.path.basename(prog))
    print('# NAME     KIND    START END  DATA  SYMBOLS')
    for k, n, v, lo, hi, d, syms in rows:
        kind = 'shipped' if k == 'shipped' else 'old'
        rng = ('%04x %04x' % (lo, hi)) if hi > lo else '-    -   '
        print('%-10s %-7s %s %4d   %s' % (n, kind, rng, d, ' '.join(syms)))


if __name__ == '__main__':
    main()
