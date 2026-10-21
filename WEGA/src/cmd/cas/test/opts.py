#!/usr/bin/env python3
"""Options of cas (-l listing, -d, -r, -t, -i, -m, -v, -^ ...) on source files: the original and the cas under
test run in clean directories; the messages, the return code and every file that is written must be equal.
usage: WEGA=<tree> opts.py <cas under test> file.s ...   (the options come from the list OPTS below)"""
import subprocess, sys, os, tempfile, shutil
HERE = os.path.dirname(os.path.abspath(__file__))
HOST = os.path.join(HERE, '..', '..', '..', '..', 'host')
ROOT = os.environ.get('WEGA')
if not ROOT:
    raise SystemExit('set WEGA=<file tree of the original system>')
EMU = [os.path.join(HOST, 'z8k-emu', 'z8k-emu'), '-r', ROOT]

NATIVE = bool(os.environ.get('NATIVE'))      # the cas under test is a program of this host (host/cas), not a WEGA binary
def runcmd(prog):
    """the command that runs prog: the original (/bin/cas) under the emulator, a WEGA binary under test too,
    a native program (NATIVE=1) directly"""
    return [prog] if NATIVE and prog != '/bin/cas' else EMU + [prog]
OPTS = [['-u', '-l'], ['-u', '-d'], ['-u', '-r'], ['-u', '-t'], ['-u', '-i'], ['-u', '-m'], ['-u', '-v'], ['-u', '-^'], ['-l'], ['-u', '-l', '-d']]

def run(prog, opts, src):
    d = tempfile.mkdtemp()
    shutil.copy(src, os.path.join(d, 'w.s'))
    if prog != '/bin/cas': shutil.copy(prog, os.path.join(d, 'cas.new'))
    p = '/bin/cas' if prog == '/bin/cas' else './cas.new'
    try:
        r = subprocess.run(runcmd(p) + opts + ['-o', 'w.o', 'w.s'], capture_output=True, timeout=60, cwd=d)
        rc, err, out = r.returncode, r.stderr, r.stdout
    except subprocess.TimeoutExpired:
        rc, err, out = 'timeout', '', ''
    files = {f: open(os.path.join(d, f), 'rb').read() for f in sorted(os.listdir(d)) if f not in ('w.s', 'cas.new')}
    shutil.rmtree(d)
    return rc, err, out, files

new = os.path.abspath(sys.argv[1])
n = bad = skip = 0
for src in sys.argv[2:]:
    for o in OPTS:
        a = run('/bin/cas', o, src)
        if a[0] != 0 and a[0] != 255 and a[0] != 1: skip += 1; continue
        b = run(new, o, src)
        n += 1
        if a[0] != 0:
            # after a semantic error the files hold what happened to be in memory (the original too): only
            # the messages count
            # (a warning after an error depends on the leftover value too, e.g. "Value too large" behind "Undefined symbol")
            def msgs(r):
                return (r[0], [l for l in r[1].splitlines() if not l.startswith(b'Warning')], r[2])
            a, b = msgs(a), msgs(b)
        else:
            # the intermediate file (-i) holds addresses of the tables, which are different in a rebuilt program
            a = a[:3] + ({f: (len(c) if f.endswith('.i') else c) for f, c in a[3].items()},)
            b = b[:3] + ({f: (len(c) if f.endswith('.i') else c) for f, c in b[3].items()},)
        if a != b:
            bad += 1
            print('DIFF', os.path.basename(src), ' '.join(o), 'rc', a[0], b[0], 'stdout equal', a[2] == b[2], 'stderr equal', a[1] == b[1],
                  *(['files', sorted(a[3]), sorted(b[3]), [f for f in a[3] if a[3].get(f) != b[3].get(f)]] if len(a) > 3 else []))
print('runs', n, 'different', bad, 'skipped', skip)
