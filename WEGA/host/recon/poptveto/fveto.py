#!/usr/bin/env python3
"""fveto.py - fast parallel veto search on one function with jfast (spopt + host cas, no model link).

  fveto.py SPOPT RAW.s DEPTH [FUNC FIDX] [start=k,k;k]   SPOPT: a veto spopt (built with -DVETO), RAW.s from `sccvc -S`
Each slot is a small root (FVETO_DIR, default $TMPDIR/fveto) holding lib/vc/spopt and tmp/ (veto.txt, veto.log).
Breadth first over the events logged by the runs (any event of the function may be vetoed); prints the best per depth."""
import os, shutil, sys
from concurrent.futures import ThreadPoolExecutor
import queue
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import jfast  # noqa: E402
N = 8
D = os.environ.get('FVETO_DIR', os.path.join(os.environ.get('TMPDIR', '/tmp'), 'fveto'))
free = queue.Queue()


def setup(spopt):
    for i in range(N):
        r = '%s/s%d' % (D, i)
        shutil.rmtree(r, ignore_errors=True)
        os.makedirs(r + '/lib/vc'); os.makedirs(r + '/tmp')
        shutil.copy(spopt, r + '/lib/vc/spopt')
        free.put(i)


def run(raw, func, fidx, V):
    i = free.get()
    try:
        r = '%s/s%d' % (D, i)
        open(r + '/tmp/veto.txt', 'w').write(' '.join(str(x) for x in [fidx] + sorted(V)) + '\n')
        try:
            os.remove(r + '/tmp/veto.log')
        except OSError:
            pass
        n, _ = jfast.ours(r + '/lib/vc/spopt', raw, func)
        ev = []
        try:
            for k_, l in enumerate(open(r + '/tmp/veto.log')):
                if k_ >= 400:
                    break
                x = l.split()
                if len(x) >= 4 and x[0] == str(fidx):
                    ev.append((int(x[1]), x[2], int(x[3])))
        except OSError:
            pass
        return tuple(sorted(V)), n, ev
    finally:
        free.put(i)


def bfs(raw, depth, func='jpr', fidx=0, start=None):
    seen = {}
    front = start or [()]
    best_all = (9999, None)
    with ThreadPoolExecutor(N) as ex:
        for d in range(depth + 1):
            res = list(ex.map(lambda V: run(raw, func, fidx, list(V)), front))
            for V, n, ev in res:
                seen[V] = (n, ev)
            best = sorted((abs(n) if n is not None else 9999, V) for V, n, ev in res)
            print('depth', d, 'runs', len(res), 'best', best[:6], flush=True)
            if best and best[0][0] < best_all[0]:
                best_all = best[0]
            if best and best[0][0] == 0 and seen[best[0][1]][0] == 0:
                return best[0]
            nxt = set()
            for V, n, ev in res:
                if n is None:
                    continue
                for k, t, lab in ev:
                    if k not in V:
                        W = tuple(sorted(V + (k,)))
                        if W not in seen:
                            nxt.add(W)
            front = sorted(nxt)
    return best_all


if __name__ == '__main__':
    a = [x for x in sys.argv[1:] if not x.startswith('start=')]
    st = [x for x in sys.argv[1:] if x.startswith('start=')]
    start = [tuple(int(k) for k in g.split(',') if k) for g in st[0][6:].split(';')] if st else None
    setup(a[0])
    print('RESULT', bfs(a[1], int(a[2]), a[3] if len(a) > 3 else 'jpr', int(a[4]) if len(a) > 4 else 0, start))
