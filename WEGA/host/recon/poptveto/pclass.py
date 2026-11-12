"""pclass.py: veto by event classes. rules: list of (type, label, which) where which is 'all' or an int n (the n-th
occurrence, 1-based). Iterates: run, add the index of the first matching event not yet listed, run again."""
import sys; sys.path.insert(0,'/tmp/SCRATCH/tools'); import pveto
def runclass(rules, maxit=25):
    V=[]
    for it in range(maxit):
        Vt,l,ev,pp=pveto.run('jpr','jpr',0,V)
        cnt={}; add=None
        for k,t,lab in ev:
            cnt[(t,lab)]=cnt.get((t,lab),0)+1
            for rt,rl,w in rules:
                if t==rt and lab==rl and (w=='all' or w==cnt[(t,lab)]) and k not in V:
                    add=k; break
            if add is not None: break
        if add is None: return l,pp,V,ev
        V=sorted(V+[add])
    return l,pp,V,ev
