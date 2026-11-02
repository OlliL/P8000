#!/usr/bin/env python3
"""castrig/fit.py: can a weighted sum of counters (entries, labels, symbols, name bytes, code location, all jpr and callr) reach one threshold at the first trigger point of each module? CAST=dir/d12model_%s/ CAS_J=1 HC=host cas with hostcas.patch."""
import os,sys
src=open('/mnt/share/cvs/P8000/WEGA/host/recon/castrig/offsets.py').read().split("test(2,'st'")[0]
src=src.replace("rows.append((a,int(J[n][2]),int(J[n][3]),int(J[n][4]),int(J[n][5]),cur))","rows.append((a,int(J[n][2]),int(J[n][3]),int(J[n][4]),int(J[n][5]),cur,int(J[n][7]),n))")
exec(src)
# windows of free sites, first trigger of each module
WIN={'z21':(0x61a,0x954),'z22':(0x1f5c,0x1fbc),'z23':(0x3486,0x4320),'scan1':(0x21a6,0x21a6),'pftn1':(0x3156,0x31ca),'trees1':(0x463a,0x463a),'sym1':(0x95a6,0x997c)}
def feat(r):
    it,st,nt,e,lc,n=r[1],r[2],r[3],r[4],r[6],r[7]
    labs=(it-2-14*e)/6.0
    return [e,labs,st/10.0,nt,lc,n]
names=['entries','labels','symbols(st/10)','namebytes','code lc','all jpr+callr']
A=[];rowsx={}
for m in MODS:
    if m[1] in WIN:
        S,LL,rows=D[m]; lo,hi=WIN[m[1]]
        a=[r for r in rows if r[0]==lo][0]; b=[r for r in rows if r[0]==hi][0]
        rowsx[m[1]]=(feat(a),feat(b))
        print(m[1],[round(x) for x in feat(a)],[round(x) for x in feat(b)])

import itertools
vals=[0]+[x/100 for x in range(1,100)]+[x/10 for x in range(10,100)]+[float(x) for x in range(10,200)]
mods=list(rowsx)
def rng(w):
    lo=max(sum(a*b for a,b in zip(w,rowsx[k][0])) for k in mods)
    hi=min(sum(a*b for a,b in zip(w,rowsx[k][1])) for k in mods)
    return lo,hi
res=[]
for i in range(6):
    w=[0]*6; w[i]=1; lo,hi=rng(w); res.append((hi-lo,names[i],'',0,lo,hi))
for i,j in itertools.permutations(range(6),2):
    for r in vals[1:]:
        w=[0]*6; w[i]=1; w[j]=r; lo,hi=rng(w)
        res.append(((hi-lo)/max(lo,1),names[i],names[j],r,lo,hi))
res.sort(key=lambda x:-x[0])
for x in res[:12]: print('%.3f %s + %s*%s window %.0f..%.0f'%(x[0],x[1],x[3],x[2],x[4],x[5]))
print('feasible pairs',sum(1 for x in res if x[0]>=0))
res3=[]
g=[0.05,0.1,0.2,0.25,0.33,0.5,0.67,0.75,1,1.5,2,3,4,5,6,8,10,12,14,16]
for i,j,k in itertools.combinations(range(6),3):
    for a in g:
        for b in g:
            w=[0]*6; w[i]=1; w[j]=a; w[k]=b; lo,hi=rng(w)
            if hi>=lo: res3.append(((hi-lo)/max(lo,1),names[i],names[j],a,names[k],b,lo,hi))
res3.sort(key=lambda x:-x[0]); print('triples feasible',len(res3))
for x in res3[:8]: print(x)
