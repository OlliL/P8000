#!/usr/bin/env python3
"""strictlayers.py - the strict difference of every function in layers, and the shapes of what remains.

Usage: strictlayers.py [-w] [-v NAME ...] [-s N] [LINKED]

  LINKED   our linked program with symbols (default progcfg STRICT_INPUT, as strictcmp.py)
  -w       write the result to findings.json (functions -> NAME -> "strict_layers")
  -v NAME  print the remaining hunks of NAME
  -s N     print the N most frequent hunk shapes (default 40)

For every function the strict form of strictcmp.py is compared three times: as it is, with the frame hidden
(zcode.hide_frame: frame offsets and the frame size, D1), and with the frame and the registers hidden (zcode.hide_regs,
D2). What differs in the last layer is cut into hunks (difflib, orig => ours); a hunk gets a shape name when it is one
of the forms below, else it stays "open". This reads the difference in its place in the aligned code, so it is
stronger than the pattern check of patterns.py (which only asks which kind of instruction differs), but it is still a
reading by form: a shape name is a candidate class, not a proof that the source is right. "mask artifact": a
constant that is a function address in one program only (norm_strict masks it there as CODE); not checked further.
"""
import collections
import difflib
import json
import os
import re
import sys

import progcfg
import zcode

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402

TEXT_TYPES = (31, 34)

# shapes of a hunk (both sides with frame and registers hidden); the first match names it
SHAPES = [
    ('D2 register copy', lambda a, b: all(re.match(r'^ldl?\tRr?,Rr?$', t) for t in a + b)),
    ('D3 reload instead of copy', lambda a, b: len(a) == len(b) == 1 and
        {a[0], b[0]} <= {'ld\tR,R', 'ld\tR,F(r15)', 'ldl\tRr,Rr', 'ldl\tRr,F(r15)'}),
    ('D3 reload', lambda a, b: all(re.match(r'^ldl?\tRr?,F\(r15\)$', t) for t in a + b)),
    ('D3 spill', lambda a, b: all(re.match(r'^ldl?\tF\(r15\),Rr?$', t) for t in a + b)),
    ('D2/D3 copies, spills, reloads', lambda a, b: all(re.match(r'^ldl?\t(Rr?,Rr?|Rr?,F\(r15\)|F\(r15\),Rr?)$', t)
        for t in a + b)),
    ('D4 spill form', lambda a, b: all(re.match(r'^ldl?\t(F\(r15\),Rr?|Rr?,Rr?)$', t) for t in a) and
        len(b) == 1 and re.match(r'^ldm\tF\(r15\),R,#\d+$', b[0])),
    ('D16 memory operand', lambda a, b: len(a) == 2 and len(b) == 1 and re.match(r'^ldl?\tRr?,(%[0-9A-F]{4}|F\(r15\))$', a[0])
        and re.match(r'^(and|or|xor|add|sub|cp)l?\tRr?,Rr?$', a[1]) and b[0] == a[1].split('\t')[0] + '\t' +
        a[1].split('\t')[1].split(',')[0] + ',' + a[0].split(',')[1]),
    ('D3 register or memory operand', lambda a, b: len(a) == len(b) == 1 and a[0].split('\t')[0] == b[0].split('\t')[0] and
        {re.sub(r'F\(r15\)|Rr?', 'X', a[0]), re.sub(r'F\(r15\)|Rr?', 'X', b[0])} == {re.sub(r'F\(r15\)|Rr?', 'X', a[0])} and
        ('F(r15)' in a[0]) != ('F(r15)' in b[0])),
    ('D1 frame', lambda a, b: all(re.match(r'^(dec|inc|sub|add)\tr15,#N$', t) for t in a + b)),
    ('D10 long shift', lambda a, b: len(a) == 2 and len(b) == 1 and re.match(r'^ld\tR,#%000[0-9A-F]$', a[0]) and
        a[1].startswith('sdal\t') and b[0].startswith('slal\t')),
    ('D7 jump distance', lambda a, b: len(a) == len(b) and all(
        re.sub(r'^j[pr]\t', 'j\t', x) == re.sub(r'^j[pr]\t', 'j\t', y) and x[:2] == 'jp' and y[:2] == 'jr' or
        re.sub(r'^j[pr]\t', 'j\t', x) == re.sub(r'^j[pr]\t', 'j\t', y) and x[:2] == 'jr' and y[:2] == 'jp'
        for x, y in zip(a, b))),
    ('D6 dispatch form', lambda a, b: len(a) == 1 and len(b) == 3 and re.match(r'^ld\tR,%[0-9A-F]{4}\(R\)$', a[0]) and
        b[0] == 'add\tR,R' and b[1] == 'neg\tR' and re.match(r'^ld\tR,%[0-9A-F]{4}\(R\)$', b[2])),
    ('D6 compare chain', lambda a, b: a and all(re.match(r'^(cp\tR,#%[0-9A-F]{4}|jr\teq,CODE|jp\tCODE|jr\tCODE)$', t)
        for t in a) and any(t.startswith('cpir\t') for t in b)),
    ('D5 local address', lambda a, b: a == ['add\tR,#%' + a[0][-4:], 'add\tR,r15'] and b == ['lda\tR,F(r15)', 'add\tR,R']),
    ('mask artifact', lambda a, b: len(a) == len(b) and a != b and all(
        re.sub(r'#(CODE|%[0-9A-F]{4})\b', '#K', x) == re.sub(r'#(CODE|%[0-9A-F]{4})\b', '#K', y) and 'CODE' in x + y for x, y in zip(a, b))),
    # candidates without a D number yet (seen in cparse 2026-10-31)
    ('new: store through computed address', lambda a, b: len(a) == 3 and re.match(r'^(add\tR,#%[0-9A-F]{4}|inc\tR,#\d+)$', a[0])
        and a[1] == 'ld\tR,R' and re.match(r'^ldl?\t@R,Rr?$', a[2]) and len(b) == 1 and re.match(r'^ldl?\t%[0-9A-F]{4}\(R\),Rr?$', b[0])),
    ('new: reload of a global', lambda a, b: len(a) == 1 and re.match(r'^ld\tR,%[0-9A-F]{4}$', a[0]) and b in (['ld\tR,R'], [])),
]


def shape(a, b):
    for name, test in SHAPES:
        try:
            if test(a, b):
                return name
        except IndexError:
            pass
    return 'open'


def layers(A, B):
    def nd(x, y):
        return sum(1 for l in difflib.unified_diff(x, y, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('---', '+++'))
    fa = [zcode.hide_frame(t) for t in A]
    fb = [zcode.hide_frame(t) for t in B]
    ra = [zcode.hide_regs(t) for t in fa]
    rb = [zcode.hide_regs(t) for t in fb]
    hunks = []
    for t, i1, i2, j1, j2 in difflib.SequenceMatcher(None, ra, rb, autojunk=False).get_opcodes():
        if t != 'equal':
            hunks.append((ra[i1:i2], rb[j1:j2], shape(ra[i1:i2], rb[j1:j2])))
    return nd(A, B), nd(fa, fb), nd(ra, rb), hunks


def main():
    a = sys.argv[1:]
    write = '-w' in a
    a = [x for x in a if x != '-w']
    verbose = set()
    nshape = 40
    if '-s' in a:
        i = a.index('-s')
        nshape = int(a[i + 1])
        del a[i:i + 2]
    while '-v' in a:
        i = a.index('-v')
        verbose.add(a[i + 1])
        del a[i:i + 2]
    path = a[0] if a else CFG.STRICT_INPUT
    o = SOut(open(path, 'rb').read())
    oc = bytes(o.image)[:o.segs[0]['code']]
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    gc = bytes(g.image)[:g.segs[0]['code']]
    text = sorted(set(s['value'] for s in o.symbols if s['type'] in TEXT_TYPES and s['value'] < len(oc)))
    ours_fn = set(s['value'] for s in o.symbols if s['type'] == 34)
    fj = json.load(open(CFG.FINDINGS))
    fn = fj['functions']
    orig_fn = set(int(f['orig'], 16) for f in fn.values())
    sym = {s['name']: s['value'] for s in o.symbols}
    shapes = collections.Counter()
    where = collections.defaultdict(set)
    tally = collections.Counter()
    for name, f in sorted(fn.items(), key=lambda x: int(x[1]['orig'], 16)):
        if f.get('status') == 'crt0' or '_' + name not in sym:
            continue
        lo = int(f['orig'], 16)
        s = sym['_' + name]
        e = min([t for t in text if t > s] + [len(oc)])
        A = [zcode.norm_strict(t, orig_fn, d18=CFG.D18) for _, t in zcode.insns(gc, lo, lo + f['size_orig'], CFG.SEG)]
        B = [zcode.norm_strict(t, ours_fn, d18=CFG.D18) for _, t in zcode.insns(oc, s, e, CFG.SEG)]
        d0, d1, d2, hunks = layers(A, B)
        named = collections.Counter(h[2] for h in hunks)
        cls = ('exact' if d0 == 0 else 'frame only' if d1 == 0 else 'frame and registers only' if d2 == 0 else
               'named shapes only' if not named['open'] else 'open shapes')
        tally[cls] += 1
        print('%-16s %04x strict %4d  -frame %4d  -regs %4d  %-24s %s' % (
            name, lo, d0, d1, d2, cls, ' '.join('%s:%d' % (k, v) for k, v in sorted(named.items()))))
        for x, y, sh in hunks:
            k = (' ; '.join(x) or '-') + '  =>  ' + (' ; '.join(y) or '-')
            shapes[(sh, k[:150])] += 1
            where[(sh, k[:150])].add(name)
        if name in verbose:
            for x, y, sh in hunks:
                print('   [%s]\n     - %s\n     + %s' % (sh, '\n     - '.join(x) or '(none)', '\n     + '.join(y) or '(none)'))
        if write:
            f['strict_layers'] = {'strict': d0, 'frame_hidden': d1, 'regs_hidden': d2, 'class': cls,
                                  'shapes': dict(named)}
    print(dict(tally))
    print('hunk shapes (count, functions, shape, orig => ours):')
    for (sh, k), c in shapes.most_common(nshape):
        print('%4d %3d  %-26s %s   [%s]' % (c, len(where[(sh, k)]), sh, k, ' '.join(sorted(where[(sh, k)])[:5])))
    if write:
        json.dump(fj, open(CFG.FINDINGS, 'w'), indent=1)


if __name__ == '__main__':
    main()
