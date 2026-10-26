#!/usr/bin/env python3
"""classify.py - writes the difference class of every confirmed function (key "audit" in findings.json).

Class: D1..D14 are the old-compiler differences; "other differences" are source differences or unexplained ones.
The bereinigte Ausrichtung (D-normalised alignment, registers, frame, spills, data addresses, shift forms and
shift sign removed) decides the default class: >= 70 % "D only", 50-70 % "D only, to verify", below 50 % "other".
The exceptions are in findings.json under "class_overrides" (with the reason); they win over the default.

Usage: python3 classify.py --mine ./codgen --orig $WEGA/lib/codgen
"""
import argparse, difflib, json, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import funcs as F  # noqa: E402
from sout import SOut  # noqa: E402


def dn(x):
    x = F.canon(x)
    x = re.sub(r'\b(r[lh]?|rr|rq)\d+\b', r'\1S', x)
    x = re.sub(r'%[0-9A-F]{4}\(rS\)', 'F', x)
    x = re.sub(r'@rS', 'F', x)
    x = re.sub(r'\b(sdal|slal|sdll|slll|sdl|sla|sll|sda)\b', 'SH', x)
    x = re.sub(r'\b(srl|sra)\b', 'SHR', x)
    x = re.sub(r'\bA\b|%[0-9A-F]{4}', 'A', x)
    x = re.sub(r'#%[0-9A-F]{4}', '#N', x)
    x = re.sub(r'^(dec|inc|sub|add)\trS,#N$', 'FRAME', x)
    return x


def align(a, b):
    sm = difflib.SequenceMatcher(None, b, a, autojunk=False)
    return round(100 * sum(i2 - i1 for t, i1, i2, j1, j2 in sm.get_opcodes() if t == 'equal') / max(len(a), len(b), 1))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mine', default='./codgen')
    ap.add_argument('--orig', default=os.path.expandvars('$WEGA/lib/codgen'))
    ap.add_argument('--findings', default=os.path.join(os.path.dirname(os.path.abspath(__file__)), 'findings.json'))
    a = ap.parse_args()
    m = F.disasm(a.mine, F.own_limit(SOut(open(a.mine, 'rb').read())))
    o = F.disasm(a.orig)
    strong, cand = F.orig_candidates(o)
    cand = sorted(set(c for c in cand if c < 0xbbc4))

    def oend(x):
        n = [c for c in cand if c > x]
        return n[0] if n else x + 0x200
    fj = json.load(open(a.findings))
    ov = fj.get('class_overrides', {})
    cnt = {}
    for n, v in fj['functions'].items():
        if v.get('pairing') != 'confirmed':
            continue
        st, en = [int(x, 16) for x in v['ours'].split('-')]
        g = int(v['orig'], 16)
        s = align([dn(x) for x in F.extent(m, st, en)], [dn(x) for x in F.extent(o, g, oend(g))])
        if s >= 70:
            k = 'D only (aligned >= 70 %)'
        elif s >= 50:
            k = 'D only, to verify (50-70 %)'
        else:
            k = 'other differences'
        if n in ov:
            k = ov[n]['class']
        v['audit'] = {'d_normalised_pct': s, 'difference_class': k}
        if n in ov:
            v['audit']['difference_note'] = ov[n]['note']
        cnt[k] = cnt.get(k, 0) + 1
    json.dump(fj, open(a.findings, 'w'), indent=1)
    print(cnt)


if __name__ == '__main__':
    main()
