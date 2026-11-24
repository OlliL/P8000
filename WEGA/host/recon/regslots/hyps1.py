import os
HERE=os.path.dirname(os.path.abspath(__file__))
WEGA=os.path.normpath(os.path.join(HERE,'..','..','..'))
WORK=os.environ.get('RS_WORK', os.path.normpath(os.path.join(WEGA,'..','tmp','tmp','regslots')))
import sys; sys.path.insert(0,HERE)
from hyp import score
def runs(ev):
    """split into items; a run of ('XF', 'f') pairs is one item (a clearst batch) as a list of pairs"""
    out=[]; i=0
    while i<len(ev):
        if ev[i][0]=='XF' and i+1<len(ev) and ev[i+1][0]=='f':
            b=[]
            while i+1<len(ev) and ev[i][0]=='XF' and ev[i+1][0]=='f':
                b.append((ev[i],ev[i+1])); i+=2
            out.append(b)
        else: out.append(ev[i]); i+=1
    return out
def flat(items):
    r=[]
    for it in items:
        if isinstance(it,list):
            for a,b in it: r+= [a,b]
        else: r.append(it)
    return r
def order(key):
    def t(ev):
        crt={}
        for i,e in enumerate(ev):
            if e[0]=='XN': crt.setdefault(e[1],i)
        return flat([sorted(it,key=lambda p:key(p,crt)) if isinstance(it,list) else it for it in runs(ev)])
    return t
H={}
H['rev']=lambda ev: flat([list(reversed(it)) if isinstance(it,list) else it for it in runs(ev)])
H['addr']=order(lambda p,c:p[0][1]); H['addrdesc']=order(lambda p,c:-p[0][1])
H['create']=order(lambda p,c:c.get(p[0][1],0)); H['createdesc']=order(lambda p,c:-c.get(p[0][1],0))
H['level']=order(lambda p,c:(-p[0][4],c.get(p[0][1],0)))   # inner levels first, by creation
H['level2']=order(lambda p,c:(p[0][4],c.get(p[0][1],0)))
def strtx(mode):
    def t(ev):
        sa=set(e[2] for e in ev if e[0]=='M' and e[1]!=26)
        r=[]
        for e in ev:
            if e[0]=='M' and e[1]!=26:
                if mode=='none': continue
                if mode=='plus1': e=('M',e[1]+1,e[2])
                if mode=='plus2': e=('M',e[1]+2,e[2])
            if e[0]=='f' and e[1] in sa and mode in('nofree','none'): continue
            r.append(e)
        return r
    return t
for m in ('nofree','none','plus1','plus2'): H['str_'+m]=strtx(m)
def extra(where,n,free):
    def t(ev):
        r=[]; k=0
        for e in ev:
            if e[0]==where or (where=='clr' and e[0]=='XF' and r and r[-1][0] not in('XF','f')):
                a=0x900000+k; k+=1
                r.append(('M',n,a))
                if free: r.append(('f',a))
            r.append(e)
        return r
    return t
for w in ('D','clr'):
    for n in (26,8,2):
        for fr in (0,1): H['x%s%d%s'%(w,n,'f' if fr else '')]=extra(w,n,fr)
def param1(ev):   # every parameter leaves 1 (not 2)
    return [(e[0],e[1],1 if e[2]==2 else e[2])+e[3:] if e[0]=='XF' and e[4]==1 else e for e in ev]
H['param1']=param1
for k,f in H.items():
    b,l=score(f); print('%-12s %d'%(k,b))
