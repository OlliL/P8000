#!/usr/bin/env python3
"""locate.py - finds our functions in a built popt with symbols and writes their ranges (findings.json "ours").

The binary must carry the function symbols: the symbol build (tools/Makefile target `symbuild`) compiles the sources with
-Dstatic= so that the static functions are global too; the code is the same (only the symbol visibility differs).
A function runs from its symbol to the next function symbol (or to the end of its object file).

Usage: python3 locate.py --mine ./popt_sym [--findings findings.json] [--out loc.json] [--check]
"""
import argparse
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
from sout import SOut  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mine', default='./popt_sym')
    ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
    ap.add_argument('--out', default=None)
    ap.add_argument('--check', action='store_true')
    ap.add_argument('--update', action='store_true', help='write ours (start-end) into findings.json')
    a = ap.parse_args()
    s = SOut(open(a.mine, 'rb').read())
    funcs = {}
    marks = []
    for x in s.symbols:
        if x.get('type') == 34 and x.get('segt') == 0 and x['name'].startswith('_') and not x['name'].startswith('__'):
            funcs.setdefault(x['name'][1:], x['value'])
        if x.get('type') == 31:
            marks.append(x['value'])
    starts = sorted(set(funcs.values()) | set(marks))
    f = json.load(open(a.findings))
    loc = {}
    missing = []
    for name, v in f['functions'].items():
        if name not in funcs:
            missing.append(name)
            continue
        st = funcs[name]
        nxt = [x for x in starts if x > st]
        en = nxt[0] if nxt else st
        loc[name] = {'start': st, 'end': en}
    print('located', len(loc), 'missing', missing)
    if a.check:
        print('check: %d functions in findings, %d not found' % (len(f['functions']), len(missing)))
    if a.out:
        json.dump(loc, open(a.out, 'w'), indent=1)
    if a.update:
        for name, r in loc.items():
            f['functions'][name]['ours'] = '0x%04x-0x%04x' % (r['start'], r['end'])
        json.dump(f, open(a.findings, 'w'), indent=1)
        print('findings.json updated')


if __name__ == '__main__':
    main()
