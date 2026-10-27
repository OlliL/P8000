#!/usr/bin/env python3
"""classify_sld.py - writes the audit of every checked own function of sld into findings.json.

The same rules as popt/tools/classify.py: the instructions of each function of our work file (object of sld.c,
compiled with the original scc) are compared with the original /bin/sld (patterns.check of popt); every differing
instruction must match a D pattern and the structure must be equal (patterns.structure_flags), then the class is
"D only (pattern-verified)" with the D classes found; otherwise "other differences" with the unexplained forms.

Functions: the work file is compiled once; its function symbols give the ranges (ours), the table
own_functions.txt gives the original ranges (orig, end). Only functions with status done or findings are checked.
The manual reading of a difference is kept in pairing_evidence (written by accept.py), not in the audit.

Usage: classify_sld.py
"""
import collections
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
POPT = os.path.join(HERE, '..', '..', 'popt', 'tools')
sys.path.insert(0, POPT)
import classify as PC  # noqa: E402
import funcs as F  # noqa: E402
import patterns as P  # noqa: E402
from sout import SOut  # noqa: E402

SLD = os.path.join(HERE, '..', 'sld.c')
# indirect register-pair accesses (@rrN) that popt's pattern set does not know: the only difference is the register
# pair (old compiler register choice, D2); checked by hand, see the note. Used only when the pattern check finds
# nothing else unexplained.
MANUAL = {
    'swap_bytes': ('D2', 'indirect word load through another register pair (rr4 here, rr2 in the original)'),
    'f85f8': ('D2', 'indirect byte store through another register pair (rr2 here, rr4 in the original)'),
    'f2e76': ('D2', 'arithmetic shift of the count with another register (r5 here, r9 in the original)'),
    'f6e1a': ('D8', 'byte b[7] read as the low byte of a word load in the original; D8 by reading, no popt pattern'),
}
TABLE = os.path.join(HERE, 'own_functions.txt')
FINDINGS = os.path.join(HERE, 'findings.json')
ORIG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
EMU = '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu'
WEGA = '/mnt/share/cvs/P8000/tmp/wega31_root'


def common(t):
    """the link-time forms are masked on both sides: segments (<<n>>), the bars of frame operands and
    address constants (#%...) are set by the linker (our object has them as 0 or as symbols)"""
    t = re.sub(r'<<\d+>>', '', t).replace('|', '')
    return re.sub(r'#%[0-9A-Fa-f]{4,8}', '#N', t)


def orig_text(t):
    # the disassembler labels the jump targets of the original (L<addr>): the same masking as in mine_text
    return common(re.sub(r'\bL[0-9A-Za-z_]+', '%0000', t))


def mine_text(t):
    """our object names the stack and the data by symbol (not linked yet): symbols become addresses"""
    t = re.sub(r'_stkseg\+(\d+)', lambda m: '%%%04X' % int(m.group(1)), t)
    t = re.sub(r'_stkseg', '%0000', t)
    t = re.sub(r'_[A-Za-z0-9_]+\+(\d+)', lambda m: '%%%04X' % int(m.group(1)), t)
    t = re.sub(r'_[A-Za-z0-9_]+', '%0000', t)
    # object labels of jumps and address constants (L<n>, L<n>t, L<n>d) are addresses too
    t = re.sub(r'\bL[0-9A-Za-z_]+(%0000)?', '%0000', t)
    return common(t)


def compile_object():
    tmp = tempfile.mkdtemp(prefix='classify', dir='/mnt/share/cvs/P8000/tmp/tmp')
    shutil.copy(SLD, os.path.join(tmp, 'all.c'))
    r = subprocess.run([EMU, '-r', WEGA, '/bin/scc', '-c', '-O', 'all.c'], cwd=tmp,
                       capture_output=True, text=True, timeout=900)
    if r.returncode != 0:
        sys.exit('sld.c does not compile: ' + (r.stdout + r.stderr)[:300])
    return os.path.join(tmp, 'all.o')


def read_table():
    rows = []
    for line in open(TABLE):
        if line.startswith('#') or not line.strip():
            continue
        p = line.split()
        rows.append({'entry': p[0], 'name': p[1], 'end': p[2], 'status': p[-1]})
    return rows


def main():
    obj = compile_object()
    o_sym = SOut(open(obj, 'rb').read())
    tsz = o_sym.segs[0]['code']
    starts = sorted((x['value'], x['name'].lstrip('_')) for x in o_sym.symbols
                    if x['type'] == 34 and x['segt'] == 0)
    ours = {}
    for i, (v, n) in enumerate(starts):
        e = starts[i + 1][0] if i + 1 < len(starts) else tsz
        ours[n] = (v, e)
    m = [(a, mine_text(t)) for a, t in F.disasm(obj, lim=0x10000)]
    o = [(a, orig_text(t)) for a, t in F.disasm(ORIG, lim=0x10000)]
    rows = read_table()
    ends = {int(r['entry'], 16): int(r['end'], 16) for r in rows}
    oend = P.orig_end_fn(o)
    end = lambda x: ends.get(x, oend(x))
    data = json.load(open(FINDINGS))
    cnt = collections.Counter()
    for r in rows:
        if r['status'] == 'open':
            continue
        name = r['name']
        if name not in ours:
            print('not in the work file: %s' % name)
            continue
        st, en = ours[name]
        g = int(r['entry'], 16)
        res = P.check(m, o, st, en, g, end, True)
        flags = P.structure_flags(res)
        fn = data['functions'][name]
        pct = PC.align([P.dn(x) for x in F.extent(m, st, en)], [P.dn(x) for x in F.extent(o, g, end(g), True)])
        audit = {'d_normalised_pct': pct, 'pattern_diff': res['diff'], 'pattern_unexplained': res['unexplained'],
                 'reordered_instructions': res['moved'], 'reordered_examples': res['moved_examples']}
        manual = MANUAL.get(name)
        if manual and res['unexplained'] and not flags and (name == 'f6e1a' or all(
                (x.split('\t')[0] in ('ld', 'ldb') and '@rr' in x) or x.split('\t')[0] == 'sra' for x in res['examples'])):
            audit['difference_class'] = 'D only (manual reading)'
            audit['d_classes'] = [manual[0]]
            audit['difference_note'] = manual[1]
        elif res['unexplained'] == 0 and not flags:
            audit['difference_class'] = 'D only (pattern-verified)'
            audit['d_classes'] = sorted({k.split(' ')[0] for k in res['classes']})
        else:
            audit['difference_class'] = 'other differences'
            if res['unexplained']:
                audit['difference_note'] = 'unexplained instruction forms: ' + '; '.join(res['examples'])
            else:
                audit['difference_note'] = 'structure (not a D pattern): ' + '; '.join(flags)
        audit['structure_flags'] = flags
        audit['len_ours'] = res['len_ours']
        audit['len_orig'] = res['len_orig']
        fn['audit'] = audit
        fn['ours'] = '0x%04x-0x%04x' % (st, en)
        cnt[audit['difference_class']] += 1
    json.dump(data, open(FINDINGS, 'w'), indent=1)
    open(FINDINGS, 'a').write('\n')
    print(dict(cnt))


if __name__ == '__main__':
    main()
