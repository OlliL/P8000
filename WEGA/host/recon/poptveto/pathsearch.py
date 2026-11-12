"""pathsearch.py RAW.s TARGET.s [maxexp] : best-first search over spopt primitives (any order) from the raw code of _jpr
to the target layout. Nodes: ('L',n) label, ('J',n) jump, ('C',cond,n) conditional jump, ('R',) ret, ('I',text) other."""
import sys, re, heapq, difflib, itertools

import os
RETSTOP = os.environ.get('RETSTOP') == '1'
WIN = int(os.environ.get('WIN', '1000'))
REV = {'eq': 'ne', 'ne': 'eq', 'lt': 'ge', 'ge': 'lt', 'le': 'gt', 'gt': 'le', 'ult': 'uge', 'uge': 'ult',
       'ule': 'ugt', 'ugt': 'ule', 'z': 'nz', 'nz': 'z', 'c': 'nc', 'nc': 'c', 'mi': 'pl', 'pl': 'mi'}


def parse(path):
    s = open(path).read()
    a = s.index('_jpr::')
    b = s.index('\n\t}', a)
    out = []
    for l in s[a:b].split('\n')[1:]:
        if not l.strip() or l.startswith('/*') or l.strip() in ('{',) or ':=' in l:
            continue
        m = re.match(r'^L(\d+):$', l)
        if m:
            out.append(('L', int(m.group(1))))
            continue
        x = l.strip().split('\t')
        if x[0] == 'jpr' or x[0] == 'jp':
            arg = x[1]
            if ',' in arg:
                c, t = arg.split(',')
                out.append(('C', c, int(t[1:])))
            else:
                out.append(('J', int(arg[1:])))
        elif x[0] == 'ret':
            out.append(('R',))
        else:
            out.append(('I', ' '.join(x)))
    return tuple(out)


def labpos(st):
    return {n[1]: i for i, n in enumerate(st) if n[0] == 'L'}


def refs(st):
    r = {}
    for n in st:
        if n[0] == 'J':
            r[n[1]] = r.get(n[1], 0) + 1
        elif n[0] == 'C':
            r[n[2]] = r.get(n[2], 0) + 1
    return r


def tgt(n):
    return n[1] if n[0] == 'J' else n[2]


def nonlab(st, i):
    while i < len(st) and st[i][0] == 'L':
        i += 1
    return i


def labels_after(st, i):
    """labels directly after position i"""
    out = []
    j = i + 1
    while j < len(st) and st[j][0] == 'L':
        out.append(st[j][1])
        j += 1
    return out


def cleanup(st):
    """eager: chain retarget, dead code after J/R, unreferenced labels removed (D49: kept in front of another label
    unless that label is L1xxxx)."""
    st = list(st)
    changed = True
    while changed:
        changed = False
        lp = labpos(st)
        for i, n in enumerate(st):
            if n[0] in 'JC':
                t = tgt(n)
                if t in lp:
                    k = nonlab(st, lp[t])
                    if k < len(st) and st[k][0] == 'J' and st[k][1] != t:
                        st[i] = ('J', st[k][1]) if n[0] == 'J' else ('C', n[1], st[k][1])
                        changed = True
        i = 0
        while i < len(st):
            if st[i][0] in 'JR':
                j = i + 1
                while j < len(st) and st[j][0] != 'L':
                    j += 1
                if j > i + 1:
                    del st[i + 1:j]
                    changed = True
            i += 1
        r = refs(st)
        i = 0
        while i < len(st):
            n = st[i]
            if n[0] == 'L' and r.get(n[1], 0) == 0 and n[1] not in (10000, 10001, 10002):
                nxt = st[i + 1] if i + 1 < len(st) else None
                if nxt and nxt[0] == 'L' and not (10000 <= nxt[1] < 20000):
                    i += 1
                    continue
                del st[i]
                changed = True
                continue
            i += 1
    return tuple(st)


def moves(st, newlab):
    lp = labpos(st)
    res = []
    for i, n in enumerate(st):
        # turn: C c,L1 ; J L2 ; L1:
        if n[0] == 'C' and i + 1 < len(st) and st[i + 1][0] == 'J' and n[2] in labels_after(st, i + 1):
            s2 = list(st)
            s2[i] = ('C', REV[n[1]], st[i + 1][1])
            del s2[i + 1]
            res.append(('turn', i, tuple(s2)))
        # jump to next label
        if n[0] in 'JC' and tgt(n) in labels_after(st, i):
            s2 = list(st)
            del s2[i]
            res.append(('jnext', i, tuple(s2)))
        if n[0] != 'J' or n[1] not in lp:
            continue
        L = lp[n[1]]
        k = L
        while k > 0 and st[k - 1][0] == 'L':
            k -= 1
        prev = st[k - 1] if k > 0 else None
        if prev and prev[0] in 'JR':
            # codemove: block from k to first J at/after L
            e = L
            while e < len(st) and st[e][0] != 'J':
                e += 1
            if e < len(st) and e != i and not (k <= i <= e) and not (RETSTOP and any(x[0] == 'R' for x in st[k:e + 1])):
                blk = st[k:e + 1]
                s2 = list(st)
                if i < k:
                    s2 = s2[:i] + list(blk) + s2[i + 1:k] + s2[e + 1:]
                else:
                    s2 = s2[:k] + s2[e + 1:i] + list(blk) + s2[i + 1:]
                res.append(('move', i, tuple(s2)))
            if e == i:
                res += ivloop(st, i, k, newlab, 'iv')
        else:
            if L < i:
                res += ivloop(st, i, k, newlab, 'inv')
    return res


def ivloop(st, i, k, newlab, kind):
    """inversion at jump i (target block starts at k, before i): next label N after i; find C to N in [k, i)"""
    after = labels_after(st, i)
    if not after:
        return []
    N = after[0]
    out = []
    for c in range(k, i):
        if sum(1 for x in st[k:c + 1] if x[0] != 'L') > WIN:
            break
        if st[c][0] == 'C' and st[c][2] == N:
            seg = list(st[k:c + 1])
            seg[-1] = ('C', REV[st[c][1]], newlab)
            jump = st[i]
            rest_after_seg = list(st[c + 1:i])
            s2 = list(st[:k]) + [jump, ('L', newlab)] + rest_after_seg + seg + list(st[i + 1:])
            out.append((kind, i, tuple(s2)))
            break
    return out


def key(st):
    """layout key: instruction texts; a jump is shown with the text of the first instructions at its target"""
    lp = labpos(st)
    def at(lab, d=0):
        if lab not in lp:
            return '?'
        k = nonlab(st, lp[lab])
        if k >= len(st):
            return 'END'
        n = st[k]
        if n[0] == 'I':
            nx = nonlab(st, k + 1)
            return n[1] + ('|' + (st[nx][1] if nx < len(st) and st[nx][0] == 'I' else st[nx][0] if nx < len(st) else 'END'))
        if n[0] == 'R':
            return 'R'
        if d < 2:
            return n[0] + '>' + at(tgt(n), d + 1)
        return n[0]
    out = []
    for n in st:
        if n[0] == 'L':
            continue
        if n[0] == 'J':
            out.append('J>' + at(n[1]))
        elif n[0] == 'C':
            out.append('C' + n[1] + '>' + at(n[2]))
        elif n[0] == 'R':
            out.append('R')
        else:
            out.append(n[1])
    return out


def dist(a, b):
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    return sum(max(i2 - i1, j2 - j1) for op, i1, i2, j1, j2 in sm.get_opcodes() if op != 'equal')


def main():
    st0 = cleanup(parse(sys.argv[1]))
    goal = key(cleanup(parse(sys.argv[2])))
    maxexp = int(sys.argv[3]) if len(sys.argv) > 3 else 200000
    cnt = itertools.count()
    h0 = dist(key(st0), goal)
    pq = [(h0, 0, next(cnt), st0, ())]
    seen = {st0}
    best = (h0, ())
    exp = 0
    while pq and exp < maxexp:
        h, g, _, st, path = heapq.heappop(pq)
        exp += 1
        if h < best[0]:
            best = (h, path)
            print('best', h, 'depth', len(path), path[-6:], flush=True)
        if dist(key(st), goal) == 0:
            print('FOUND', path)
            return
        for kind, i, s2 in moves(st, 30000 + len(path)):
            s2 = cleanup(s2)
            if s2 in seen:
                continue
            seen.add(s2)
            h2 = dist(key(s2), goal)
            heapq.heappush(pq, (h2 + float(__import__('os').environ.get('PEN','0.2')) * (len(path) + 1), len(path) + 1, next(cnt), s2, path + ((kind, i),)))
    print('END', exp, 'best', best)


if __name__ == '__main__':
    main()
