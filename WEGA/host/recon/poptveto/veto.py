import os,re,subprocess,sys,collections
S='/tmp/claude-1000/-mnt-share-cvs-P8000/6b8d4c9d-ace2-40e9-b4e5-33a880fd019b/scratchpad'
W='/home/olivleh1/git/P8000/WEGA'; E=W+'/host/z8k-emu/z8k-emu'
R=S+'/o_veto'
cc=S+'/cc_veto'
open(cc,'w').write('#!/bin/sh\nexec %s -r %s /bin/sccvc "$@"\n'%(E,R)); os.chmod(cc,0o755)
env=dict(os.environ, RECON_PROG='cas', RECON_TMP=S+'/vt', CCHOST=cc, CAS=S+'/cas_old', D12_SRC=S+'/vsrc')
def run(module, func, fidx, V):
    open(R+'/tmp/veto.txt','w').write(' '.join(str(x) for x in [fidx]+sorted(V))+'\n')
    try: os.remove(R+'/tmp/veto.log')
    except OSError: pass
    subprocess.run(['python3',W+'/host/recon/d12model.py','-q'],env=dict(env,D12_ONLY=module),capture_output=True,text=True)
    out=subprocess.run(['python3',W+'/host/recon/strictcmp.py'],env=env,capture_output=True,text=True).stdout
    m=re.search(r'^%s\s+\w+\s+\d+\s+\d+\s+\w+\s+(SAME|DIFF (\d+))'%func,out,re.M)
    lines = None if not m else (0 if m.group(1)=='SAME' else int(m.group(2)))
    ev=collections.defaultdict(list)
    try:
        for l in open(R+'/tmp/veto.log'):
            f,k,t,lab=l.split(); ev[int(f)].append((int(k),t,int(lab)))
    except OSError: pass
    return lines, ev
def search2(module, func, fidx, depth=2):
    base,ev0=run(module,func,fidx,[])
    best=(base,[]); print('base',base,len(ev0[fidx]),flush=True)
    seen={}
    def rec(V,ev,d):
        nonlocal best
        start = (max(V)+1) if V else 0
        for k,t,lab in ev[fidx]:
            if k<start: continue
            l,e2=run(module,func,fidx,V+[k])
            if l is None: continue
            if l<best[0]:
                best=(l,V+[k]); print('  better',best,(t,lab),flush=True)
                if l==0: return True
            if d>1 and rec(V+[k],e2,d-1): return True
        return False
    rec([],ev0,depth)
    print('RESULT',func,best,flush=True)
    return best
if __name__=='__main__':
    module,func=sys.argv[1],sys.argv[2]
    lines,ev=run(module,func,-5,[])
    print('base',lines,{f:len(v) for f,v in ev.items()})
    if len(sys.argv)>3:
        fidx=int(sys.argv[3])
        V=[int(x) for x in sys.argv[4:]]
        best,ev=run(module,func,fidx,V)
        print('start',best,'events',len(ev[fidx]))
        k=0
        while True:
            n=len(ev[fidx])
            if k>=n: break
            if k in V: k+=1; continue
            l,e2=run(module,func,fidx,V+[k])
            typ=[x for x in ev[fidx] if x[0]==k]
            print(k,typ,l,flush=True)
            if l is not None and l<best:
                best=l; V=V+[k]; ev=e2; print('  keep',V,best,flush=True)
                if best==0: break
            k+=1
        print('RESULT',V,best)
