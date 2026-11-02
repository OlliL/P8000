#!/usr/bin/env python3
"""castrig/codgen_test.py: the independent test of the sum rule of the old cas on /lib/codgen, with the values fixed on cparse and
popt (OLDCC_LOG.md 2026-11-02): stoff + name bytes + 12 per jpr/callr, threshold 7053, reset at each trigger. HC, CAS_J as for
offsets.py; the _old.s modules of oldcc.sh model codgen. The sites of an open function map by their number: near, not exact."""
import bisect
src = open('/mnt/share/cvs/P8000/WEGA/host/recon/castrig/offsets.py').read().split("test(2,'st'")[0]
src = src.replace("rows.append((a,int(J[n][2]),int(J[n][3]),int(J[n][4]),int(J[n][5]),cur))",
                  "rows.append((a,int(J[n][2]),int(J[n][3]),int(J[n][4]),int(J[n][5]),cur,n))")
a = src.index('MODS=[')
b = src.index('\n', a)
src = src[:a] + "MODS=[('codgen',m,m+'.c') for m in 'reader3 rdr3 local3 order3 match3 allo3 regallo3 comm3 wrt3 regs3 lcl3 tr3 optim3'.split()]" + src[b:]
exec(src)
W, T = (0, 0, 10, 1, 0, 12), 7053


def run(w, t, detail=False):
    tot = {'long': 0, 'short': 0, 'miss': 0, 'false': 0}
    for m in MODS:
        S, LL, rows = D[m]
        trig, base = [], 0
        for r in rows:
            if r[0] is None:
                continue
            f = (r[4], (r[1] - 2 - 14 * r[4]) / 6.0, r[2] / 10.0, r[3], 0, r[6])
            x = sum(a * b for a, b in zip(w, f))
            if x - base >= t:
                trig.append(r[0])
                base = x
        def spans(a, e):
            i = bisect.bisect_right(trig, a)
            return i < len(trig) and trig[i] < e
        miss = sum(1 for a, e in LL if not spans(a, e))
        false = sum(1 for a, e in S if spans(a, e))
        tot['long'] += len(LL); tot['short'] += len(S); tot['miss'] += miss; tot['false'] += false
        if detail:
            print('%-9s long %3d short %3d  miss %3d false %3d  sites %4d triggers %s' % (
                m[1], len(LL), len(S), miss, false, len([r for r in rows if r[0]]), [hex(x) for x in trig]))
    return tot


print('fixed rule, threshold', T)
print(run(W, T, True))
print('no trigger: miss = long sites, false 0')
print('for information only (not a fit to take over): the same form at other thresholds')
print([(t, (lambda r: r['miss'] + r['false'])(run(W, t))) for t in range(3000, 11001, 1000)])
print('other thresholds of the neighbourhood', [(t, (lambda r: r['miss'] + r['false'])(run(W, t))) for t in range(6853, 7254, 50)])
