#!/usr/bin/env python3
"""castrig/memsum.py: as reset.py, for sums in bytes of the cas tables."""
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

# features: 0 entries,1 labels,2 st/10,3 nt,4 codelc,5 alljpr,6 databytes,7 lines,8 funcs
V={
 'st+nt+14all':[0,0,10,1,0,14,0,0,0],
 'st+nt+14all+6lab':[0,6,10,1,0,14,0,0,0],
 'st+nt+14e':[14,0,10,1,0,0,0,0,0],
 'st+nt+14e+6lab(all tables)':[14,6,10,1,0,0,0,0,0],
 'st+14all':[0,0,10,0,0,14,0,0,0],
 'st+14all+6lab':[0,6,10,0,0,14,0,0,0],
 'nt+14all':[0,0,0,1,0,14,0,0,0],
 'st+nt+10all':[0,0,10,1,0,10,0,0,0],
 'st+nt+12all':[0,0,10,1,0,12,0,0,0],
 'st+nt+16all':[0,0,10,1,0,16,0,0,0],
 'st+nt+6all':[0,0,10,1,0,6,0,0,0],
 'st+nt+18all':[0,0,10,1,0,18,0,0,0],
}
res=[]
for name,w in V.items():
    mx=max(sum(wi*fi for wi,fi in zip(w,d[1][-1][1]) if wi) for d in DATA if d[1])
    b=None
    for T in range(int(mx*0.1),int(mx),8):
        s_=score(w,T)
        if b is None or s_<b[0]: b=(s_,T)
    res.append((b[0],b[1],name,w)); print(name,b[0],'T',b[1],hex(b[1]),flush=True)
res.sort(key=lambda x:x[0])
b=res[0]; print(b[2],score(b[3],b[1],detail=True)[3])
# all near thresholds of the best
w=b[3]
print([(T,score(w,T)[0]) for T in range(b[1]-200,b[1]+200,16)])
