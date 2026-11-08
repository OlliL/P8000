"""Fixtures of the corpus of sld: the segmented test objects (test/*.c with the original scc, test/*.s with cas), the archive libs1.a\n(test/mklib.py), the kernel objects of the system, the fixed command lines SFIXED and rnd_seg(n, seed) (a fixed seed)."""
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



def setup_seg():
    base = os.path.join(W, 'sbase')
    os.makedirs(base)
    for f in os.listdir(os.path.join(TESTDIR)):
        if f.endswith('.c') or f.endswith('.s'):
            shutil.copy(os.path.join(TESTDIR, f), base)
    for f in sorted(os.listdir(base)):
        if f.endswith('.c'):
            r = orig(['-c', '-O', f], base, 'scc')
        else:
            r = orig(['-u', '-o', f[:-2] + '.o', f], base, 'cas')
        if r.returncode:
            sys.exit('%s failed: %s' % (f, r.stderr.decode()))
    mk = os.path.join(TESTDIR, 'mklib.py')
    subprocess.run(['python3', mk, 'libs1.a', 'b.o', 'c.o', 'd.o', 'u.o', 'e.o', 'f.o'], cwd=base, check=True, capture_output=True)
    # the kernel link of make.wega: the objects of the system, a ver.o of our own
    sysdir = os.path.join(ROOT, 'usr/sys')
    if os.path.isdir(sysdir):
        for n in ('wpar.o', 'event.o', 'mch.o', 'u.o', 'fpe.o', 'conf.o'):
            shutil.copy(os.path.join(sysdir, 'conf', n), os.path.join(base, 'k_' + n))
        shutil.copy(os.path.join(sysdir, 'sys/LIB1'), os.path.join(base, 'LIB1'))
        shutil.copy(os.path.join(sysdir, 'dev/LIB2'), os.path.join(base, 'LIB2'))
        open(os.path.join(base, 'ver.c'), 'w').write('char version[] = "WEGA test";\nchar copyrit[] = "c";\n'
                                                      'char utsrelease[] = "r";\nint cpu2_flags;\n')
        orig(['-c', 'ver.c'], base, 'scc')


SFIXED = [
    '-o t /lib/scrt0.o a.o b.o u.o -lc', '-X -e start -z -o t /lib/scrt0.o a.o b.o u.o -lc', '-i -o t /lib/scrt0.o a.o b.o u.o -lc',
    '-i -z -o t /lib/scrt0.o a.o b.o u.o -lc', '-Mt4 -o t /lib/scrt0.o a.o b.o u.o -lc', '-Md4 -o t /lib/scrt0.o a.o b.o u.o -lc',
    '-M5 -o t /lib/scrt0.o a.o b.o u.o -lc', '-i -M5 -Mt4 -o t /lib/scrt0.o a.o b.o u.o -lc', '-Ms62 -o t /lib/scrt0.o a.o b.o u.o -lc',
    '-R9 -z -o t /lib/scrt0.o a.o b.o u.o -lc', '-M7 -Md1 -o t /lib/scrt0.o a.o b.o u.o -lc', '-Md2 -Mb3 -o t /lib/scrt0.o a.o b.o u.o -lc',
    '-x -o t /lib/scrt0.o a.o b.o u.o -lc', '-X -S -Ns -o t /lib/scrt0.o a.o b.o u.o -lc', '-w -o t /lib/scrt0.o a.o libs1.a u.o -lc',
    '-r -o t a.o b.o', '-r -d -o t a.o b.o c.o', '-r -i -z -o t a.o big1.o', '-r -x -o t /lib/scrt0.o a.o',
    '-r -i -d -o t a.o b.o big2.o big5.o', '-o t /lib/scrt0.o a.o b.o -lc', '-Mt4 -o t a.o b.o', '-i -o t a.o b.o big5.o',
    '-i -o t /lib/scrt0.o a.o b.o u.o big1.o big2.o big3.o big4.o -lc', '-Mt3 -Md9 -Mb11 -o t /lib/scrt0.o a.o b.o u.o big1.o big2.o big3.o big4.o -lc',
    '-o t /lib/scrt0.o a.o b.o u.o big5.o big6.o big7.o big8.o big1.o -lc', '-i -o t /lib/scrt0.o a.o b.o u.o big5.o big6.o big7.o big8.o big1.o -lc',
    '-M4 -o t /lib/scrt0.o a.o b.o u.o big5.o big6.o big7.o big8.o big1.o -lc',
    '-o t al1.o', '-i -o t al1.o', '-Md3 -o t al1.o', '-Mt3 -o t al1.o', '-i -o t al2.o', '-o t al2.o', '-i -Ms62 -o t al1.o', '-R5 -o t al1.o',
    '-o t abs1.o', '-e _f1 -o t abs1.o', '-Q', '-o t nofile.o',
    '-Ns -o wega -e start -X -i -Ms62 k_wpar.o k_event.o k_mch.o k_u.o ver.o k_fpe.o k_conf.o LIB1 LIB2',
]


def rnd_seg(n, seed):
    random.seed(seed)
    extra = ['c.o', 'e.o', 'big3.o', 'big4.o', 'big5.o', 'big7.o']
    for _ in range(n):
        objs = ['a.o', 'b.o', 'u.o'] + random.sample(extra, random.randint(0, 3))
        random.shuffle(objs)
        args = ['/lib/scrt0.o'] + objs + ['-lc']
        fl = []
        if random.random() < 0.5: fl.append('-i')
        if random.random() < 0.5: fl.append('-z')
        fl += random.sample(['-x', '-X', '-S', '-Ns', '-w'], random.randint(0, 2))
        if random.random() < 0.6: fl += ['-e', 'start']
        for _ in range(random.choice([0, 0, 1, 2])):     # -M options in front of the files
            fl.append(random.choice(['-M', '-Mt', '-Md', '-Mb']) + str(random.choice([0, 1, 2, 3, 4, 5, 7, 9, 11, 20, 62])))
        if random.random() < 0.15 and '-z' not in fl: fl.append('-Ms' + str(random.choice([40, 62, 100])))
        if random.random() < 0.1: fl.append('-R' + str(random.choice([30, 50])))
        yield fl + ['-o', 't'] + args
