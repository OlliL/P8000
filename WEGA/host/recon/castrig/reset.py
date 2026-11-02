#!/usr/bin/env python3
"""castrig/reset.py: a weighted counter with reset at each trigger, all modules of cparse and popt, scored against the forms of the originals. CAST, CAS_J, HC as fit.py."""
import os,sys,bisect,itertools,math
src=open('/mnt/share/cvs/P8000/WEGA/host/recon/castrig/offsets.py').read().split("test(2,'st'")[0]
src=src.replace("rows.append((a,int(J[n][2]),int(J[n][3]),int(J[n][4]),int(J[n][5]),cur))","rows.append((a,int(J[n][2]),int(J[n][3]),int(J[n][4]),int(J[n][5]),cur,int(J[n][7]),n))")
src=src.replace("D={m:load(*m) for m in MODS}","D={m:load(*m) for m in MODS}")
PROG='all'
exec(src)
names=['entries','labels','symbols','namebytes','codelc','alljpr','databytes','lines','funcs']
DATA=[]
import re as _re
EX={}
for m in MODS:
    L=open(T%m[0]+m[1]+'_old.s',errors='replace').read().split('\n')
    db=0;ln=0;out=[];funcs=0
    for l in L:
        x=l.split('/*')[0].split('//')[0].split()
        ln+=1
        if x:
            if x[0]=='.byte': db+=len(' '.join(x[1:]).split(','))
            elif x[0]=='.word' or x[0]=='.addr': db+=2*len(' '.join(x[1:]).split(','))
            elif x[0]=='.long': db+=4
            elif x[0].startswith('.=.+'): db+=int(x[0][4:])
            if x[0]=='{': funcs+=1
            if x[0] in('jpr','callr'): out.append((db,ln,funcs))
    EX[m[1]]=out
for m in MODS:
    S,LL,rows=D[m]
    pts=[]
    for r in rows:
        if r[0] is None: continue
        it,st,nt,e,lc,n=r[1],r[2],r[3],r[4],r[6],r[7]
        ex=EX[m[1]][r[7]]
        pts.append((r[0],(e,(it-2-14*e)/6.0,st/10.0,nt,lc,n)+ex))
    DATA.append((m[1],pts,S,LL))
total=sum(len(d[3]) for d in DATA); shorts=sum(len(d[2]) for d in DATA)
print('modules',len(DATA),'long sites',total,'short sites',shorts,'sites',sum(len(d[1]) for d in DATA),flush=True)
def score(w,T,reset=True,detail=False):
    miss=false=0; out={}
    for name,pts,S,LL in DATA:
        trig=[]; base=0.0
        for a,f in pts:
            x=sum(wi*fi for wi,fi in zip(w,f) if wi)
            if x-base>=T:
                trig.append(a)
                base=x if reset else base+T
        m=0;fl=0
        for a,t in LL:
            i=bisect.bisect_right(trig,a)
            if not(i<len(trig) and trig[i]<t): m+=1
        for a,t in S:
            i=bisect.bisect_right(trig,a)
            if i<len(trig) and trig[i]<t: fl+=1
        miss+=m; false+=fl
        if detail: out[name]=(m,fl,[hex(x) for x in trig])
    return (miss+false,miss,false,out) if detail else (miss+false,miss,false)
best=[]
def scan(w,label):
    mx=max(sum(wi*fi for wi,fi in zip(w,d[1][-1][1])) for d in DATA if d[1])
    Ts=[mx*(0.04+0.96*i/99) for i in range(100)]
    b=None
    for T in Ts:
        s=score(w,T)
        if b is None or s<b[0]: b=(s,T)
    best.append((b[0][0],b[0][1],b[0][2],b[1],label,w)); 
print('no trigger: miss %d false 0'%total,flush=True)
for i in range(9):
    w=[0]*9; w[i]=1; scan(w,names[i])
best.sort(key=lambda x:x[0])
print('single:',[(b[4],b[0],b[1],b[2],round(b[3])) for b in best[:6]],flush=True)
R=[0.05,0.1,0.2,0.33,0.5,0.71,1,1.4,2,3,5,7,10,14]
for i,j in itertools.combinations(range(9),2):
    for r in R:
        w=[0]*9; w[i]=1; w[j]=r; scan(w,'%s+%g*%s'%(names[i],r,names[j]))
best.sort(key=lambda x:x[0])
print('pairs best:')
for b in best[:10]: print(' total %d miss %d false %d T %.0f %s'%(b[0],b[1],b[2],b[3],b[4]),flush=True)
b=best[0]; print(score(b[5],b[3],detail=True)[3])
