import sys,threading; sys.path.insert(0,'/tmp/SCRATCH/tools'); import pveto
S='/tmp/SCRATCH'
pveto.setup({}, base='t', root=sys.argv[1])
jobs=[(f,r) for f in ('jpr_pl1.c','jpr_pl2.c') for r in ([],[('F',105,1)],[('F',105,2)],[('M',109,1),('F',105,1)])]
out=[]
def work(i,f,r):
    open('%s/pv%d/src/jpr.c'%(S,i),'w').write(open(S+'/'+f).read())
    V=[]
    for it in range(20):
        Vt,l,ev,pp=pveto.run('jpr','jpr',0,V,slot=i)
        cnt={}; add=None
        for k,t,lab in ev:
            cnt[(t,lab)]=cnt.get((t,lab),0)+1
            for rt,rl,w in r:
                if t==rt and lab==rl and (w=='all' or w==cnt[(t,lab)]) and k not in V: add=k; break
            if add is not None: break
        if add is None: break
        V=sorted(V+[add])
    out.append((l,pp,f,r,V))
th=[threading.Thread(target=work,args=(i,f,r)) for i,(f,r) in enumerate(jobs)]
for t in th: t.start()
for t in th: t.join()
for x in sorted(out,key=lambda x:(x[0] if x[0] is not None else 999)): print(x)
