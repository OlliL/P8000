"""Fixtures of the corpus of gld: the objects and archives of sld (setup_seg of ../sld/fixtures.py) and those of its own (gx1 ... gx4,\npsec1, archives, damaged objects), the fixed command lines GFIXED and rnd_gld(n, seed) (a fixed seed)."""
import os
import random
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TESTDIR = os.path.join(HERE, '..', 'ld', 'test')
ROOT = None		# the file tree of the original system, EMU the emulator, W the work directory: set by progcfg.py before the use
EMU = None
W = None


def orig(args, cwd, tool='ld'):
    return subprocess.run([EMU, '-r', ROOT, '/bin/' + tool] + args, cwd=cwd, capture_output=True, timeout=120)



def setup_gld():
    base = os.path.join(W, 'sbase')
    sh = lambda *a: subprocess.run(list(a), cwd=base, check=True, capture_output=True)
    for n, src in (('gx1', 'gx1.o'), ('gx2', 'gx2.o')):
        pass
    orig(['r', 'libgp.a', 'gx2.o', 'gx3.o', 'gx4.o', 'b.o', 'c.o', 'd.o', 'u.o', 'e.o', 'f.o'], base, 'ar')
    shutil.copy(os.path.join(base, 'libgp.a'), os.path.join(base, 'libgq.a'))
    subprocess.run([EMU, '-r', ROOT, '/usr/bin/ranlib', 'libgq.a'], cwd=base, capture_output=True)
    shutil.copy(os.path.join(base, 'libgq.a'), os.path.join(base, 'libgo.a'))
    t = os.stat(os.path.join(base, 'libgo.a')).st_mtime + 100000      # older than the archive: table out of date
    os.utime(os.path.join(base, 'libgo.a'), (t, t))
    open(os.path.join(base, 'bad1.o'), 'w').write('hello world\n')
    open(os.path.join(base, 'bad2.o'), 'w').close()
    d = bytearray(open(os.path.join(base, 'gx1.o'), 'rb').read())
    d[5] |= 1
    open(os.path.join(base, 'bad3.o'), 'wb').write(d)
    d = bytearray(open(os.path.join(base, 'gx1.o'), 'rb').read())
    d[12:14] = b'\0\0'
    open(os.path.join(base, 'bad4.o'), 'wb').write(d)


GFIXED = [
    '/lib/scrt0.o a2.o b.o c.o d.o u.o /lib/slibc.a', '/lib/scrt0.o a2.o b.o c.o d.o u.o /lib/slibc.a -o t', '/lib/scrt0.o a2.o b.o c.o d.o u.o /lib/slibc.a -s',
    '/lib/scrt0.o a2.o b.o c.o d.o u.o /lib/slibc.a -e start', '/lib/scrt0.o a2.o b.o c.o d.o u.o /lib/slibc.a -t', '/lib/scrt0.o a2.o b.o c.o d.o u.o /lib/slibc.a -M',
    '/lib/scrt0.o a2.o b.o c.o d.o u.o -lc', '/lib/scrt0.o a2.o b.o c.o d.o u.o -L/lib -lc', '/lib/scrt0.o a2.o b.o c.o d.o u.o /lib/slibc.a -w',
    '/lib/scrt0.o a2.o b.o c.o e.o f.o d.o u.o /lib/slibc.a', '/lib/scrt0.o a2.o b.o c.o e.o f.o d.o u.o /lib/slibc.a -w', 'a2.o b.o c.o d.o u.o', 'a2.o b.o',
    '-e nosuch a2.o b.o c.o d.o u.o', 'nosuch.o', '-q a2.o', '-o', '-e', '-L', 'gx1.o gx2.o', 'gx2.o gx1.o', 'gx1.o gx2.o gx3.o', 'gx1.o gx3.o gx2.o',
    'gx1.o gx2.o gx3.o -w', 'gx1.o gx2.o gx4.o', 'gx1.o gx2.o gx3.o gx4.o', 'gx4.o gx3.o gx1.o -e _x4', 'gx4.o gx3.o gx1.o -e _xd', 'psec1.o', 'gx1.o gx2.o -o',
    'gx1.o gx2.o -lnone', 'gx1.o gx2.o -sl', 'gx1.o gx2.o -L/usr', 'gx1.o gx2.o -Lx', 'gx1.o gx2.o -Zq', 'gx1.o gx2.o -e _gx1 -s', 'gx3.o gx3.o',
    'gx1.o libgp.a', 'gx1.o libgq.a', 'gx1.o libgo.a', 'gx1.o libs1.a', 'gx1.o libgp.a -M', 'gx1.o libgq.a -M', 'gx1.o libgo.a -M',
    'gx4.o gx1.o libgq.a gx3.o', 'gx4.o gx1.o libgq.a gx3.o -w', 'gx4.o gx1.o libgp.a gx3.o', 'a2.o libgp.a', 'a2.o libgq.a', 'a2.o libs1.a libgp.a',
    '/lib/scrt0.o a2.o libgq.a /lib/slibc.a', '/lib/scrt0.o a2.o libs1.a /lib/slibc.a -M',
    'bad1.o', 'bad2.o', 'bad3.o gx2.o', 'bad4.o gx2.o',
]


def rnd_gld(n, seed):
    random.seed(seed)
    objs = ['gx1.o', 'gx2.o', 'gx3.o', 'gx4.o', 'a2.o', 'b.o', 'c.o', 'd.o', 'e.o', 'f.o', 'u.o']
    libs = ['libgp.a', 'libgq.a', 'libgo.a', 'libs1.a', '/lib/slibc.a']
    for _ in range(n):
        args = ['/lib/scrt0.o'] if random.random() < 0.6 else []
        args += [random.choice(objs) if random.random() < 0.7 else random.choice(libs) for _ in range(random.randint(1, 6))]
        if random.random() < 0.5:
            args.append('/lib/slibc.a')
        for o in ('-s', '-w', '-t', '-M'):
            if random.random() < 0.1:
                args.insert(random.randint(0, len(args)), o)
        if random.random() < 0.15:
            args += ['-e', random.choice(['start', '_x1', '_xd', '_main', '_x4'])]
        yield args
