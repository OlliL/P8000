#!/usr/bin/env python3
"""var_addr.py - find the address in /bin/sld of every global variable of our sources.

Usage: var_addr.py [-v]    (writes var_addr.txt: name, segment, offset, votes, types of the references)

Our functions are compiled to assembler text; the references to variables are `_name` operands. The original function (table
own_functions.txt) has the same instructions with the addresses `<<seg>>%off` or `#%0seg00off`. The instruction sequences are aligned
by mnemonic (difflib) and the variable operands of aligned instructions are paired; the votes over all functions give the address of a
name. A name with two addresses is a conflict (printed).
"""
import collections
import difflib
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import ldsrc  # noqa: E402
import unlink_seg as U  # noqa: E402
from sout import SOut  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
LD = os.path.join(HERE, '..')
MN = {'callr': 'call', 'jpr': 'jp', 'jr': 'jp', 'call': 'call', 'jp': 'jp', 'dbjnz': 'djnz'}


def var_names():
    names = set()
    for l in open(os.path.join(LD, 'ld.h')):
        if l.startswith('extern '):
            m = re.search(r'([A-Za-z_]\w*)\s*(\[[^\]]*\]\s*)*;$', re.sub(r'\{[^}]*\}', '', l))
            names.add(m.group(1))
    return names


def ours_streams(asm, names):
    """{function: [(mnemonic, [var names in the line])]}"""
    out, cur = {}, None
    for l in asm.split('\n'):
        t = l.strip()
        m = re.match(r'^_(\w+)::$', t)
        if m:
            cur = out.setdefault(m.group(1), [])
            continue
        if t.startswith('}\t/* _'):
            cur = None
            continue
        if cur is None or not l.startswith('\t') or t.startswith(('~', '/*', '.')) or ':=' in t:
            continue
        parts = t.split(None, 1)
        mn = MN.get(parts[0], parts[0])
        ops = parts[1] if len(parts) > 1 else ''
        vs = [x[1:] for x in re.findall(r'_\w+', ops) if x[1:] in names]
        cur.append((mn, vs))
    return out


def orig_stream(prog, lo, hi):
    out = []
    for pc, i in U.decode_range(prog, True, lo, hi):
        t = U.z8k.text(i)
        parts = t.split(None, 1)
        mn = MN.get(parts[0], parts[0])
        refs = []
        for m in re.finditer(r'<<([12])>>%([0-9A-F]{4})|#%0([12])00([0-9A-F]{4})', parts[1] if len(parts) > 1 else ''):
            if m.group(1):
                refs.append((int(m.group(1)), int(m.group(2), 16), 'd'))
            else:
                refs.append((int(m.group(3)), int(m.group(4), 16), 'a'))
        out.append((mn, refs))
    return out


def main():
    names = var_names()
    s = SOut(open(PROG, 'rb').read())
    prog = bytes(s.image[:s.segs[0]['code']])
    rows = {}
    for l in open(os.path.join(HERE, 'own_functions.txt')):
        if l[0] == '#' or not l.strip():
            continue
        p = l.split()
        rows[p[1]] = (int(p[0], 16), int(p[2], 16))
    outs, errs = ldsrc.compile_all('-O', asm=True)
    if errs:
        sys.exit(errs)
    votes = collections.defaultdict(collections.Counter)
    fvotes = collections.defaultdict(collections.Counter)
    users = collections.defaultdict(set)
    unaligned = []
    for f, path in outs.items():
        streams = ours_streams(open(path).read(), names)
        for fn, st in streams.items():
            for mn, vs in st:
                for v in vs:
                    users[v].add(f)
            if fn not in rows:
                continue
            o = orig_stream(prog, *rows[fn])
            A = [x[0] for x in st]
            B = [x[0] for x in o]
            sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
            for tag, i1, i2, j1, j2 in sm.get_opcodes():
                if tag != 'equal':
                    continue
                for k in range(i2 - i1):
                    vs = st[i1 + k][1]
                    rf = o[j1 + k][1]
                    if vs and len(vs) == len(rf):
                        for v, r in zip(vs, rf):
                            votes[v][(r[0], r[1])] += 1
                            fvotes[(f, fn, v)][(r[0], r[1])] += 1
            if len(A) != len(B):
                unaligned.append(fn)
    res = {}
    for v in sorted(names):
        c = votes.get(v)
        if not c:
            res[v] = None
            continue
        res[v] = c.most_common()
    with open(os.path.join(HERE, 'var_addr.txt'), 'w') as f:
        for v in sorted(names):
            c = res[v]
            if c is None:
                f.write('%s - unused-or-unpaired users=%s\n' % (v, ','.join(sorted(users.get(v, [])))))
            else:
                f.write('%s %s users=%s\n' % (v, ' '.join('%d:%04x(%d)' % (sg, off, n) for (sg, off), n in c), ','.join(sorted(users[v]))))
    with open(os.path.join(HERE, 'var_fn_addr.txt'), 'w') as g:
        for (f, fn, v), c in sorted(fvotes.items()):
            g.write('%s %s %s %s\n' % (f, fn, v, ' '.join('%d:%04x(%d)' % (sg, off, n) for (sg, off), n in c.most_common())))
    conf = [v for v, c in res.items() if c and len(c) > 1]
    print('variables %d, paired %d, unpaired %d, conflicts %d' % (len(names), sum(1 for c in res.values() if c), sum(1 for c in res.values() if not c), len(conf)))
    for v in conf:
        print('  conflict', v, res[v])
    print('functions with other instruction counts:', len(unaligned))


if __name__ == '__main__':
    main()
