#!/usr/bin/env python3
"""castrig/feas.py DS.json: is there a sum of counters with small whole weights and one threshold that is reached inside the free part
of every first window? With non-negative weights every sum grows, so a threshold exists exactly when the largest value just in front of a window
is smaller than the smallest value at its end (an interval test, no grid over thresholds). Reads dataset.py."""
import json,sys,itertools
from math import gcd
from functools import reduce
D=json.load(open(sys.argv[1]))
F=['ent','open','zero','nz','longd','nj','nc','r0','r1','r2','sites','jprs','callrs','labn','labg','labl','comm','assign','ins','stmts','lines','byte','word','even','sect','names','it','st','nt','lc','ifp','sfp','dep','idg','idb','idl','idt','blk']
for m in D:
    a={k:0 for k in ('r0','r1','r2','sites','jprs','callrs')}
    for r in m['rows']:
        r.update(a)
        a['sites']+=1
        if r['tok']==80: a['jprs']+=1
        else: a['callrs']+=1
        if r['res']==0: a['r0']+=1
        elif r['res']==1: a['r1']+=1
        else: a['r2']+=1
        r.setdefault('dep',0)
        for k in ('open','zero','nz','longd','nj','nc'): r.setdefault(k,0)
def wins(m):
    w=[]
    for a,t in sorted(m['long'],key=lambda x:x[1]):
        if w and a<w[-1][1] and t>w[-1][0]: w[-1]=[max(w[-1][0],a),min(w[-1][1],t)]
        else: w.append([a,t])
    return w
C=[]
for m in D:
    ws=wins(m)
    if not ws: continue
    rs=m['rows']
    lo,hi=ws[0]
    idx=[i for i,r in enumerate(rs) if r['a'] is not None and lo<=r['a']<hi and not any(a<r['a']<t for a,t in m['short'])]
    if not idx: print('no free',m['prog'],m['mod']); continue
    i0,i1=idx[0],idx[-1]
    pre=rs[i0-1] if i0>0 else {k:0 for k in F}
    C.append((m['prog'],m['mod'],[pre[k] for k in F],[rs[i1][k] for k in F],rs[i0]['ent'],rs[i1]['ent']))
print(len(C),'first windows:',[(c[0][:2]+c[1],c[4],c[5]) for c in C])
exc=set(sys.argv[2].split(',')) if len(sys.argv)>2 else set()
C=[c for c in C if c[1] not in exc and c[0]+':'+c[1] not in exc]
print(len(C),'windows used')
n=len(F)
A=[[c[2][i] for c in C] for i in range(n)]
B=[[c[3][i] for c in C] for i in range(n)]
W=len(C)
res=[]
for k in (1,2,3):
    for sub in itertools.combinations(range(n),k):
        a=[A[i] for i in sub]; b=[B[i] for i in sub]
        for w in itertools.product(range(1,9),repeat=k):
            if k>1 and reduce(gcd,w)>1: continue
            if k==1:
                l=max(a[0]);u=min(b[0])
            elif k==2:
                x,y=w;p,q=a;r,t=b
                l=max(x*p[j]+y*q[j] for j in range(W));u=min(x*r[j]+y*t[j] for j in range(W))
            else:
                x,y,z=w;p,q,e=a;r,t,f=b
                l=max(x*p[j]+y*q[j]+z*e[j] for j in range(W));u=min(x*r[j]+y*t[j]+z*f[j] for j in range(W))
            if l<u: res.append(((u-l)/max(u,1),[F[i] for i in sub],w,l,u))
res.sort(key=lambda x:-x[0])
print(len(res),'feasible of up to 3 counters, weights 1..8')
for r in res[:25]: print('%.3f'%r[0],r[1],r[2],r[3],r[4])
