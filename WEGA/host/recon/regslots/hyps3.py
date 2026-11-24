"""hyps3.py: names on the heap (classic pcc): an extra malloc of strlen(name)+1 per new entry; variants: globals only or
all names, in front of or behind the entry, kept or freed with the entry"""
import sys,os; sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import hyp
def names(which='glob', where='before', free=False, extra=1):
    def t(ev):
        # which entries are freed later (locals) and when
        freed={}
        for i,e in enumerate(ev):
            if e[0]=='XN': freed[i]=None
        last={}
        for i,e in enumerate(ev):
            if e[0]=='XN': last[e[1]]=i
            elif e[0]=='XF' and e[1] in last: freed[last.pop(e[1])]=i
        out=[]; tag=0xA00000; pend={}
        for i,e in enumerate(ev):
            if e[0]=='XN':
                isglob = freed.get(i) is None
                if which=='all' or isglob:
                    blk=('M',len(e[5])+extra,tag); tag+=2
                    if where=='before':
                        # in front of the malloc of the entry (the M just before this N)
                        out.insert(len(out)-1,blk)
                    else:
                        out.append(e); out.append(blk); e=None
                    if free and not isglob: pend[freed[i]]=blk[2]
            if e is not None: out.append(e)
            if i in pend and e is not None and e[0]=='XF':
                pass
            if e is not None and e[0]=='f' and out and len(out)>=2 and out[-2][0]=='XF':
                j=None
            # free the name behind the free of the entry
            if e is not None and e[0]=='f':
                k=i-1
                if k in pend: out.append(('f',pend.pop(k)))
        return out
    return t
for which in ('glob','all'):
    for where in ('before','after'):
        for fr in (False,True):
            if which=='glob' and fr: continue
            for ex in (1,0,2):
                b,l=hyp.score(names(which,where,fr,ex)); print(which,where,'free' if fr else 'keep','+%d'%ex,b)
