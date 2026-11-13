"""par.py: parallel tries. run(tasks) with tasks = [(name, module, {file: text}, popt)], returns {name: (lines of FUNCS..., others)}"""
import os, re, shutil, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
S='/tmp/SCRATCH'
W='/home/olivleh1/git/P8000/WEGA'
E=W+'/host/z8k-emu/z8k-emu'
N=8
BASE=None
def setup():
    for i in range(N):
        d='%s/w%d'%(S,i)
        shutil.rmtree(d,ignore_errors=True); os.makedirs(d)
        shutil.copytree(S+'/'+os.environ.get('PBASE','t')+'/d12model_cas', d+'/d12model_cas')
        shutil.copytree(W+'/src/cmd/cas', d+'/src', ignore=shutil.ignore_patterns('build','old','*.o'))
        for b in ('d45','A','B','cur'):
            root = '/home/olivleh1/git/P8000/tmp/oldroot' if b=='cur' else '%s/o_%s'%(S,b)
            open('%s/cc_%s'%(d,b),'w').write('#!/bin/sh\nexec %s -r %s /bin/sccvc "$@"\n'%(E,root)); os.chmod('%s/cc_%s'%(d,b),0o755)
import queue
free=queue.Queue()
for i in range(N): free.put(i)
def one(task):
    name, module, files, popt, funcs = task
    i=free.get()
    try:
        d='%s/w%d'%(S,i)
        saved={}
        for f,t in files.items():
            saved[f]=open(d+'/src/'+f).read(); open(d+'/src/'+f,'w').write(t)
        cc='%s/cc_%s'%(d,popt)
        if not os.path.exists(cc):
            root = '/home/olivleh1/git/P8000/tmp/oldroot' if popt=='cur' else '%s/o_%s'%(S,popt)
            open(cc,'w').write('#!/bin/sh\nexec %s -r %s /bin/sccvc "$@"\n'%(E,root)); os.chmod(cc,0o755)
        env=dict(os.environ, RECON_PROG='cas', RECON_TMP=d, CCHOST=cc, CAS=S+'/cas_old', D12_SRC=d+'/src')
        if module: env['D12_ONLY']=module
        subprocess.run(['python3',W+'/host/recon/d12model.py','-q'],env=env,capture_output=True,text=True)
        out=subprocess.run(['python3',W+'/host/recon/strictcmp.py'],env=env,capture_output=True,text=True).stdout
        for f,t in saved.items(): open(d+'/src/'+f,'w').write(t)
        for fn in os.environ.get('SHOW','').split():
            v=subprocess.run(['python3',W+'/host/recon/strictcmp.py','-v',fn],env=dict(env,STRICT_CTX=os.environ.get('CTX','3')),capture_output=True,text=True).stdout
            v='\n'.join(l for l in v.split('\n') if not re.search(r'(SAME|DIFF \d+)$',l) and not l.startswith('same'))
            open('%s/show_%s_%s.txt'%(S,name,fn),'w').write(v)
        res={}
        for l in out.split('\n'):
            m=re.match(r'(\w+)\s+\w+\s+(\d+)\s+(\d+)\s+\w+\s+DIFF (\d+)',l)
            if m: res[m.group(1)]=int(m.group(4))
        if 'same' not in out: return name, None, None
        return name, tuple(res.get(f,0) for f in funcs), sorted('%s:%d'%(k,res[k]) for k in res if k not in BASEOPEN and k not in funcs)
    finally:
        free.put(i)
BASEOPEN={'yylex','main','jpr','exprreloc'}
def run(tasks):
    with ThreadPoolExecutor(N) as ex:
        for r in ex.map(one,tasks):
            yield r
if __name__=='__main__':
    setup()
