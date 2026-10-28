#!/usr/bin/env python3
"""strict_audit.py - compare every accepted function of ../sld.c with the original in the strict mode of fncheck.

Usage: strict_audit.py [NAME ...]

sld.c is compiled once with scc -O; each function is cut out of the object by its symbol and compared with the code
range of the original (own_functions.txt) after fncheck.norm and keep_locals (frame offsets are compared, link
addresses are masked). Prints per function: the status of the table, the instruction counts and the number of
differing instructions after a sequence alignment, so functions that were accepted with the old loose check but
differ in frame offsets or parameter registers show up.
"""
import difflib
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fncheck as F  # noqa: E402
from sout import SOut  # noqa: E402


def main():
    tmp = tempfile.mkdtemp(prefix='saudit', dir='/mnt/share/cvs/P8000/tmp/tmp')
    shutil.copy(os.path.join(HERE, '..', 'sld.c'), os.path.join(tmp, 'all.c'))
    r = subprocess.run([F.EMU, '-r', F.WEGA, '/bin/scc', '-c', '-O', 'all.c'], cwd=tmp, capture_output=True,
                       text=True, timeout=900)
    if r.returncode != 0:
        sys.exit('sld.c does not compile')
    o = SOut(open(os.path.join(tmp, 'all.o'), 'rb').read())
    tsz = o.segs[0]['code']
    code = bytes(o.image[:tsz])
    starts = sorted((x['value'], x['name'].lstrip('_')) for x in o.symbols if x['type'] == 34 and x['segt'] == 0)
    ours = {n: (v, starts[i + 1][0] if i + 1 < len(starts) else tsz) for i, (v, n) in enumerate(starts)}
    s = SOut(open(F.ORIG, 'rb').read())
    cs = s.segs[0]['code']
    ocode = bytes(s.image[:cs])
    want = sys.argv[1:]
    rows = [l.split() for l in open(os.path.join(HERE, 'own_functions.txt')) if l.strip() and not l.startswith('#')]
    for p in rows:
        entry, name, end, status = int(p[0], 16), p[1], int(p[2], 16), p[-1]
        if status == 'open' or (want and name not in want) or name not in ours:
            continue
        lo, hi = ours[name]
        A = F.keep_locals([F.norm(t) for _, t in F.insns(code, lo, hi, True)])
        B = F.keep_locals([F.norm(t) for _, t in F.insns(ocode, entry, end, (s.magic >> 8) == 0xE6)])
        sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
        diff = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != 'equal')
        print('%-16s %-14s mine %3d orig %3d differ %3d%s' % (name, status, len(A), len(B), diff,
                                                              '' if diff == 0 else '  <--'))
        if want and diff:
            for tag, i1, i2, j1, j2 in sm.get_opcodes():
                if tag == 'equal':
                    continue
                for t in A[i1:i2]:
                    print('    mine ' + t)
                for t in B[j1:j2]:
                    print('    orig ' + t)
                print('    --')


if __name__ == '__main__':
    main()
