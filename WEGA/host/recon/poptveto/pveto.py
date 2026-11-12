"""pveto.py: parallel veto search. 8 slots, each with its own root copy, model copy and source copy.
setup(module_src_overrides) ; bfs(module, func, fidx, depth)"""
import os, re, shutil, subprocess, collections, queue, sys, itertools
from concurrent.futures import ThreadPoolExecutor
S='/tmp/SCRATCH'
W='/home/olivleh1/git/P8000/WEGA'; E=W+'/host/z8k-emu/z8k-emu'
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
        open(d+'/cc','w').write('#!/bin/sh\nexec timeout -s KILL 20 %s -r %s/root /bin/sccvc "$@"\n'%(E,d)); os.chmod(d+'/cc',0o755)
    for i in range(N): free.put(i)
def run(module, func, fidx, V):
    i=free.get()
    try:
        d='%s/pv%d'%(S,i)
        open(d+'/root/tmp/veto.txt','w').write(' '.join(str(x) for x in [fidx]+sorted(V))+'\n')
        try: os.remove(d+'/root/tmp/veto.log')
        except OSError: pass
        for f in ('casm', module+'.o', module+'.s'):
            try: os.remove(d+'/d12model_cas/'+f)
            except OSError: pass
        env=dict(os.environ, RECON_PROG='cas', RECON_TMP=d, CCHOST=d+'/cc', CAS=S+'/cas_old', D12_SRC=d+'/src', D12_ONLY=module)
        r=subprocess.run(['python3',W+'/host/recon/d12model.py','-q'],env=env,capture_output=True,text=True); open(d+'/d12.log','w').write(r.stdout+r.stderr)
        out=subprocess.run(['python3',W+'/host/recon/strictcmp.py'],env=env,capture_output=True,text=True).stdout
        m=re.search(r'^%s\s+\w+\s+\d+\s+\d+\s+\w+\s+(SAME|DIFF (\d+))'%func,out,re.M)
        lines=None if not m else (0 if m.group(1)=='SAME' else int(m.group(2)))
        ev=[]
        try:
            for n_,l in enumerate(open(d+'/root/tmp/veto.log')):
                if n_ >= 300: break
                x=l.split()
                if len(x)<4: continue
                f,k,t,lab=x[:4]
                try:
                    if int(f)==fidx: ev.append((int(k),t,int(lab)))
                except ValueError: pass
        except OSError: pass
        return tuple(V), lines, ev, pat(d)
    finally:
        free.put(i)
def bfs(module, func, fidx, depth, keep=None, start=None, expand=None, anyk=False):
    seen={}
    front=start or [()]
    with ThreadPoolExecutor(N) as ex:
        for d in range(depth+1):
            res=list(ex.map(lambda V: run(module,func,fidx,list(V)), front))
            for V,l,ev,pp in res: seen[V]=(l,ev,pp)
            best=sorted((l if l is not None else 9999,V) for V,l,ev,pp in res)
            pb=sorted((l if l is not None else 9999,V) for V,l,ev,pp in res if pp)
            print('   plain-behind-A:',len(pb),pb[:6],flush=True)
            print('depth',d,'runs',len(res),'best',best[:5],flush=True)
            if best and best[0][0]==0: return best[0]
            nxt=set()
            for V,l,ev,pp in res:
                if keep and l is not None and l>keep: continue
                if l is None: continue
                if expand and not expand(l, pp, d): continue
                for k,t,lab in ev:
                    if (anyk or not V or k>max(V)) and k not in V and tuple(sorted(V+(k,))) not in seen: nxt.add(tuple(sorted(V+(k,))))
            front=sorted(nxt)
    return None

def pat(d):
    """plain behind A in jpr: first jpput after the tok==0x51 compare and before the cp 0x19 (25) of the second dispatch"""
    try: L=open(d+'/d12model_cas/jpr.s').read().split('\n')
    except OSError: return None
    try:
        a=next(i for i,l in enumerate(L) if '#81' in l and 'stkseg' in l)
    except StopIteration: return None
    j=[i for i,l in enumerate(L) if '_jpput' in l]
    c=[i for i,l in enumerate(L) if 'cp\tr2,#25' in l]
    if not j or not c: return None
    first=j[0]
    return a < first < c[0]
def runp(module, func, fidx, V):
    i=free.get(); free.put(i)
    r=run(module,func,fidx,V)
    return r
