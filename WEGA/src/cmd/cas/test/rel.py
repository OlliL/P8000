"""Relative jumps, jpr/callr and the ldr family at many distances, and chains of jpr at the limit of
the short form: the original cas and the cas under test must give the same messages and objects.
usage: WEGA=<tree> rel.py <cas under test>"""
import subprocess, sys, os, tempfile, shutil
HERE = os.path.dirname(os.path.abspath(__file__))
HOST = os.path.join(HERE, '..', '..', '..', '..', 'host')
ROOT = os.environ.get('WEGA')                 # file tree of the original system
if not ROOT:
    raise SystemExit('set WEGA=<file tree of the original system>')
EMU = [os.path.join(HOST, 'z8k-emu', 'z8k-emu'), '-r', ROOT]
ORIG = '/bin/cas'                             # the original, run by the emulator
sys.path.insert(0, os.path.join(HOST, 'tools'))
from sout import SOut
CC = ['f','lt','le','ule','ov','mi','eq','c','t','ge','gt','ugt','nov','pl','ne','nc','z','nz','pe','po','ult','uge','nz']
def gen(seg, dists, chains=True, pick=None):
    L = ['.seg'] if seg else []
    L.append('.code')
    k = 0
    def lab():
        nonlocal k; k += 1; return 'L%d' % k
    for d in dists:
        for op in ['jr %s,', 'jr ', 'djnz r5,', 'dbjnz rh3,', 'calr ', 'ldr r5,', 'ldar r5,', 'ldrb rh3,', 'ldrl rr4,', 'jpr %s,', 'jpr ', 'callr ', 'jp %s,', 'call ']:
            for c in (['eq', 'ne'] if '%s' in op else ['']):
                o = op % c if '%s' in op else op
                t = lab()
                L.append('\t%s%s' % (o, t))          # forward
                if d: L.append('\t.blkw\t%d' % d)
                L.append('%s:' % t)
                b = lab()
                L.append('%s:' % b)                  # backward
                if d: L.append('\t.blkw\t%d' % d)
                L.append('\t%s%s' % (o, b))
    # reverse stores: ldr L,r5 etc.
    for d in dists:
        for op in ['ldr %s,r5', 'ldrb %s,rh3', 'ldrl %s,rr4']:
            t = lab(); L.append('\t' + op % t)
            if d: L.append('\t.blkw\t%d' % d)
            L.append(t + ':')
    # cascading jpr chains: n jprs in front of the target, the growth of one decides the next
    for base in ((200, 240, 250, 252, 254) if chains else ()):
        for cnt in (5, 30, 80):
            t = lab()
            for i in range(cnt): L.append('\tjpr\t%s' % t)
            L.append('\t.blkw\t%d' % base)
            L.append(t + ':')
            b = lab(); L.append(b + ':'); L.append('\t.blkw\t%d' % base)
            for i in range(cnt): L.append('\tjpr\t%s' % b)
            # mixed: jprs spanning each other
            a = lab(); c2 = lab()
            L += ['\tjpr\t%s' % c2, '\t.blkw\t%d' % (base // 2), a + ':', '\tjpr\t%s' % a, '\tcallr\t%s' % c2, '\t.blkw\t%d' % (base // 2), c2 + ':']
    return '\n'.join(L) + '\n'

ALL = [0, 1, 2, 10, 100, 126, 127, 128, 129, 130, 250, 254, 255, 256, 257, 258, 1000, 2046, 2047, 2048, 2049, 2050, 3000]

EMU = ['/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu', '-r', '/mnt/share/cvs/P8000/tmp/wega31_root']
def go(cas, f, o):
    r = subprocess.run(EMU + [cas, '-u', '-o', o, f], capture_output=True, text=True, timeout=300)
    try: img = bytes(SOut(open(o, 'rb').read()).image)
    except Exception: img = None
    return r.returncode, r.stderr, img
bad = 0; n = 0; okobj = 0
src = os.path.abspath(sys.argv[1])
os.chdir(tempfile.mkdtemp())
shutil.copy(src, 'cas.new')          # the guest sees only ROOT and the working directory
new = './cas.new'
jobs = [(seg, [d], False) for seg in (0, 1) for d in ALL] + [(seg, [], True) for seg in (0, 1)]
for seg, d, ch in jobs:
    open('t.s', 'w').write(gen(seg, d, ch))
    a = go(ORIG, 't.s', 'o.o'); b = go(new, 't.s', 'n.o')
    n += 1
    if a[2] is not None: okobj += 1
    if a != b:
        bad += 1; print('DIFF seg', seg, 'dist', d, 'chains', ch, 'rc', a[0], b[0], 'obj', a[2] is not None, b[2] is not None, 'msgs equal', a[1] == b[1])
        print('   ', a[1][:150].replace(chr(10), ' | ')); print('   ', b[1][:150].replace(chr(10), ' | '))
print('files', n, 'with object', okobj, 'different', bad)
