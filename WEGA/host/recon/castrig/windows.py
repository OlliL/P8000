#!/usr/bin/env python3
"""castrig/windows.py PROG...: from the original binary alone, the points in each module where the old cas must have run its early
jump optimization: every long jump or call in reach spans such a point, no short one does (OLDCC.md, cparse point 4)."""
import sys,re,json
sys.path.insert(0,'/mnt/share/cvs/P8000/WEGA/host/tools'); sys.path.insert(0,'/mnt/share/cvs/P8000/WEGA/host/recon')
from sout import SOut
import zcode
W='/mnt/share/cvs/P8000/WEGA/src/cmd/'; R='/mnt/share/cvs/P8000/tmp/wega31_root/lib/'
def load(prog):
    F=json.load(open(W+prog+'/tools/findings.json'))['functions']
    fn=sorted((int(f['orig'],16),f['size_orig'],k,f.get('file')) for k,f in F.items() if isinstance(f.get('orig'),str) and f.get('size_orig') and f.get('status')!='crt0')
    g=SOut(open(R+prog,'rb').read()); gc=bytes(g.image)[:g.segs[0]['code']]
    mods=[]
    for lo,s,k,fl in fn:
        if not mods or mods[-1][0]!=fl: mods.append([fl,lo,lo+s,[k]])
        else: mods[-1][2]=lo+s; mods[-1][3].append(k)
    return fn,gc,mods
def ana(prog):
    fn,gc,mods=load(prog)
    print('==',prog,[(m[0],hex(m[1]),hex(m[2])) for m in mods])
    for fl,lo,hi,ks in mods:
        ins=list(zcode.insns(gc,lo,hi,False))
        L=[];S=[];BL=[];BS=[]
        for a,x in ins:
            m=re.match(r'(call|calr)\t%([0-9A-F]+)$',x)
            n=re.match(r'(jp|jr)\t(?:[a-z]+,)?%([0-9A-F]+)$',x)
            if m: f,t=m[1],int(m[2],16); lng=f=='call'; rs=2; rg=(-4092,4098)
            elif n: f,t=n[1],int(n[2],16); lng=f=='jp'; rs=2; rg=(-254,256)
            else: continue
            if not lo<=t<hi: continue
            d=t-a
            if not rg[0]<=d<=rg[1]: continue
            if t>a: (L if lng else S).append((a,t,f))
            else: (BL if lng else BS).append((a,t,f))
        # triggers: greedy stabbing of L intervals, conflicts with S
        L.sort(key=lambda x:x[1]); tr=[]; 
        for a,t,f in L:
            if tr and a<tr[-1]<t: continue
            # choose the latest point not inside a short interval, else t
            tr.append(t)
        # refine: for each trigger choose window: intersection of intervals it stabs
        print(fl,'forward long',len(L),'short',len(S),'back long',len(BL),'short',len(BS))
        print('  long fw:',' '.join('%x>%x%s'%(a,t,'c' if f=='call' else 'j') for a,t,f in L)[:700])
        print('  long bw:',' '.join('%x>%x%s'%(a,t,'c' if f=='call' else 'j') for a,t,f in BL)[:300])
        # windows
        W2=[]
        LL=sorted(L)
        cur=None
        for a,t,f in sorted(L,key=lambda x:x[0]):
            if cur and a<cur[1]:
                cur=[max(cur[0],a),min(cur[1],t)]
            else:
                if cur: W2.append(cur)
                cur=[a,t]
        if cur: W2.append(cur)
        # conflict count: shorts that span all of window
        out=[]
        for w in W2:
            c=[s for s in S if s[0]<=w[0] and s[1]>=w[1]]
            out.append('%x..%x%s'%(w[0],w[1],' CONFLICT%d'%len(c) if c else ''))
        print('  windows:',out)
for p in sys.argv[1:]: ana(p)
