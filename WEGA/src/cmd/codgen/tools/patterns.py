#!/usr/bin/env python3
"""patterns.py - the pattern check of the D classes (step "Musterprüfung").

For every confirmed function the instructions of our binary and of the original are compared in the normalised form
(dn: registers, frame, spill slots, data addresses and shift forms removed). Each instruction that still differs is
looked up in explained(): if it matches a D pattern (D1 frame, D2 registers, D3 spill/reload, D5 lda, D6 switch,
D7 jumps, D8 bytes, D10 shifts, D11 tail merge, D13 addresses) it is explained, otherwise it is "unexplained".
A function is pattern-verified when nothing is unexplained. This checks which kind of instruction differs, not
the order; it is a necessary, not a sufficient condition.

Needs: findings.json with the current "ours" ranges (run confirm.py first), the built binary, the original.
Usage: python3 patterns.py --mine ./codgen --orig $WEGA/lib/codgen [--json patterns.json]
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
from sout import SOut  # noqa: E402

LIBC_FROM = 0xbbc4


def dn(x):
    x=F.canon(x); x=re.sub(r'\b(r[lh]?|rr|rq)\d+\b', r'\1S', x)
    x=re.sub(r'%[0-9A-F]{4}\(rS\)', 'F', x); x=re.sub(r'@rS','F',x)
    x=re.sub(r'\b(sdal|slal|sdll|slll|sdl|sla|sll|sda)\b','SH',x); x=re.sub(r'\b(srl|sra)\b','SHR',x)
    x=re.sub(r'\bA\b|%[0-9A-F]{4}','A',x); x=re.sub(r'#%[0-9A-F]{4}','#N',x)
    x=re.sub(r'^(dec|inc|sub|add)\trS,#N$','FRAME',x); return x
def explained(t):
    if re.match(r'^(dec|inc|add|sub)\trS,#\d+$', t): return 'D1 frame'
    if re.match(r'^(dec|inc)\trS,#\d+$', t): return 'D1 frame'
    if t in ('FRAME',): return 'D1 frame'
    if re.match(r'^(ld|ldb|ldl)\t(F,rS|rS,F|F,rrS|rrS,F|F,rlS|rlS,F|F,rhS|rhS,F|F,#N|F,A|A,F|F,rS)$', t): return 'D3 spill/reload'
    if re.match(r'^(ld|ldl)\trS,rS$', t) or re.match(r'^ld\trrS,rrS$', t): return 'D2 register copy'
    if re.match(r'^(jr|jp)\s*', t) or re.match(r'^(jr|jp)\t', t): return 'D7 jump form'
    if re.match(r'^(call|calr) T$', t): return 'D7 call form'
    if re.match(r'^(cpir|jp\t@rS|jp\t@)', t) or re.match(r'^ld\trS,A\(rS\)$', t): return 'D6 switch table'
    if t in ('add\trS,rS',): return 'D6 index scaling'
    if re.search(r'\brlS\b|\brhS\b|^(ldb|cpb|extsb|clrb|decb|incb|subb|andb)', t): return 'D8 byte'
    if re.match(r'^(test|bit|cp)\s*(rS|F)?(,#\w+)?$', t) or t.startswith(('test\t','bit\t','cp\trS,rS','test\tF')): return 'D8/D2 test form'
    if re.match(r'^(and|or)\trS,#A$', t) or re.match(r'^(and|or)\trS,#N$', t): return 'D8 mask'
    if t.startswith(('SH','SHR')) or re.match(r'^(sdal|slal|sdll|slll|sla|sll|sda|sdl|srl|sra)\b', t): return 'D10 shift'
    if re.match(r'^(addl|subl|ldl|cpl|ldk|ldl)\t', t) or re.match(r'^(addl|subl|cpl|extsl|divl|mult)\b', t): return 'D10 long op'
    if re.match(r'^(inc|dec)\tA,#\d+$', t) or re.match(r'^(inc|dec)\tF,#\d+$', t): return 'D13 counter'
    if re.match(r'^(ldk|ld)\trS,#\d+$', t): return 'D2 constant'
    if re.match(r'^ldm\t(F,rS|rS,F),#\d+$', t): return 'D1 register save block'
    if re.match(r'^ld\trS,#A$', t): return 'D13 address constant'
    if re.match(r'^(ld|cp|clr|add|sub)\t(rS,A|A,#A|A|F,#A|rS,#A|A\(rS\)|rS,A\(rS\))(,#A)?$', t) or re.match(r'^(ld|cp|clr)\tA(,#A)?$', t) or re.match(r'^ld\tA\(rS\),rS$', t) or re.match(r'^cp\trS,A\(rS\)$', t): return 'D13 absolute data / D6 table'
    if re.match(r'^neg\trS$', t): return 'D6 index'
    if t.startswith('tcc'): return 'D7 bool form'
    if re.match(r'^clr\tF$', t): return 'D3 clear'
    if re.match(r'^(sral|sdal|slal|sdll|slll)\b', t): return 'D10 shift'
    if re.match(r'^cp\trS,F$', t): return 'D3 compare with spill'
    if re.match(r'^rl\trS,#1$', t): return 'D6 index'
    if re.match(r'^sub\trS,rS$', t): return 'D2 zero'
    if t == 'ret': return 'D11 tail merge'
    if re.match(r'^lda\trS,F$', t) or re.match(r'^lda\trS,A$', t): return 'D5 lda'
    if re.match(r'^(exts|div|mult|testl|srll|sral|sdll|slll|addl|subl|cpl|ldl|extsl)\b', t) or re.match(r'^(exts)\trrS$', t): return 'D10 long op'
    if re.match(r'^(testb|res|setb|resb|bitb)\b', t) or re.match(r'^(and|or)\trS,(rS|A\(rS\))$', t) or re.match(r'^com\trS$', t): return 'D8 mask/bit'
    if re.match(r'^ldir\b', t): return 'D2 block copy'
    if re.match(r'^ld\tA,rS$', t): return 'D13 store absolute'
    return None

def check(m, o, st, en, g, oend):
    """Compares our [st, en) with the original from g (its end from oend). Returns a dict: diff (differing
    instructions), unexplained (count), classes (D label -> count), examples (unexplained forms)."""
    a_ = [dn(x) for x in F.extent(m, st, en)]
    b_ = [dn(x) for x in F.extent(o, g, oend(g))]
    sm = difflib.SequenceMatcher(None, b_, a_, autojunk=False)
    unexp = []
    cls = collections.Counter()
    ndiff = 0
    for t, i1, i2, j1, j2 in sm.get_opcodes():
        if t == 'equal':
            continue
        for x in b_[i1:i2] + a_[j1:j2]:
            ndiff += 1
            c = explained(x)
            if c:
                cls[c] += 1
            else:
                unexp.append(x)
    moved, ex = order_moves(b_, a_)
    return {'diff': ndiff, 'unexplained': len(unexp), 'classes': dict(cls), 'examples': sorted(set(unexp))[:5], 'moved': moved, 'moved_examples': ex}


def order_moves(b_, a_, minlen=2):
    """Instructions that are moved: a chunk that the original has at one place (deleted in the alignment) and that
    we have at another place (inserted in the alignment). Chunks shorter than minlen do not count.
    Returns (moved instructions, the chunks as strings for the first few)."""
    sm = difflib.SequenceMatcher(None, b_, a_, autojunk=False)
    dels = []
    inss = []
    for t, i1, i2, j1, j2 in sm.get_opcodes():
        if t in ('delete', 'replace') and i2 - i1 >= minlen:
            dels.append(b_[i1:i2])
        if t in ('insert', 'replace') and j2 - j1 >= minlen:
            inss.append(a_[j1:j2])
    moved = 0
    ex = []
    for chunk in dels:
        for ins in inss:
            n = len(chunk)
            hit = any(ins[k:k + n] == chunk for k in range(len(ins) - n + 1))
            if hit:
                moved += n
                ex.append(chunk[0])
                break
    return moved, ex[:3]


def orig_end_fn(o):
    _, cand = F.orig_candidates(o)
    cand = sorted(set(c for c in cand if c < LIBC_FROM))

    def oend(x):
        n = [c for c in cand if c > x]
        return n[0] if n else x + 0x200
    return oend


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mine', default='./codgen')
    ap.add_argument('--orig', default=os.path.expandvars('$WEGA/lib/codgen'))
    ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
    ap.add_argument('--json', default=None, help='write the result per function')
    a = ap.parse_args()
    m = F.disasm(a.mine, F.own_limit(SOut(open(a.mine, 'rb').read())))
    o = F.disasm(a.orig)
    oend = orig_end_fn(o)
    f = json.load(open(a.findings))['functions']
    out = {}
    for name, v in f.items():
        if v.get('pairing') != 'confirmed' or not v.get('ours'):
            continue
        st, en = [int(x, 16) for x in v['ours'].split('-')]
        out[name] = check(m, o, st, en, int(v['orig'], 16), oend)
    full = sum(1 for r in out.values() if r['unexplained'] == 0)
    print('confirmed functions checked: %d; fully explained by D patterns: %d; with unexplained items: %d'
          % (len(out), full, len(out) - full))
    for n, r in sorted(out.items()):
        if r['unexplained']:
            print('  %-12s unexplained %3d  e.g. %s' % (n, r['unexplained'], '; '.join(r['examples'])[:120]))
    print('reordered (moved) instructions per function:')
    for n, r in sorted(out.items(), key=lambda x: -x[1]['moved'])[:15]:
        print('  %-12s moved %4d' % (n, r['moved']))
    if a.json:
        json.dump(out, open(a.json, 'w'), indent=1)


if __name__ == '__main__':
    main()
