#!/usr/bin/env python3
"""actcmp.py - compare the actions of yyparse (cgram1.y) with those of /lib/cparse, rule by rule.

Usage: actcmp.py [-a] [-v RULE ...] [-s]

cgram1.y is made into cgram1.c by the original yacc and compiled alone (modcheck.build, the work directory is kept); in
both yyparse functions the action switch (`ld r2,TABLE(r2); jp @r2` behind `dec r2,#2; cp r2,#N`) gives the start of
the code of every rule; the code of a rule runs to the next start. Prints per rule: masked differences (registers and
frame hidden with -s), the rule text from y.output (yacc -v). -v RULE prints the diff of that rule, -a of all.
"""
import difflib
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
os.environ['RECON_PROG'] = HERE
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon'))
import progcfg  # noqa: E402
import zcode  # noqa: E402

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402
import modcheck  # noqa: E402


def table(code, data, lo, hi, dbase):
    """(first rule, targets) of the action switch of yyparse in [lo, hi)"""
    ins = zcode.insns(code, lo, hi)
    for i, (pc, t) in enumerate(ins):
        m = re.match(r'^ld\tr2,%([0-9A-F]{4})\(r2\)$', t)
        if m and i + 1 < len(ins) and ins[i + 1][1] == 'jp\t@r2' and ins[i - 4][1].startswith('dec\tr2,#'):
            n = int(re.match(r'^cp\tr2,#%([0-9A-F]{4})$', ins[i - 3][1]).group(1), 16) + 1
            k = int(ins[i - 4][1].split('#')[1])
            tb = int(m.group(1), 16) - dbase
            return k, [(data[tb + 2 * j] << 8) | data[tb + 2 * j + 1] for j in range(n)]
    sys.exit('no action switch')


def split(code, lo, hi, k, targets):
    starts = sorted(set(targets + [hi]))
    out = {}
    for j, t in enumerate(targets):
        e = min(x for x in starts if x > t)
        out[j + k] = [x for _, x in zcode.insns(code, t, e)]
    return out


def main():
    a = sys.argv[1:]
    alld = '-a' in a
    soft = '-s' in a
    verbose = set()
    if '-v' in a:
        verbose = set(int(x) for x in a[a.index('-v') + 1:] if x.isdigit())
    os.environ.setdefault('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp')
    work, obj = modcheck.build('cgram1.y')
    subprocess.run([CFG.EMU, '-r', CFG.ROOT, '/usr/bin/yacc', '-v', 'cgram1.y'], cwd=work, capture_output=True)
    rules = {}
    if os.path.exists(os.path.join(work, 'y.output')):
        for m in re.finditer(r'^\s*(\S.*?)_?\s+\((\d+)\)\s*$', open(os.path.join(work, 'y.output')).read(), re.M):
            rules.setdefault(int(m.group(2)), m.group(1))
    o = SOut(open(obj, 'rb').read())
    oc = bytes(o.image)
    ocode = o.segs[0]['code']
    ys = [s['value'] for s in o.symbols if s['name'] == '_yyparse'][0]
    oend = min([s['value'] for s in o.symbols if s['type'] in (2, 34) and s['value'] > ys] + [ocode])
    k1, t1 = table(oc[:ocode], oc[ocode:], ys, oend, 0)
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    gc = bytes(g.image)
    gcode = g.segs[0]['code']
    k2, t2 = table(gc[:gcode], gc[gcode:], 0x0312, 0x1374, 0)
    mine = split(oc[:ocode], ys, oend, k1, t1)
    theirs = split(gc[:gcode], 0x0312, 0x1374, k2, t2)
    same = 0
    for r in sorted(theirs):
        def norm(ls):
            out = [zcode.norm_masked(t, CFG.D18) for t in ls]
            if soft:
                out = [zcode.hide_regs(zcode.hide_frame(t)) for t in out]
            return out
        x, y = norm(theirs[r]), norm(mine.get(r, []))
        d = [l for l in difflib.unified_diff(x, y, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('---', '+++')]
        same += not d
        print('%3d %4d %4d %s  %s' % (r, len(x), len(y), 'SAME' if not d else 'DIFF %d' % len(d), rules.get(r, '')[:90]))
        if (alld and d) or r in verbose:
            print('\n'.join(difflib.unified_diff(x, y, 'orig', 'ours', lineterm='', n=2)))
    print('actions: same %d of %d' % (same, len(theirs)))


if __name__ == '__main__':
    main()
