#!/usr/bin/env python3
"""fnc.py - one line per function: our draft against the original, the diff goes to a file (context economy).

Usage: RECON_PROG=NAME fnc.py [-v] FILE.c FUNC [FUNC ...]     one line each: OK / DIFF n (lines) / ERR; the diff: $RECON_TMP/fnc/FUNC.diff
       RECON_PROG=NAME fnc.py -a FILE.c                       every function of findings.json with file FILE.c (not the case blocks):
                                                               prints only the totals and the names that are not OK
       -v   also the first 20 lines of the diff of the first FUNC

The range of FUNC is its start in findings.json up to the end of the case blocks and tails that follow it (field "part_of":
FUNC in findings.json). The compiler: env FNC_CC (default /bin/sccvc), FNC_ROOT (default tmp/oldroot), FNC_DROP (masked lines, default none), FNC_DROPRAW
(unmasked lines; default: the redundant `and r0,#%7f00` behind an lda; not a field mask as `and r0,#%0800`), a known difference of the old compilers: genquads); file level `static` is dropped for the symbols.
Replaces the scratch chk.sh/all.sh of the gopt work. The results are written to compare/NAME/fnc.last (name, result) too.
"""
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

import progcfg

CFG = progcfg.load()
HERE = os.path.dirname(os.path.abspath(__file__))
TMP = os.path.join(os.environ.get('RECON_TMP', os.path.join(CFG.REPO, '..', 'tmp', 'tmp')), 'fnc')
os.makedirs(TMP, exist_ok=True)
FN = json.load(open(CFG.FINDINGS))['functions']


def key_order():
    return sorted(FN, key=lambda k: (int(FN[k]['orig'].split(':')[0]), int(FN[k]['orig'].split(':')[1], 16)))


def span(name):
    """(start key, end key) of name: the case blocks behind it belong to it"""
    ks = key_order()
    i = ks.index(name)
    j = i
    while j + 1 < len(ks) and FN[ks[j + 1]].get('part_of') == name:
        j += 1
    return ks[i], ks[j]


def run(src, name):
    k0, k1 = span(name)
    a = FN[k0]['orig']
    hi = '%x' % (int(FN[k1]['orig'].split(':')[1], 16) + FN[k1]['size_orig'])
    td = tempfile.mkdtemp(prefix='fnc', dir=TMP)
    dd = os.path.join(td, os.path.basename(src))
    open(dd, 'w').write(re.sub(r'(?m)^static ', '', open(src).read()))
    env = dict(os.environ, FNC_ROOT=os.environ.get('FNC_ROOT', os.path.join(CFG.REPO, '..', 'tmp', 'oldroot')),
               FNC_CC=os.environ.get('FNC_CC', '/bin/sccvc'), FNC_OLDV7='1',
               FNC_EXTRA=os.environ.get('FNC_EXTRA', ' '.join(f for f in os.listdir(CFG.SRCDIR) if f.startswith('ops_'))),
               FNC_DROP=os.environ.get('FNC_DROP', ''), FNC_DROPRAW=os.environ.get('FNC_DROPRAW', ''))
    r = subprocess.run([sys.executable, os.path.join(HERE, 'fncheck.py'), dd, a, hi, name], env=env, capture_output=True, text=True)
    shutil.rmtree(td, ignore_errors=True)
    out = (r.stdout + r.stderr)
    open(os.path.join(TMP, name + '.diff'), 'w').write(out)
    first = out.split('\n', 1)[0]
    n = sum(1 for l in out.split('\n') if l[:1] in '+-' and l[:3] not in ('---', '+++'))
    if first.startswith('OK'):
        return 'OK', out
    if first.startswith('DIFF'):
        return 'DIFF %d (%s)' % (n, re.sub(r'^DIFF \(masked\): ', '', first)), out
    return 'ERR ' + first[:60], out


def main():
    a = sys.argv[1:]
    verbose = '-v' in a
    a = [x for x in a if x != '-v']
    if a and a[0] == '-a':
        src = a[1]
        base = os.path.basename(src)
        names = [k for k, f in FN.items() if f.get('file') == base and not f.get('part_of')
                 and not re.match(r'f\d_', k)]
        res = {}
        for k in sorted(names, key=lambda k: key_order().index(k)):
            res[k] = run(src, k)[0]
        bad = {k: v for k, v in res.items() if v != 'OK'}
        print('%d functions, %d OK; not OK: %s' % (len(res), len(res) - len(bad), ', '.join('%s=%s' % (k, v.split(' (')[0]) for k, v in bad.items()) or '-'))
        json.dump(res, open(os.path.join(CFG.DIR, 'fnc.last'), 'w'), indent=0)
        return
    src = a[0]
    for i, name in enumerate(a[1:]):
        r, out = run(src, name)
        print('%-14s %s' % (name, r))
        if verbose and i == 0 and r != 'OK':
            print('\n'.join(out.split('\n')[3:23]))


if __name__ == '__main__':
    main()
