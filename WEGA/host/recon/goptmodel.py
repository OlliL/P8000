#!/usr/bin/env python3
"""goptmodel.py - the model of /lib/gopt for the switch matrix of vc (switchmatrix.py, MATRIX_VER=vc).

Usage: goptmodel.py WORK          (env: WEGA_ROOT or WEGA, the tree of the original; RECON_TMP, where the results go)

Builds WORK/src/cmd/gopt (make clean, make gopt) with the tools of WORK/wbin (sccvc, sldo of the work root), strips a copy
and compares it with the original /lib/gopt instruction by instruction, code segment by code segment (difflib over the
instructions with the addresses of other places masked: jump and call targets, segmented addresses; frame offsets and
constants are compared). Every block that differs counts for the function of the original it lies in (findings.json:
orig, part_of). Writes RECON_TMP/oldcc_model_gopt.txt, one line per function: NAME SEG:OFF SAME or NAME SEG:OFF DIFF LINES
(the format of strictcmp.py), and RECON_TMP/data_gopt.txt: the data bytes equal at the same address.
"""
import bisect
import difflib
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'host', 'tools'))
import zcode  # noqa: E402
from sout import SOut  # noqa: E402

ROOT = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/home/olivleh1/git/P8000/tmp/wega31_root'
TIMEOUT = 1800


def norm(t):
    t = re.sub(r'<<\d+>>%[0-9A-F]+', 'SA', t)				# a segmented address (call, jp, a global)
    t = re.sub(r'#%8[0-9A-F]{7}\b', '#SA', t)				# a segmented address constant
    t = re.sub(r'^(jr|djnz|dbjnz|calr)(\t.*?)%[0-9A-F]+$', r'\1\2R', t)	# a relative target
    return t


def listing(img, segs):
    out, o = [], 0
    for sg, n in segs:
        out += [(sg * 0x10000 + x, t) for x, t in zcode.insns(img[o:o + n], 0, n, True)]
        o += n
    return out


def main():
    work = sys.argv[1]
    tmp = os.environ.get('RECON_TMP', os.path.join(work, 'tmp'))
    src = os.path.join(work, 'src', 'cmd', 'gopt')
    env = dict(os.environ, PATH=os.path.join(work, 'wbin') + ':' + os.environ['PATH'])
    subprocess.run(['make', 'clean'], cwd=src, env=env, capture_output=True, timeout=TIMEOUT)
    r = subprocess.run(['make', 'gopt'], cwd=src, env=env, capture_output=True, text=True, timeout=TIMEOUT)
    if r.returncode or not os.path.exists(os.path.join(src, 'gopt')):
        sys.exit('gopt build failed: ' + (r.stdout + r.stderr)[-300:])
    shutil.copy(os.path.join(src, 'gopt'), os.path.join(src, 'gopt.s'))
    subprocess.run(['strip', 'gopt.s'], cwd=src, env=env, capture_output=True, timeout=TIMEOUT)
    a = SOut(open(os.path.join(ROOT, 'lib', 'gopt'), 'rb').read())
    b = SOut(open(os.path.join(src, 'gopt.s'), 'rb').read())
    sa = [(g['segno'], g['code']) for g in a.segs if g['code']]
    sb = [(g['segno'], g['code']) for g in b.segs if g['code']]
    ia, ib = bytes(a.image), bytes(b.image)
    da, db = ia[sum(n for _, n in sa):], ib[sum(n for _, n in sb):]
    open(os.path.join(tmp, 'data_gopt.txt'), 'w').write(str(sum(1 for x, y in zip(da, db) if x == y)))
    fn = json.load(open(os.path.join(REPO, 'compare', 'gopt', 'findings.json')))['functions']
    starts = sorted((int(v['orig'].split(':')[0]) * 0x10000 + int(v['orig'].split(':')[1], 16), v.get('part_of') or k)
                    for k, v in fn.items())
    keys = [s for s, _ in starts]
    owner = lambda addr: starts[max(0, bisect.bisect_right(keys, addr) - 1)][1]
    lines = {}
    A, B = listing(ia, sa), listing(ib, sb)
    sm = difflib.SequenceMatcher(None, [norm(t) for _, t in A], [norm(t) for _, t in B], autojunk=False)
    for op, i, j, k, l in sm.get_opcodes():
        if op == 'equal':
            continue
        names = [owner(A[x][0]) for x in range(i, j)] or [owner(A[min(i, len(A) - 1)][0])]
        for nm in names:					# each instruction of the original that differs
            lines[nm] = lines.get(nm, 0) + 1
        if l - k > j - i:					# and each one more of ours
            lines[names[-1]] = lines.get(names[-1], 0) + (l - k) - (j - i) - (0 if j > i else 1)
    with open(os.path.join(tmp, 'oldcc_model_gopt.txt'), 'w') as o:
        for _, k in starts:
            # the start-up code and the library are not compiled here: a difference there only follows a shift
            if fn[k].get('part_of') or fn[k].get('status') in ('crt0', 'libc') or (fn[k].get('module') or '').startswith('libc'):
                continue
            o.write('%-16s %s %s\n' % (k, fn[k]['orig'], ('DIFF %d' % lines[k]) if k in lines else 'SAME'))


if __name__ == '__main__':
    main()
