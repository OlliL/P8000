"""fscan.py OURS ORIG: every text function of OURS (symbols), masked, searched in ORIG; prints the found ones whose frame
(dec/sub r15) differs: name ours orig"""
import os
HERE=os.path.dirname(os.path.abspath(__file__))
WEGA=os.path.normpath(os.path.join(HERE,'..','..','..'))
WORK=os.environ.get('RS_WORK', os.path.normpath(os.path.join(WEGA,'..','tmp','tmp','regslots')))
import sys,re
W=WEGA
sys.path.insert(0,W+'/host/tools'); sys.path.insert(0,W+'/host/recon')
import zcode
from sout import SOut
def ld(p):
    o=SOut(open(p,'rb').read()); c=o.segs[0]['code']; return o,bytes(o.image)[:c],c
def norm(t):
    t=re.sub(r'^(dec|sub)\tr15,.*','FRAME-',t); t=re.sub(r'^(inc|add)\tr15,.*','FRAME+',t)
    return re.sub(r'%[0-9A-Fa-f]{1,8}\b|#\d+\b|_\w+','A',t.replace('@r15','%0000(r15)'))
on,gn,cn=ld(sys.argv[1]); oo,go,co=ld(sys.argv[2])
syms=sorted((s['value'],s['name']) for s in on.syms if s.get('name','').startswith('_')) if isinstance(on.syms,list) and on.syms and isinstance(on.syms[0],dict) else None
if syms is None:
    syms=[]
    for s in on.syms if isinstance(on.syms,list) else on.symbols:
        try: n,v=s['name'],s['value']
        except Exception: n,v=s[0],s[1]
        if n.startswith('_'): syms.append((v,n))
    syms.sort()
AN=list(zcode.insns(gn,0,cn,False)); AO=list(zcode.insns(go,0,co,False))
idxN={a:i for i,(a,_) in enumerate(AN)}
NO=[norm(t) for _,t in AO]; NN=[norm(t) for _,t in AN]
starts=[v for v,_ in syms]
def frame(t):
    m=re.match(r'^(dec\tr15,#(\d+)|sub\tr15,#%([0-9A-F]{4}))$',t)
    return (int(m.group(2)) if m.group(2) else int(m.group(3),16)) if m else None
found=nf=0
for k,(v,n) in enumerate(syms):
    if v not in idxN or v>=cn: continue
    i=idxN[v]; end=starts[k+1] if k+1<len(starts) else cn
    j=i
    while j<len(AN) and AN[j][0]<end: j+=1
    key=NN[i:j]
    if len(key)<3: continue
    fo=frame(AN[i][1])
    if fo is None: continue
    hit=None
    for x in range(len(NO)-len(key)+1):
        if NO[x]==key[0] and NO[x:x+len(key)]==key: hit=x; break
    if hit is None: nf+=1; continue
    found+=1
    fg=frame(AO[hit][1])
    if fg!=fo: print('%s %d %d %+d'%(n[1:],fo,fg,fg-fo))
print('# found %d, not found %d'%(found,nf))
