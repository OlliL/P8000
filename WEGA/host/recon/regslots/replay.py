"""replay.py: the events of a cparse log (#M #F #X N/R/F #D) through the V7 malloc simulator with a model of the byte at
entry+12 (sfldc): predicts the stale sfldc of every new entry and the frame words of the register locals per function.
hook(ev, i, emit) of a hypothesis may change the stream."""
import os
HERE=os.path.dirname(os.path.abspath(__file__))
WEGA=os.path.normpath(os.path.join(HERE,'..','..','..'))
WORK=os.environ.get('RS_WORK', os.path.normpath(os.path.join(WEGA,'..','tmp','tmp','regslots')))
import sys
sys.path.insert(0,HERE)
from v7sim import Heap
SF=12
def parse(path):
    ev=[]
    for l in open(path,errors='replace'):
        w=l.split()
        if not w: continue
        if w[0]=='#M': ev.append(('M',int(w[1]),int(w[2],16)))
        elif w[0]=='#F': ev.append(('f',int(w[1],16)))
        elif w[0]=='#X' and len(w)>=7: ev.append(('X'+w[1],int(w[2],16),int(w[3]),int(w[4]),int(w[5]),w[6]))
        elif w[0]=='#D': ev.append(('D',w[1]))
    return ev
class Heap2(Heap):
    def __init__(s,brk):
        Heap.__init__(s,brk); s.mem={}
        orig=s.m
        class W(dict):
            def __setitem__(d,k,v):
                dict.__setitem__(d,k,v); s.mem[k]='h'; s.mem[k+1]='h'
        s.m=W()
def run(ev, hook=None, clearcls=(), symsz=26, sf=12):
    SF=sf
    """returns (per function: [predicted stale of each register local]), checks"""
    first=[e for e in ev if e[0]=='M'][0][2]
    h=Heap2(first-2)
    amap={}      # logged address -> simulated address
    sym={}       # simulated address -> [stale, is_param]
    out=[]; fn=None; chk=[0,0]
    pend=None
    def emit(e):
        nonlocal fn,pend
        k=e[0]
        if k=='M':
            n=e[1]
            r=h.malloc(symsz if n==26 else n); amap[e[2]]=r
            if n==26: pend=r   # a symbol entry: its fields come with the N event
            else:
                for b in range(r,r+n): h.mem[b]='s'
        elif k=='f':
            r=amap.get(e[1]); h.free(r)
        elif k=='XN':
            r=amap.get(e[1]); st=h.mem.get(r+SF,0)
            pred=st if st in (1,2) else 0
            act=e[2] if e[2] in (1,2) else 0
            chk[0]+=1; chk[1]+= pred==act
            for b in range(r,r+symsz):
                if b!=r+SF: h.mem[b]='o'
            sym[r]=pred
        elif k=='XR':
            r=amap.get(e[1])
            if e[4]>=2 and e[3]==4: out.append((fn,e[5],sym.get(r,0)))
        elif k=='XF':
            r=amap.get(e[1])
            # a parameter (level 1) has sfldc from getpreg: the logged value; else what it got at its creation
            v=e[2] if e[4]==1 else (0 if e[3] in clearcls else sym.get(r,0))
            h.mem[r+SF]=v
        elif k=='D': fn=e[1]
    for i,e in enumerate(ev):
        if hook: hook(e,i,emit)
        else: emit(e)
    return out,chk
if __name__=='__main__':
    out,chk=run(parse(sys.argv[1]))
    print('stale predicted right: %d of %d'%(chk[1],chk[0]))
    import collections
    c=collections.OrderedDict()
    for f,n,s in out: c.setdefault(f,[]).append('%s%s'%(n,'*' if s else ''))
    print(' '.join('%s(%s)'%(f,','.join(v)) for f,v in c.items() if any('*' in x for x in v)))
