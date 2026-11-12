import sys, itertools, threading, queue
sys.path.insert(0,'/tmp/SCRATCH/tools'); import pveto
S='/tmp/SCRATCH'
exec(open(S+'/jp1.py').read().split('tasks=[]')[0])
root=sys.argv[1]
pveto.setup({}, base='t', root=root)
while not pveto.free.empty(): pveto.free.get()
RULES=[[('M',109,1),('F',105,1)],[('F',105,1)],[]]
forms=[(s,o) for s in itertools.product([0,1],repeat=3) for o in itertools.permutations('abd')]
q=queue.Queue()
for fm in forms: q.put(fm)
out=[]
def worker(i):
    d='%s/pv%d'%(S,i)
    while True:
        try: s,o=q.get_nowait()
        except queue.Empty: return
        open(d+'/src/jpr.c','w').write(src[:i_]+mk(s,o)+src[j_:]) if False else open(d+'/src/jpr.c','w').write(SRC0[:I0]+mk(s,o)+SRC0[J0:])
        for r in RULES:
            V=[]
            for it in range(20):
                Vt,l,ev,pp=pveto.run("jpr","jpr",0,V,slot=i)
                cnt={}; add=None
                for k,t,lab in ev:
                    cnt[(t,lab)]=cnt.get((t,lab),0)+1
                    for rt,rl,w in r:
                        if t==rt and lab==rl and (w=='all' or w==cnt[(t,lab)]) and k not in V: add=k; break
                    if add is not None: break
                if add is None: break
                V=sorted(V+[add])
            out.append((l if l is not None else 999, pp, ''.join(map(str,s)), ''.join(o), str(r), V)); print(out[-1], flush=True)
SRC0=src; I0=i; J0=j
th=[threading.Thread(target=worker,args=(k,)) for k in range(8)]
for t in th: t.start()
for t in th: t.join()
print('BEST')
for x in sorted(out)[:12]: print(x)
