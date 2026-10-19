#!/usr/bin/env python3
"""casyacc.py: rebuild the grammar of the cas parser from the yacc tables of /bin/cas.

    casyacc.py BINARY TRACE... > grammar.txt

The tables (yyr1, yyr2, yychk, ...) are read from the data of the binary. The right hand
sides come from traces of the original run with yydebug set (a copy of cas with the word at
data 0x1fc6 set to 1, run on assembler sources): at every "reduce P" the states on the top of
the stack are known, and the accessing symbol of a state is yychk[state].
"""
import sys, os, re, struct, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sout import SOut

YYEXCA, YYACT, YYPACT, YYPGO, YYR1, YYR2, YYCHK, YYDEF = 0x656, 0x6fa, 0x11fa, 0x14f0, 0x155e, 0x179c, 0x19da, 0x1cd0
NACT, NST, NNT, NPROD = 1408, 379, 55, 287

def data5(path):
    s = SOut(open(path, 'rb').read())
    off = sum(g['code'] for g in s.segs)
    for g in s.segs:
        if g['segno'] == 5:
            return s.image[off:off + g['data']]
        off += g['data']

def words(d, a, n):
    return list(struct.unpack('>%dh' % n, d[a:a + 2 * n]))

def load(binary):
    d = data5(binary)
    t = dict(exca=words(d, YYEXCA, 82), act=words(d, YYACT, NACT), pact=words(d, YYPACT, NST),
             pgo=words(d, YYPGO, NNT), r1=words(d, YYR1, NPROD), r2=words(d, YYR2, NPROD),
             chk=words(d, YYCHK, NST), defl=words(d, YYDEF, NST))
    return t

def traces(files, r2):
    for f in files:
        stack = []
        for line in open(f, errors='replace'):
            m = re.match(r'state (\d+), char 0(\d+)', line)
            if m:
                stack.append(int(m.group(1)))
                continue
            m = re.match(r'reduce (\d+)', line)
            if m:
                p = int(m.group(1))
                yield p, list(stack)
                n = r2[p]
                if n:
                    del stack[len(stack) - n:]
                continue
            if line.startswith('error recovery pops'):
                if stack: stack.pop()

def derive(t, p, trans, pred, stackstates):
    """rhs of production p from the tables: walk back from the states that reduce by p"""
    n = t['r2'][p]
    exca_states = set()
    ex = t['exca']; i = 0
    cand = [s for s in range(NST) if t['defl'][s] == p]
    i = 0
    while i < len(ex):
        if ex[i] == -1:
            st = ex[i + 1]; i += 2
            while i < len(ex) and ex[i] >= 0:
                if ex[i + 1] == p and st not in cand:
                    cand.append(st)
                i += 2
            i += 2
        else:
            i += 1
    results = set()
    for s in cand:
        cur = {s}
        syms = []
        ok = True
        for k in range(n):
            x = {t['chk'][c] for c in cur}
            if len(x) != 1:
                ok = False; break
            syms.append(x.pop())
            if k == n - 1: break
            nxt = set()
            for c in cur:
                for (src, sym) in pred.get(c, ()):
                    if sym == syms[-1]:
                        nxt.add(src)
            if not nxt:
                ok = False; break
            cur = nxt
        if ok:
            results.add(tuple(reversed(syms)))
    return results

def tables_trans(t):
    trans = {}
    pred = collections.defaultdict(set)
    for s in range(NST):
        if t['pact'][s] > -1000:
            for tk in range(1, 128):
                i = t['pact'][s] + tk
                if 0 <= i < NACT:
                    nstate = t['act'][i]
                    if 0 <= nstate < NST and t['chk'][nstate] == tk:
                        trans[(s, tk)] = nstate
                        pred[nstate].add((s, tk))
    for nt in range(1, NNT):
        for s in range(NST):
            j = t['pgo'][nt] + s + 1
            if 0 <= j < NACT:
                nstate = t['act'][j]
                if 0 <= nstate < NST and t['chk'][nstate] == -nt:
                    trans[(s, -nt)] = nstate
                    pred[nstate].add((s, -nt))
    return trans, pred

def main(binary, files):
    t = load(binary)
    rhs = {}
    variants = collections.defaultdict(collections.Counter)
    seen = collections.Counter()
    conflicts = set()
    for p, stack in traces(files, t['r2']):
        n = t['r2'][p]
        if len(stack) < n:
            continue
        syms = tuple(t['chk'][s] for s in stack[len(stack) - n:])
        seen[p] += 1
        variants[p][syms] += 1
    for p, c in variants.items():
        rhs[p] = c.most_common(1)[0][0]
        if len(c) > 1:
            conflicts.add(p)
    def sym(x):
        return 'N%d' % -x if x < 0 else str(x)
    trans, pred = tables_trans(t)
    derived = {}
    for p in range(1, NPROD):
        if p not in rhs and t['r2'][p] > 0:
            r = derive(t, p, trans, pred, None)
            if len(r) == 1:
                derived[p] = next(iter(r))
    ntok = set()
    for p in range(1, NPROD):
        if t['r2'][p] == 0:
            r = ()
        elif p in rhs:
            r = rhs[p]
        elif p in derived:
            r = derived[p]
        else:
            r = None
        lhs = 'N%d' % t['r1'][p]
        if r is None:
            print('%3d  %s : ??? (%d symbols)   [not seen]' % (p, lhs, t['r2'][p]))
        else:
            print('%3d  %s : %s   [%s]' % (p, lhs, ' '.join(sym(x) for x in r) or '/* empty */', seen[p] if p in rhs or not r else 'tables'))
            ntok.update(x for x in r if x > 0)
    print('# conflicts:')
    for p in sorted(conflicts):
        print('#  %d: %s' % (p, '; '.join('%s x%d' % (' '.join(sym(x) for x in k), v) for k, v in variants[p].most_common())))
    print('# not seen:', [p for p in range(1, NPROD) if p not in rhs and t['r2'][p] > 0])
    print('# terminals used:', sorted(ntok))

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2:])
