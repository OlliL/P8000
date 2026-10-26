#!/usr/bin/env python3
"""classify.py - writes the difference class of every confirmed function (key "audit" in findings.json).

The class is decided by one rule set, the pattern check of patterns.py (the same as in the analysis):
  - a function with a documented real difference (findings.json, "class_overrides" with class
    "other differences" and a note that names the finding) is "other differences";
  - else, if every differing instruction matches a D pattern and patterns.structure_flags() gives no warning (length,
    order, unmatched): "D only (pattern-verified)" with the D classes found;
  - else: "other differences", with the unexplained instruction forms as the note.
A function may name its own end in the original ("orig_end", for an internal ret that is followed by more of the same function).
A layout class (D15, findings.json "layout_classes") is added to d_classes; it does not decide the difference class.
The reordered_instructions count (lower bound: chunks of two or more identical instructions that moved) is written too.
The alignment (d_normalised_pct) is written as information only.

Usage: python3 classify.py --mine ./codgen --orig $WEGA/lib/codgen
"""
import argparse
import collections
import difflib
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import funcs as F  # noqa: E402
import patterns as P  # noqa: E402
from sout import SOut  # noqa: E402


def align(a, b):
    """The alignment of the normalised instruction lists (information only)."""
    sm = difflib.SequenceMatcher(None, b, a, autojunk=False)
    return round(100 * sum(i2 - i1 for t, i1, i2, j1, j2 in sm.get_opcodes() if t == 'equal') / max(len(a), len(b), 1))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mine', default='./codgen')
    ap.add_argument('--orig', default=os.path.expandvars('$WEGA/lib/codgen'))
    ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
    a = ap.parse_args()
    m = F.disasm(a.mine, F.own_limit(SOut(open(a.mine, 'rb').read())))
    o = F.disasm(a.orig, lim=0x10000)
    oend = P.orig_end_fn(o)
    fj = json.load(open(a.findings))
    ov = fj.get('class_overrides', {})
    lay = fj.get('layout_classes', {})
    ends = {int(v['orig'], 16): int(v['orig_end'], 16) for v in fj['functions'].values() if v.get('orig_end')}
    cnt = collections.Counter()
    for n, v in fj['functions'].items():
        if v.get('pairing') != 'confirmed' or not v.get('ours'):
            continue
        st, en = [int(x, 16) for x in v['ours'].split('-')]
        g = int(v['orig'], 16)
        end = lambda x: ends.get(x, oend(x))
        r = P.check(m, o, st, en, g, end, g in ends)
        flags = P.structure_flags(r)
        s = align([P.dn(x) for x in F.extent(m, st, en)], [P.dn(x) for x in F.extent(o, g, end(g), g in ends)])
        audit = {'d_normalised_pct': s, 'pattern_diff': r['diff'], 'pattern_unexplained': r['unexplained'],
                 'reordered_instructions': r['moved'], 'reordered_examples': r['moved_examples']}
        if n in ov and ov[n]['class'] == 'other differences':
            audit['difference_class'] = 'other differences'
            audit['difference_note'] = ov[n]['note']
        elif r['unexplained'] == 0 and not flags:
            audit['difference_class'] = 'D only (pattern-verified)'
            audit['d_classes'] = sorted({k.split(' ')[0] for k in r['classes']})
        else:
            audit['difference_class'] = 'other differences'
            if r['unexplained']:
                audit['difference_note'] = 'unexplained instruction forms: ' + '; '.join(r['examples'])
            else:
                audit['difference_note'] = 'structure (not a D pattern): ' + '; '.join(flags)
        audit['structure_flags'] = flags
        audit['len_ours'] = r['len_ours']
        audit['len_orig'] = r['len_orig']
        if n in lay:
            # D15 is a layout class (block order of if/else branches): it is not an instruction pattern, so it
            # is added from findings.json "layout_classes" and does not change the difference class.
            audit['d_classes'] = sorted(set(audit.get('d_classes', [])) | {lay[n]['class']})
            audit['layout_note'] = lay[n]['note']
        v['audit'] = audit
        cnt[audit['difference_class']] += 1
    json.dump(fj, open(a.findings, 'w'), indent=1)
    print(dict(cnt))


if __name__ == '__main__':
    main()
