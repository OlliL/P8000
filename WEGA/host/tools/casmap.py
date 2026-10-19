#!/usr/bin/env python3
"""casmap.py: map of /bin/cas (or another segmented tool with calr-linked modules).

Usage: casmap.py BINARY
Prints the function entries and the blocks of the text that no calr (call
relative = call inside one module) crosses; with the SCCS ids in the data a
block can be assigned to a module (see KNOWLEDGE.md, section cas).
"""
import sys, re, json, collections, subprocess, os
here = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, here)
from sout import SOut

def listing(path):
    out = subprocess.run([sys.executable, os.path.join(here, 'sdis.py'), '-l', path],
                         capture_output=True, text=True).stdout
    ins, seg = [], None
    for l in out.split('\n'):
        if l.startswith('// segment table entry'):
            seg = 'text' if 'text' in l else 'data'
            continue
        if seg != 'text':
            continue
        m = re.match(r'([0-9a-f]{4}) ([0-9a-f ]+?)\t(.*)$', l)
        if m:
            ins.append((int(m.group(1), 16), m.group(3)))
    return ins

def main(path):
    ins = listing(path)
    end = ins[-1][0] + 2
    calls = collections.Counter()
    calr = []
    for a, t in ins:
        m = re.match(r'call\t<<\d+>>%([0-9A-F]{4})', t)
        if m:
            calls[int(m.group(1), 16)] += 1
        m = re.match(r'calr\tL0t_([0-9a-f]{4})', t)
        if m:
            calls[int(m.group(1), 16)] += 1
            calr.append((a, int(m.group(1), 16)))
    ents = set(calls) | {0}
    prev = None
    for a, t in ins:
        if prev and prev.startswith('ret'):
            ents.add(a)
        prev = t
    ents = sorted(e for e in ents if e < end)
    cover = [0] * (end // 2 + 2)
    for a, b in calr:
        for p in range(min(a, b) // 2 + 1, max(a, b) // 2 + 1):
            cover[p] += 1
    bounds = [e for e in ents if cover[e // 2] == 0]
    print('%d instructions, %d function entries, %d calr, %d block boundaries'
          % (len(ins), len(ents), len(calr), len(bounds)))
    bs = sorted(set(bounds + [end]))
    for i in range(len(bs) - 1):
        lo, hi = bs[i], bs[i + 1]
        nf = len([e for e in ents if lo <= e < hi])
        print('%04x-%04x %6d bytes %3d functions' % (lo, hi - 1, hi - lo, nf))

if __name__ == '__main__':
    main(sys.argv[1])
