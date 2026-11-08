"""Fixtures of the corpus of ld: the test objects test/*.c (built with the original cc), archives (ar, ranlib, test/mklib.py), the fixed\ncommand lines FIXED and the command lines of rnd(n, seed) (a fixed seed gives the same lines every time)."""
import os
import random
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TESTDIR = os.path.join(HERE, 'test')
ROOT = None		# the file tree of the original system, EMU the emulator, W the work directory: set by progcfg.py before the use
EMU = None
W = None


def orig(args, cwd, tool='ld'):
    return subprocess.run([EMU, '-r', ROOT, '/bin/' + tool] + args, cwd=cwd, capture_output=True, timeout=120)



def setup():
    base = os.path.join(W, 'base')
    os.makedirs(base)
    for f in os.listdir(os.path.join(TESTDIR)):
        if f.endswith('.c'):
            shutil.copy(os.path.join(TESTDIR, f), base)
    for f in sorted(os.listdir(base)):
        r = orig(['-c', '-O', f], base, 'cc')
        if r.returncode:
            sys.exit('cc %s failed: %s' % (f, r.stderr.decode()))
    mk = os.path.join(TESTDIR, 'mklib.py')
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
