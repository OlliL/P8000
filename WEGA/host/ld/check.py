#!/usr/bin/env python3
"""Compare the host ld with the original /bin/ld (run under z8k-emu).

usage: check.py [-n RANDOM_RUNS] [-s SEED]     (environment: WEGA=file tree of the original system, EMU=emulator)

Builds the test objects test/*.c with the original cc, makes archives (ar, ranlib, test/mklib.py), then
links with both linkers: fixed command lines (options, areas, archives, -r chains) and random ones. The
output file and the exit status must be identical.
"""
import os, random, shutil, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.environ.get('WEGA', ''))
EMU = os.environ.get('EMU', os.path.join(HERE, '..', 'z8k-emu', 'z8k-emu'))
HOST = os.path.join(HERE, 'ld')
W = None


def orig(args, cwd, tool='ld'):
    return subprocess.run([EMU, '-r', ROOT, '/bin/' + tool] + args, cwd=cwd, capture_output=True, timeout=120)


def link(args):
    """run both linkers in separate copies of the work dir; returns (ok, message)"""
    res = []
    for who in ('orig', 'host'):
        d = os.path.join(W, who)
        shutil.rmtree(d, ignore_errors=True)
        shutil.copytree(os.path.join(W, 'base'), d, copy_function=shutil.copy2)
        if who == 'orig':
            r = orig(args, d)
        else:
            env = dict(os.environ, WEGA_LIBPATH=ROOT + '/lib:' + ROOT + '/usr/lib')
            r = subprocess.run([HOST] + [ROOT + a if a.startswith('/') else a for a in args], cwd=d,
                               capture_output=True, env=env, timeout=120)
        out = 'a.out'
        for i, a in enumerate(args):
            if a == '-o' and i + 1 < len(args):
                out = args[i + 1]
        p = os.path.join(d, out)
        res.append((r.returncode, open(p, 'rb').read() if os.path.exists(p) else None, r.stderr.decode()))
    (rc1, o1, e1), (rc2, o2, e2) = res
    if rc1 == rc2 and o1 == o2:
        return True, ''
    return False, '%s: rc %d/%d, size %s/%s\n  orig: %s\n  host: %s' % (
        ' '.join(args), rc1, rc2, len(o1) if o1 is not None else None, len(o2) if o2 is not None else None,
        e1.replace('\n', '|')[:200], e2.replace('\n', '|')[:200])


def setup():
    base = os.path.join(W, 'base')
    os.makedirs(base)
    for f in os.listdir(os.path.join(HERE, 'test')):
        if f.endswith('.c'):
            shutil.copy(os.path.join(HERE, 'test', f), base)
    for f in sorted(os.listdir(base)):
        r = orig(['-c', '-O', f], base, 'cc')
        if r.returncode:
            sys.exit('cc %s failed: %s' % (f, r.stderr.decode()))
    mk = os.path.join(HERE, 'test', 'mklib.py')
    sh = lambda *a: subprocess.run(list(a), cwd=base, check=True, capture_output=True)
    orig(['r', 'libx.a', 'b.o', 'c.o', 'd.o', 'u.o'], base, 'ar')
    orig(['r', 'liby.a', 'd.o', 'c.o', 'u.o', 'b.o'], base, 'ar')
    shutil.copy(os.path.join(base, 'libx.a'), os.path.join(base, 'librx.a'))
    orig(['librx.a'], base, '../usr/bin/ranlib') if False else subprocess.run(
        [EMU, '-r', ROOT, '/usr/bin/ranlib', 'librx.a'], cwd=base, capture_output=True)
    sh('python3', mk, 'libs1.a', 'b.o', 'c.o', 'd.o', 'u.o', 'e.o', 'f.o')
    sh('python3', mk, 'libs2.a', 'a.o', 'c.o', 'b.o')
    # the libc, with and without the table of contents (the original ranlib loses most long names)
    shutil.copy(ROOT + '/lib/libc.a', os.path.join(base, 'lc.a'))
    shutil.copy(ROOT + '/lib/libc.a', os.path.join(base, 'lcr.a'))
    subprocess.run([EMU, '-r', ROOT, '/usr/bin/ranlib', 'lcr.a'], cwd=base, capture_output=True)
    # r*.o: relocatable outputs of the original ld, used as input
    for name, opts, objs in (('r1.o', [], ['a.o', 'b.o']), ('r2.o', ['-i'], ['b.o', 'c.o', 'd.o']),
                             ('r3.o', ['-d'], ['a.o', 'b.o', 'c.o', 'f.o']), ('r4.o', ['-x'], ['c.o', 'e.o'])):
        orig(['-r'] + opts + ['-o', name] + objs, base)


FIXED = [
    '-o t /lib/crt0.o a.o b.o u.o -lc', '-o t /lib/crt0.o a.o libx.a -lc', '-o t /lib/crt0.o a.o librx.a -lc',
    '-o t /lib/crt0.o a.o liby.a -lc', '-o t /lib/crt0.o a.o libs1.a -lc', '-o t /lib/crt0.o a.o libs2.a libs1.a -lc',
    '-s -e start -o t /lib/crt0.o a.o libx.a /lib/libc.a', '-x -o t /lib/crt0.o a.o libx.a -lc',
    '-X -o t /lib/crt0.o a.o libx.a -lc', '-S -o t /lib/crt0.o a.o libx.a -lc', '-Ns -o t /lib/crt0.o a.o c.o libx.a -lc',
    '-r -o t a.o b.o c.o', '-r -o t a.o libx.a', '-d -r -o t a.o b.o', '-i -o t /lib/crt0.o a.o libx.a -lc',
    '-o t a.o b.o u.o', '-u _bc -o t b.o u.o', '-o t /lib/crt0.o a.o lcr.a libx.a', '-o t /lib/crt0.o a.o lc.a libx.a',
    '-o t /lib/crt0.o r1.o u.o -lc', '-o t /lib/crt0.o r2.o a.o u.o -lc', '-o t /lib/crt0.o r3.o u.o -lc',
    '-o t /lib/crt0.o a.o r4.o b.o u.o -lc', '-r -o t r1.o r2.o', '-i -o t /lib/crt0.o r2.o a.o u.o -lc',
    '-b 0x200 -o t /lib/crt0.o a.o b.o u.o -lc', '-bt 0x1000 -o t /lib/crt0.o a.o b.o u.o -lc',
    '-bd 0x4000 -o t /lib/crt0.o a.o b.o u.o -lc', '-bb 0x8000 -o t /lib/crt0.o a.o b.o u.o -lc',
    '-i -bd 0x4000 -o t /lib/crt0.o a.o b.o u.o -lc', '-i -b 0x1000 -o t /lib/crt0.o a.o b.o u.o -lc',
    '-t 0x9000 -o t /lib/crt0.o a.o b.o u.o -lc', '-td 0x9000 -o t /lib/crt0.o a.o b.o u.o -lc',
    '-bt 0x1000 -bd 0x3000 -bb 0x5000 -o t /lib/crt0.o a.o b.o u.o -lc', '-bt 0x1000 -bd 0x2000 -o t /lib/crt0.o a.o b.o u.o -lc',
    '-b 0x2000 -bt 0x1000 -o t d.o', '-t 0x2000 -o t d.o', '-o t d.o f.o', '-o t', '-e start -o t d.o', '-Q', '-bx 0x100 d.o',
    '-b 0x100 -b 0x200 -o t d.o', '-o t nofile.o', '-o t u.c', '-o t d.o -lnolib',
]


def rnd(n, seed):
    random.seed(seed)
    objs = ['a.o', 'b.o', 'c.o', 'd.o', 'u.o', 'e.o', 'f.o']
    libs = ['libs1.a', 'libs2.a', 'libx.a', 'librx.a', 'liby.a', 'lcr.a', 'lc.a', '/lib/libc.a', '-lc', '-lm', '-ly', '-ll']
    flags = ['-s', '-x', '-X', '-S', '-d', '-i', '-Ns', '-Nl', '-w']
    for _ in range(n):
        args = ['/lib/crt0.o'] if random.random() < 0.7 else []
        args += random.sample(objs, random.randint(1, 5))
        for _ in range(random.randint(0, 3)):
            args.insert(random.randint(0, len(args)), random.choice(libs))
        args += [random.choice(libs) for _ in range(random.randint(1, 2))]
        fl = random.sample(flags, random.randint(0, 3))
        if random.random() < 0.15:
            fl.append('-r')
        if random.random() < 0.2:
            fl += ['-u', random.choice(['_bc', '_e1', '_printf', '_d1'])]
        if random.random() < 0.2:
            fl += ['-e', random.choice(['start', '_main', '_b1'])]
        for _ in range(random.choice([0, 0, 0, 1, 2])):
            fl += [random.choice(['-b', '-bt', '-bd', '-bb', '-t', '-tt', '-td', '-tb']),
                   hex(random.choice([0x100, 0x200, 0x1000, 0x2000, 0x4000, 0x8000, 0x9000, 0xa000, 0xf000, 0x123]))]
        yield fl + ['-o', 't'] + args


def main():
    global W
    n, seed = 100, 1
    a = sys.argv[1:]
    while a:
        o = a.pop(0)
        if o == '-n': n = int(a.pop(0))
        elif o == '-s': seed = int(a.pop(0))
    if not ROOT or not os.path.isdir(ROOT):
        sys.exit('set WEGA to the file tree of the original system')
    if not os.path.exists(HOST):
        sys.exit('build ./ld first')
    W = tempfile.mkdtemp(prefix='ldcheck')
    try:
        setup()
        bad = total = 0
        for line in FIXED:
            total += 1
            ok, msg = link(line.split())
            if not ok:
                bad += 1
                print('DIFF', msg)
        for args in rnd(n, seed):
            total += 1
            ok, msg = link(args)
            if not ok:
                bad += 1
                print('DIFF', msg)
        print('ld: %s (%d of %d command lines identical)' % ('identical' if not bad else 'DIFFERENT', total - bad, total))
        return 1 if bad else 0
    finally:
        shutil.rmtree(W, ignore_errors=True)


if __name__ == '__main__':
    sys.exit(main())
