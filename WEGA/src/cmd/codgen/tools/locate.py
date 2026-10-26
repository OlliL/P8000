#!/usr/bin/env python3
"""locate.py - finds our functions by name in a built codgen binary (for findings.json).

The linked binary has an object marker (symbol type 31) in front of every object file; the functions of an object
come in the order of their definitions in the source (the old cc emits them in source order). So:
  object = the range from its marker to the next one; source file = the marker name (reader3. -> reader3.c);
  starts = the global function symbols (type 34) of the object plus the targets of its calls (static functions have no symbol);
  names  = the function definitions of the source file, in source order.
The numbers of starts and names must agree; otherwise the object is reported and nothing is guessed.

Usage:
  python3 locate.py --mine ./codgen [--src .] [--out located.json]      # locate all
  python3 locate.py --mine ./codgen --check                            # reproduction test against findings.json
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import funcs as F  # noqa: E402
from sout import SOut  # noqa: E402

KEYWORDS = {'if', 'while', 'for', 'switch', 'return', 'sizeof', 'do', 'else'}
DEF = re.compile(r'^(?:static\s+)?(?:(?:int|void|long|char|unsigned|register|struct\s+\w+|\w+)\s+\*?\s*)?\**\s*([A-Za-z_]\w*)\s*\(')


def source_defs(path, seg=False):
    """The function definitions of a source file in source order (column 0, no prototype, no #).
    The preprocessor conditions #ifdef / #ifndef / #else / #endif on SEG are evaluated (seg: scodgen build)."""
    names = []
    stack = []   # each entry: whether the current branch is active
    active = True
    for line in open(path, errors='replace'):
        t = line.strip()
        if t.startswith('#'):
            d = t[1:].strip()
            if d.startswith('ifdef') or d.startswith('ifndef'):
                cond = d.split()[1] == 'SEG'
                if d.startswith('ifndef'):
                    cond = not cond
                if d.split()[1] == 'SEG':
                    cond = seg if d.startswith('ifdef') else not seg
                else:
                    cond = True
                stack.append((active, cond))
                active = active and cond
            elif d.startswith('else') and stack:
                outer, cond = stack[-1]
                active = outer and not cond
                stack[-1] = (outer, not cond)
            elif d.startswith('endif') and stack:
                outer, cond = stack.pop()
                active = outer
            continue
        if not active or not line or line[0] in ' \t/}{*\n' or line.rstrip().endswith(';'):
            continue
        m = DEF.match(line)
        if m and m.group(1) not in KEYWORDS and not m.group(1).isupper():
            names.append(m.group(1))
    return names


def objects(s):
    """(marker name, start, end) for the objects of the program part (in address order)."""
    marks = sorted((x['value'], x['name']) for x in s.symbols if x['type'] == 31 and x['segt'] == 0)
    lim = F.own_limit(s)
    out = []
    for i, (v, n) in enumerate(marks):
        if v >= lim:
            break
        end = marks[i + 1][0] if i + 1 < len(marks) else lim
        out.append((n, v, min(end, lim)))
    return out


def source_name(marker):
    base = marker.rstrip('.')
    base = re.sub(r'\.o$', '', base)
    return base + '.c'


def starts_of(s, rows, lo, hi):
    """function starts in [lo, hi): global function symbols and the targets of calls (static functions)."""
    st = {x['value'] for x in s.symbols if x['type'] == 34 and x['segt'] == 0 and lo <= x['value'] < hi}
    syms = {x['name'].lstrip('_'): x['value'] for x in s.symbols if x['type'] == 34 and x['segt'] == 0}
    prev = None
    for a, r in rows:
        if prev is not None and lo <= a < hi and prev == 'ret' and re.match(r'^(dec|sub)\tr15,', r):
            st.add(a)                       # a frame starts after a ret
        prev = r
        if not (lo <= a < hi):
            continue
        m = re.match(r'^(call|calr)\s+(.*)$', r)
        if not m:
            continue
        t = m.group(2).strip()
        mm = re.match(r'^%([0-9A-F]{4})$', t) or re.match(r'^L0t_([0-9a-f]{4})$', t)
        if mm:
            x = int(mm.group(1), 16)
        elif t.startswith('_') and t[1:] in syms:
            x = syms[t[1:]]
        else:
            continue
        if lo <= x < hi:
            st.add(x)
    return sorted(st)


def locate(mine, src, seg=False):
    s = SOut(open(mine, 'rb').read())
    rows = F.disasm(mine, F.own_limit(s))
    result = {}
    problems = []
    skipped = []
    for marker, lo, hi in objects(s):
        fn = source_name(marker)
        path = os.path.join(src, fn)
        if not os.path.exists(path):
            if not marker.startswith('crt0'):
                problems.append('%s: no source %s' % (marker, fn))
            skipped.append((lo, hi))
            continue
        names = source_defs(path, seg)
        starts = starts_of(s, rows, lo, hi)
        if lo not in starts:
            starts = [lo] + starts
        if len(names) != len(starts):
            problems.append('%s: %d definitions in %s but %d starts %s' % (
                marker, len(names), fn, len(starts), ' '.join('%04x' % a for a in starts)))
            continue
        result[fn] = [{'name': n, 'addr': a} for n, a in zip(names, starts)]
    return result, problems, skipped


def check(result, findings, skipped=()):
    """every function of findings.json (outside the objects without source, e.g. crt0) must be at a located address."""
    f = json.load(open(findings))['functions']
    located = {}
    for fn, lst in result.items():
        for e in lst:
            located['%s:%s' % (fn, e['name'])] = e['addr']
    bad = []
    for name, v in f.items():
        if not v.get('ours'):
            continue   # not located (the start code): not a codgen function
        a = int(v['ours'].split('-')[0], 16)
        if any(lo <= a < hi for lo, hi in skipped):
            continue
        hits = [k for k, x in located.items() if x == a]
        if not hits:
            bad.append('%s at %s: no located function' % (name, v['ours']))
        elif not name.startswith('static_') and not any(h.split(':')[1] == name for h in hits):
            bad.append('%s at %s: located as %s' % (name, v['ours'], hits[0]))
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mine', default='./codgen')
    ap.add_argument('--src', default=os.path.join(HERE, '..'))
    ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
    ap.add_argument('--out', default=None)
    ap.add_argument('--check', action='store_true')
    ap.add_argument('--seg', action='store_true', help='scodgen build (SEG defined)')
    a = ap.parse_args()
    result, problems, skipped = locate(a.mine, a.src, a.seg)
    for p in problems:
        print('PROBLEM', p)
    if a.check:
        bad = check(result, a.findings, skipped)
        for b in bad:
            print('MISMATCH', b)
        print('check: %d functions in findings, %d mismatches, %d problems' % (
            len(json.load(open(a.findings))['functions']), len(bad), len(problems)))
        sys.exit(1 if (bad or problems) else 0)
    if a.out:
        json.dump(result, open(a.out, 'w'), indent=1)
        print('written', a.out)
    print('objects located:', len(result), ' functions:', sum(len(v) for v in result.values()))


if __name__ == '__main__':
    main()
