#!/usr/bin/env python3
"""tmpclash.py LOG: clashes of guest temporary files in a Z8K_TMPLOG of z8k-emu (host/z8k-emu/wega.c, tmplog).

Every process belongs to the tree of the guest process it was forked from (lines "fork child PID"); a tree starts with a
"start" line. A clash: a file below /tmp is created (creat) by one tree while another tree has it created and not yet
removed, or opened by a tree that did not create it while another tree holds it. Also lists the guest pids that two
trees alive at the same time have got from getpid (the cause of a clash of names like /tmp/cc.<pid>.x).
Prints one line per finding and a summary line.
"""
import sys
from collections import defaultdict


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    parent, root = {}, {}
    life = {}                       # tree -> [first time, last time]
    live = {}                       # path -> (tree, time of creat)
    gpids = defaultdict(list)       # guest pid -> [(tree, time)]
    clashes = 0
    n = 0

    def tree(pid):
        r = pid
        while r in parent:
            r = parent[r]
        return r

    lines = [l.split() for l in open(sys.argv[1], errors='replace')]
    for f in lines:                 # first all forks: a child may log before its parent logs the fork
        if len(f) >= 5 and f[2] == 'fork':
            parent[int(f[4])] = int(f[1])
    lines.sort(key=lambda f: float(f[0]) if f else 0)
    for f in lines:
        if len(f) < 3:
            continue
        n += 1
        t, pid, ev = float(f[0]), int(f[1]), f[2]
        tr = tree(pid)
        lf = life.setdefault(tr, [t, t])
        lf[1] = t
        if ev == 'getpid' and len(f) >= 5:
            gpids[f[4]].append((tr, t))
        elif ev == 'creat' and len(f) >= 4 and f[-1] != '->' and not f[-1].startswith('-'):
            path = f[3]
            if not path.startswith('/tmp/'):
                continue
            if path in live and live[path][0] != tr:
                o, ot = live[path]
                print('clash creat %s: tree %d at %.6f, held by tree %d since %.6f' % (path, tr, t, o, ot))
                clashes += 1
            live[path] = (tr, t)
        elif ev == 'unlink' and len(f) >= 4:
            path = f[3]
            if not path.startswith('/tmp/'):
                continue
            if path in live and live[path][0] != tr:
                o, ot = live[path]
                print('clash unlink %s: tree %d at %.6f, created by tree %d at %.6f' % (path, tr, t, o, ot))
                clashes += 1
            live.pop(path, None)
        elif ev == 'open' and len(f) >= 4:
            path = f[3]
            if path in live and live[path][0] != tr:
                o, ot = live[path]
                print('clash open %s: tree %d at %.6f, created by tree %d at %.6f' % (path, tr, t, o, ot))
                clashes += 1
    same = 0
    for g, uses in gpids.items():
        trees = {}
        for tr, t in uses:
            trees.setdefault(tr, t)
        if len(trees) < 2:
            continue
        ts = sorted(trees.items(), key=lambda x: x[1])
        for (a, ta), (b, tb) in zip(ts, ts[1:]):
            if life[a][1] >= tb:     # tree a still alive when b got the same guest pid
                print('same guest pid %s: tree %d (%.6f..%.6f) and tree %d (from %.6f)' % (g, a, life[a][0], life[a][1], b, tb))
                same += 1
    print('tmpclash: %d lines, %d trees, %d clashes, %d guest pids shared by living trees' % (n, len(life), clashes, same))


main()
