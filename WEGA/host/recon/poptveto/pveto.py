"""pveto.py: parallel veto search. 8 slots, each with its own root copy, model copy and source copy.
setup(module_src_overrides) ; bfs(module, func, fidx, depth)"""
import os, re, shutil, subprocess, collections, queue, sys, itertools
from concurrent.futures import ThreadPoolExecutor
S='/tmp/SCRATCH'
W='/mnt/share/cvs/P8000/WEGA'; E=W+'/host/z8k-emu/z8k-emu'
N=8
free=queue.Queue()
def setup(files=None, base='t', root='o_veto'):
    for i in range(N):
        d='%s/pv%d'%(S,i); shutil.rmtree(d,ignore_errors=True); os.makedirs(d)
        subprocess.run(['cp','-a',S+'/'+root,d+'/root'],check=True)
        os.remove(d+'/root/tmp'); os.makedirs(d+'/root/tmp')
        shutil.copytree(S+'/'+base+'/d12model_cas', d+'/d12model_cas')
        shutil.copytree(W+'/src/cmd/cas', d+'/src', ignore=shutil.ignore_patterns('build','old','*.o'))
        for f,t in (files or {}).items(): open(d+'/src/'+f,'w').write(t)
        open(d+'/cc','w').write('#!/bin/sh\nexec %s -r %s/root /bin/sccvc "$@"\n'%(E,d)); os.chmod(d+'/cc',0o755)
    for i in range(N): free.put(i)
def run(module, func, fidx, V):
    i=free.get()
    try:
        d='%s/pv%d'%(S,i)
        open(d+'/root/tmp/veto.txt','w').write(' '.join(str(x) for x in [fidx]+sorted(V))+'\n')
        try: os.remove(d+'/root/tmp/veto.log')
        except OSError: pass
        env=dict(os.environ, RECON_PROG='cas', RECON_TMP=d, CCHOST=d+'/cc', CAS=S+'/cas_old', D12_SRC=d+'/src', D12_ONLY=module)
        r=subprocess.run(['python3',W+'/host/recon/d12model.py','-q'],env=env,capture_output=True,text=True); open(d+'/d12.log','w').write(r.stdout+r.stderr)
        out=subprocess.run(['python3',W+'/host/recon/strictcmp.py'],env=env,capture_output=True,text=True).stdout
        m=re.search(r'^%s\s+\w+\s+\d+\s+\d+\s+\w+\s+(SAME|DIFF (\d+))'%func,out,re.M)
        lines=None if not m else (0 if m.group(1)=='SAME' else int(m.group(2)))
        ev=[]
        try:
            for l in open(d+'/root/tmp/veto.log'):
                x=l.split()
                if len(x)<4: continue
                f,k,t,lab=x[:4]
                try:
                    if int(f)==fidx: ev.append((int(k),t,int(lab)))
                except ValueError: pass
        except OSError: pass
        return tuple(V), lines, ev
    finally:
        free.put(i)
def bfs(module, func, fidx, depth, keep=None):
    seen={}
    front=[()]
    with ThreadPoolExecutor(N) as ex:
        for d in range(depth+1):
            res=list(ex.map(lambda V: run(module,func,fidx,list(V)), front))
            for V,l,ev in res: seen[V]=(l,ev)
            best=sorted((l if l is not None else 9999,V) for V,l,ev in res)
            print('depth',d,'runs',len(res),'best',best[:5],flush=True)
            if best and best[0][0]==0: return best[0]
            nxt=set()
            for V,l,ev in res:
                if keep and l is not None and l>keep: continue
                for k,t,lab in ev:
                    if (not V or k>max(V)) and k not in V: nxt.add(tuple(sorted(V+(k,))))
            front=sorted(nxt)
    return None
