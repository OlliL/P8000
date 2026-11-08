"""Fixed (deterministic) inputs of the cas corpus (compare/cas/progcfg.py), made from tools of the repository:

  opcodes(mode)    every valid opcode as decoded by host/tools/z8k.py, mode ns, ss or sl (nonsegmented, short and long segmented
                   addresses), as (bytes, text, mask)
  fuzz(mode)       operand patterns for the mnemonics z8k.py does not decode (EPU, block instructions ...), mode ns or sl
  reljump(seg, dists, chains)   jr, djnz, calr, ldr, jpr, callr, jp, call at many distances and chains of jpr at the limit of the short form

Nothing here is random: the same inputs every time."""
import functools
import os
import sys
HERE = os.path.dirname(os.path.abspath(__file__))
HOST = os.path.join(HERE, '..', '..', '..', 'host')
sys.path.insert(0, os.path.join(HOST, 'tools'))
import z8k

FILL = {'ns': [0x1234, 0x5678, 0x9ABC], 'ss': [0x0134, 0x5678, 0x9ABC], 'sl': [0x8100, 0x1234, 0x5678]}

@functools.lru_cache(maxsize=None)
def opcodes(mode):
    seg = mode != 'ns'
    out = []
    for w0 in range(0x10000):
        data = b''.join(w.to_bytes(2, 'big') for w in [w0] + FILL[mode] + [0x1111])
        ins = z8k.decode(z8k.Buf(data), 0, seg)
        if ins is None or ins.mn == 'epu':
            continue
        if any(o.kind == 'rel' for o in ins.ops):
            continue                      # relative operands: tested separately (labels)
        mask = bytearray(b'\xff' * (2 * len(ins.words)))
        for o in ins.ops:
            if o.kind == 'imm' and o.b == 8 and o.word is not None:
                mask[2 * o.word] = 0          # the high byte of a byte immediate is not defined
        out.append((b''.join(w.to_bytes(2, 'big') for w in ins.words), z8k.text(ins), bytes(mask)))
    return out

POOL = ['r5','r0','rh3','rl3','rr4','rq4','@r5','@rr4','#5','#%1234','#1','#%12','%1234','|%12|','%1234(r5)','r5(#%1234)','rr4(#%1234)','r5(r6)','rr4(r6)','eq','ne','nz','c','z','f3','f0','fcw','vi','nvi','x','x(r5)','#x','x+2']
P3 = ['r5','@r5','@rr4','#3','eq','rr4','f2']
P4 = ['r5','@r5','eq','#3']

@functools.lru_cache(maxsize=None)
def fuzz(mode):
    import itertools, re as _re
    rows = _re.findall(r'\{ (\d+), (\d+), (\d+), [^,]+, "([^"]+)" \}', open(os.path.join(HERE, '..', '..', '..', 'src', 'cmd', 'cas', 'pstdata.h')).read())
    skip = {51, 52, 53, 54, 55, 58, 59, 60, 61}
    names = [(n, int(t)) for t, b, k, n in rows if not n.startswith('.') and int(t) not in skip]
    out = []
    P2 = POOL[:26]
    for n, t in names:
        pats = [()] + [(a,) for a in POOL] + list(itertools.product(P2, repeat=2)) + list(itertools.product(P3, repeat=3))
        if t == 56: pats += list(itertools.product(P4, repeat=4))
        for p in pats:
            out.append((b'', n + ('\t' + ','.join(p) if p else ''), b''))
    return out


CC = ['f','lt','le','ule','ov','mi','eq','c','t','ge','gt','ugt','nov','pl','ne','nc','z','nz','pe','po','ult','uge','nz']
def reljump(seg, dists, chains=True, pick=None):
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
