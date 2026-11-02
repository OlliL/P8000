#!/usr/bin/env python3
"""castrig/offsets.py: the table offsets of cas (itable, symbol table, name table, entries) at every jpr and callr of the modules of popt and
cparse, and tests of thresholds. HC is a scratch copy of host/cas with this line at the top of jpr() in jpr.c, itoff and ntoff not static in ssymtab.c:
  fprintf(stderr, "J %d %u %u %u %d %d %u\\n", tok, itoff, stoff, ntoff, lcaux[cursec][cursub].la_0, cursec, (unsigned)lctab[cursec][cursub]);
Needs the _old.s modules of oldcc.sh model."""
import os,sys,re,json
sys.path.insert(0,'/mnt/share/cvs/P8000/WEGA/host/tools'); sys.path.insert(0,'/mnt/share/cvs/P8000/WEGA/host/recon')
from sout import SOut
import zcode
W='/mnt/share/cvs/P8000/WEGA/src/cmd/'; R='/mnt/share/cvs/P8000/tmp/wega31_root/lib/'; T='/mnt/share/cvs/P8000/tmp/tmp/d12model_%s/'
REG=re.compile(r'^(r[lh]?\d+|rr\d+|rq\d+|f\d+|fp|sp|eq|ne|lt|le|gt|ge|c|nc|ugt|uge|ult|ule|z|nz|mi|pl|ov|nov)$')
def jumps(gc,lo,hi):
    r=[]
    for a,x in zcode.insns(gc,lo,hi,False):
        m=re.match(r'(call|calr|jp|jr)\t(?:[a-z]+,)?%([0-9A-F]+)$',x)
        if m: r.append((a,m[1],int(m[2],16)))
    return r
exec(open('sim.py').read().split("run('popt'")[0])
HC=os.environ['HC']
def load(prog,mod,fl):
    F=json.load(open(W+prog+'/tools/findings.json'))['functions']
    g=SOut(open(R+prog,'rb').read()); gc=bytes(g.image)[:g.segs[0]['code']]
    allf=sorted((int(f['orig'],16),f['size_orig'],k,f.get('file')) for k,f in F.items() if isinstance(f.get('orig'),str) and f.get('size_orig') and f.get('status')!='crt0')
    rng=[(l,l+s) for l,s,k,f2 in allf if f2==fl]; lo=rng[0][0]; hi=rng[-1][1]
    S=[];LL=[]
    for a,f,t in jumps(gc,lo,hi):
        if not(lo<=t<hi and t>a): continue
        rg=4098 if f in('call','calr') else 256
        if t-a>rg: continue
        (LL if f in('call','jp') else S).append((a,t))
    bases={k:(l,s) for l,s,k,f2 in allf}
    fn=T%prog+mod+'_old.s'
    r=subprocess.run([HC,'-u','-o','/dev/null',fn],capture_output=True,text=True)
    J=[l.split() for l in r.stderr.split('\n') if l.startswith('J ')]
    L=open(fn,errors='replace').read().split('\n')
    cur=None;seq=[];idx=0;n=0;rows=[]
    for l in L:
        s=l.split('/*')[0].split('//')[0]
        m=re.match(r'\s*([_~\w]+)::',s)
        if m and m[1][1:] in bases:
            cur=m[1][1:]; seq=jumps(gc,bases[cur][0],bases[cur][0]+bases[cur][1]); idx=0
        t=s.split()
        if t and t[0] in('jpr','callr'):
            a=seq[idx][0] if cur and idx<len(seq) else None; idx+=1
            if n<len(J): rows.append((a,int(J[n][2]),int(J[n][3]),int(J[n][4]),int(J[n][5]),cur))
            n+=1
    return S,LL,rows
MODS=[('popt','z21','z21.c'),('popt','z22','z22.c'),('popt','z23','z23.c'),('cparse','cgram1','cgram1.y'),('cparse','scan1','scan1.c'),('cparse','pftn1','pftn1.c'),('cparse','trees1','trees1.c'),('cparse','optim1','optim1.c'),('cparse','code1','code1.c'),('cparse','local1','local1.c'),('cparse','comm1','comm1.c'),('cparse','vinit1','vinit1.c'),('cparse','sym1','sym1.c')]
D={m:load(*m) for m in MODS}
def test(col,name,vals):
    print('==',name)
    for m in MODS:
        S,LL,rows=D[m]
        o=[]
        for v in vals:
            c=[r for r in rows if r[0] and r[col]>=v]
            if not c: continue
            p=c[0][0]
            prev=[r for r in rows if r[0] and r[0]<p]
            pp=prev[-1][0] if prev else p
            # the trigger lies between the previous site and p: s = shorts spanning the whole gap
            sp=sum(1 for a,t in S if a<=pp and t>=p+1 and a<p<t); lp=sum(1 for a,t in LL if a<p<t)
            o.append('%x@%x s%d l%d'%(v,p,sp,lp))
        print(m[1],' '.join(o))
test(2,'st',[0x800,0xc00,0x1000])
test(3,'nt',[0x800,0x900,0x924,0x930,0x940])
test(4,'e',[255])
