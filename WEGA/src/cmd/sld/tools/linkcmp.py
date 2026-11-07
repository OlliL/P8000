#!/usr/bin/env python3
"""linkcmp.py [-v NAME]: every function of our linked sld (../sld, with symbols) against /bin/sld, instruction by instruction.

Both are disassembled by host/tools/sdis.py; the original's functions are the ranges of own_functions.txt, ours run from
_NAME to the next text symbol. Masked: addresses of segments other than 127 (the frame), labels and symbols; frame offsets
and registers count. Prints the differing functions; -v NAME the diff of NAME (orig | ours)."""
import re, sys, difflib, os, subprocess
D = os.path.dirname(os.path.abspath(__file__))
OWN = os.path.join(D, 'own_functions.txt')
SDIS = os.path.join(D, '..', '..', '..', '..', 'host', 'tools', 'sdis.py')
WEGA = os.environ.get('WEGA_ROOT', os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root'))

def rows(fn):
    r = []
    text = subprocess.run(['python3', SDIS, '-l', fn], capture_output=True, text=True).stdout
    for l in text.splitlines(True):
        if '// segment table entry' in l and 'text' not in l:
            break
        m = re.match(r'^([0-9a-f]{4}) [0-9a-f ]+\t(.*)$', l.rstrip('\n'))
        if m:
            r.append((int(m.group(1), 16), m.group(2).split('//')[0].strip()))
        m2 = re.match(r'^(_\w+)::', l)
        if m2:
            r.append((None, m2.group(1)))
    return r

def norm(i):
    i = re.sub(r"\|?<<(?!127>>)\d+>>%[0-9A-F]+\|?", "A", i)
    i = re.sub(r'L\dt_[0-9a-f]+', 'L', i)
    i = re.sub(r'_\w+', 'L', i)
    return re.sub(r'\s+', ' ', i)

new, orig = rows(os.path.join(D, '..', 'sld')), rows(os.path.join(WEGA, 'bin', 'sld'))
funcs = {}
cur = None
for a, i in new:
    if a is None:
        cur = i[1:]; funcs[cur] = []
    elif cur:
        funcs[cur].append(i)
ofun = []
for l in open(OWN):
    if l.startswith('#') or not l.strip():
        continue
    p = l.split()
    ofun.append((p[1], int(p[0], 16), int(p[2], 16)))
oi = [(a, i) for a, i in orig if a is not None]
verbose = sys.argv[2] if len(sys.argv) > 2 and sys.argv[1] == '-v' else None
bad = 0
for name, lo, hi in ofun:
    o = [norm(i) for a, i in oi if lo <= a < hi]
    if name not in funcs:
        continue
    n = [norm(i) for i in funcs[name]][:len(o) + 40]
    # cut ours at the length of the original's last ret position
    n = n[:len(o)] if len(n) >= len(o) else n
    if o == n:
        continue
    sm = difflib.SequenceMatcher(None, o, n, autojunk=False)
    d = sum(max(i2 - i1, j2 - j1) for t, i1, i2, j1, j2 in sm.get_opcodes() if t != 'equal')
    bad += 1
    print('%-16s %04x len %3d/%3d differ %3d' % (name, lo, len(o), len(funcs[name]), d))
    if verbose == name:
        for l in difflib.unified_diff(o, n, 'orig', 'ours', lineterm='', n=3):
            print(l)
print('differing', bad)
