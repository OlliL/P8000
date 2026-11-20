#!/usr/bin/env python3
"""vt.py - many source forms of one function at once (the source stays untouched).

Usage: RECON_PROG=NAME vt.py FILE FUNC SPEC.py [-j N]
SPEC.py defines V = [(name, [(old, new), ...]), ...]; each variant is FILE (relative to WEGA) with the replacements (each old
must occur once; MISSING otherwise), written to a copy in vt_FUNC/ beside SPEC.py and measured with fnc.py, N at a time
(default 4). Prints base and every variant sorted by DIFF."""
import os, re, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
f, fn, spec = sys.argv[1], sys.argv[2], sys.argv[3]
j = int(sys.argv[sys.argv.index('-j') + 1]) if '-j' in sys.argv else 4
ns = {}
exec(open(spec).read(), ns)
src = open(os.path.join(W, f)).read()
base = os.path.join(os.environ.get('RECON_TMP', os.path.join(W, '..', 'tmp', 'tmp')), 'vt', 'vt_' + fn)    # the copies, outside the repository
os.makedirs(base, exist_ok=True)

def run(item):
    name, reps = item
    s = src
    for o, n in reps:
        if o not in s:
            return (99999, name, 'MISSING: ' + o[:50].replace('\n', '|'))
        s = s.replace(o, n, 1)
    d = tempfile.mkdtemp(dir=base)
    p = os.path.join(d, os.path.basename(f))
    open(p, 'w').write(s)
    r = subprocess.run(['python3', 'host/recon/fnc.py', p, fn], cwd=W, capture_output=True, text=True,
                       env=dict(os.environ))
    m = re.search(r'(OK|DIFF (\d+))(.*)', r.stdout)
    if not m:
        return (9999, name, r.stdout.strip()[:80] + r.stderr.strip()[:80])
    return (0 if m.group(1) == 'OK' else int(m.group(2)), name, m.group(0))

with ThreadPoolExecutor(j) as ex:
    res = list(ex.map(run, [('base', [])] + ns['V']))
for k, name, txt in sorted(res):
    print('%-28s %s' % (name, txt))
