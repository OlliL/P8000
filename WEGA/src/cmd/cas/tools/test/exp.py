#!/usr/bin/env python3
"""Expressions, numbers, strings and directives: random statements are assembled by the original cas and
by the cas under test; the messages, the return code and the object must be equal.
usage: WEGA=<tree> exp.py <cas under test> [seed] [number of statements]"""
import subprocess, sys, os, re, random, tempfile, shutil
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
sys.path.insert(0, os.path.join(HOST, 'tools'))
from sout import SOut

NUM = ['0', '1', '2', '7', '10', '255', '256', '4660', '65535', '65536', '100000', '2147483647', '%0', '%1', '%7F', '%FF', '%1234', '%FFFF', '%10000', '%7FFFFFFF', '%FFFFFFFF',
       '%(8)17', '%(8)777', '%(8)7726', '%(2)1001', '%(2)11111111', '%(2)0', "'a'", "'\\n'", "'\\0'", "'\\\\'", "'\\''", '%(16)A', '%(3)12', '%(8)8', '%G']
SYM = ['k1', 'k2', 'Lab', 'Lab2', 'ext', 'ext2', '.', 'undef']
BIN = ['+', '-', '*', '/', '^<', '^>', '^|', '^X', '^&']        # the others give `Invalid token` or a syntax error; some loop in the original
UN = ['+', '-', '^S', '^O']

def term(r):
    c = r.random()
    if c < 0.45: return r.choice(NUM)
    if c < 0.85: return r.choice(SYM)
    if c < 0.9: return '<<%d>>%s' % (r.choice([0, 1, 5, 127, 128]), r.choice(['0', '%1234', '%FFFF', 'k1', 'Lab']))
    return '|%s|' % r.choice(['%34', '%FF', 'k1', 'Lab', '%1234'])

def expr(r, d=0):
    c = r.random()
    if d > 3 or c < 0.3: return term(r)
    if c < 0.45: return r.choice(UN) + expr(r, d + 1)
    if c < 0.55: return '(' + expr(r, d + 1) + ')'
    return expr(r, d + 1) + r.choice(BIN) + expr(r, d + 1)

STR = ['"abc"', '"a\\nb"', '"\\%41"', '"\\%7FF"', '"\\0"', '"\\\\"', '"\\""', '""', '"tab\\t"', '"x\\q"', "'abc'", '"a" "b"']

def stmt(r):
    c = r.random(); e = lambda: expr(r)
    if c < 0.12: return '\t.word\t' + ','.join(e() for _ in range(r.randint(1, 3)))
    if c < 0.22: return '\t.long\t' + ','.join(e() for _ in range(r.randint(1, 2)))
    if c < 0.32: return '\t.byte\t' + ','.join((e() if r.random() < 0.7 else r.choice(STR)) for _ in range(r.randint(1, 3)))
    if c < 0.38: return '\t.blk%s\t%s' % (r.choice('bwl'), r.choice(['0', '1', '3', '10', 'k2', 'k1']))
    if c < 0.5: return '\tld\tr1,#' + e()
    if c < 0.56: return '\tldb\trl1,#' + e()
    if c < 0.62: return '\tldl\trr2,#' + e()
    if c < 0.68: return '\tld\tr1,' + e()
    if c < 0.72: return '\tld\tr1,%s(r2)' % e()
    if c < 0.76: return '\tadd\tr1,#' + e()
    if c < 0.78: return '\t.even'
    if c < 0.8: return '\t.odd'
    if c < 0.84: return 'N%d:=\t%s' % (r.randint(0, 9999), e())
    if c < 0.87: return 'M%d::=\t%s' % (r.randint(0, 9999), e())
    if c < 0.9: return '\t.addr\t' + e()
    if c < 0.93: return '\t.comm\t%s,cm%d' % (r.choice(['1', '2', '4', '10', e()]), r.randint(0, 99))
    if c < 0.95: return '\t.even'      # (.line would change the line numbers of the messages)
    if c < 0.97: return '\tsla\tr1,#' + e()
    return '\tinc\tr1,#' + e()

PRE = ['.code', 'k1:=\t%10', 'k2:=\t7', 'Lab:', '\tnop', 'Lab2::', '\tnop', '\t.extern\text, ext2']

def run(cas, src, d):
    open(os.path.join(d, 'x.s'), 'w').write(src)
    try:
        r = subprocess.run(runcmd(cas) +[ '-u', '-o', 'x.o', 'x.s'], capture_output=True, text=True, timeout=120, cwd=d)
    except subprocess.TimeoutExpired:
        return 'hang', '', None
    try: img = open(os.path.join(d, 'x.o'), 'rb').read()
    except Exception: img = None
    if os.path.exists(os.path.join(d, 'x.o')): os.remove(os.path.join(d, 'x.o'))
    return r.returncode, r.stderr, img

def hangs(cas, pre, ss, d):
    open(os.path.join(d, 'h.s'), 'w').write('\n'.join(pre + ss) + '\n')
    try:
        subprocess.run(runcmd(cas) +[ '-u', '-o', 'h.o', 'h.s'], capture_output=True, timeout=20, cwd=d)
        return False
    except subprocess.TimeoutExpired:
        return True

def bisect(cas, pre, ss, d):
    """a statement (the smallest list found) that makes cas run for ever"""
    while len(ss) > 1:
        h = len(ss) // 2
        if hangs(cas, pre, ss[:h], d): ss = ss[:h]
        elif hangs(cas, pre, ss[h:], d): ss = ss[h:]
        else: break
    return ss

def main():
    new = os.path.abspath(sys.argv[1]); seed = int(sys.argv[2]) if len(sys.argv) > 2 else 1; n = int(sys.argv[3]) if len(sys.argv) > 3 else 6000
    d = tempfile.mkdtemp(); shutil.copy(new, os.path.join(d, 'cas.new'))
    r = random.Random(seed)
    stmts = [stmt(r) for _ in range(n)]
    nbad = 0
    for seg in (0, 1):
        pre = (['.seg'] if seg else []) + PRE
        keep = list(range(n))
        for it in range(40):
            src = '\n'.join(pre + [stmts[i] for i in keep]) + '\n'
            a = run('/bin/cas', src, d)
            if a[0] == 'hang':
                h = bisect('/bin/cas', pre, [stmts[i] for i in keep], d)
                print('seg=%d: the original does not terminate on %r (left out)' % (seg, h))
                keep = [i for i in keep if stmts[i] not in h]
                continue
            b = run('./cas.new', src, d)
            if b[0] == 'hang':
                h = bisect('./cas.new', pre, [stmts[i] for i in keep], d)
                print('DIFF seg=%d: the cas under test does not terminate on %r, the original does' % (seg, h)); nbad += 1
                keep = [i for i in keep if stmts[i] not in h]
                continue
            if a[0] != b[0] or a[1] != b[1] or a[2] != b[2]:
                print('DIFF seg=%d: rc %s %s, messages equal %s, objects equal %s' % (seg, a[0], b[0], a[1] == b[1], a[2] == b[2])); nbad += 1
                for x, y in zip(a[1].splitlines(), b[1].splitlines()):
                    if x != y:
                        print('   orig:', x); print('   new: ', y)
                        m = re.search(r'line (\d+):', x)
                        if m: print('   statement:', repr(src.split('\n')[int(m.group(1)) - 1]))
                        break
                break
            lines = set(int(m) for m in re.findall(r'line (\d+):', a[1]))
            base = len(pre) + 1
            bad = {keep[ln - base] for ln in lines if 0 <= ln - base < len(keep)}
            if not bad:
                print('seg=%d: %d statements, %d refused by the original (and by the other the same way), object of %d bytes equal' % (seg, n, n - len(keep), len(a[2] or b'')))
                break
            keep = [i for i in keep if i not in bad]
        else:
            print('seg=%d: not converged' % seg)
    shutil.rmtree(d)
    return nbad

if __name__ == '__main__':
    sys.exit(1 if main() else 0)
