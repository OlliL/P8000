"""condrun.py NAME=COND ... : spopt with PPY d41c.py (D41C=COND) built in parallel, full models, strictcmp lines."""
import os, subprocess, sys, re
from concurrent.futures import ThreadPoolExecutor
S='/tmp/SCRATCH'
W='/home/olivleh1/git/P8000/WEGA'; E=W+'/host/z8k-emu/z8k-emu'
DEF="-DOLD_D13 -DOLD_D45 -DOLD_D20S -DOLD_D49"
PPY=os.environ.get('CPPY', S+'/d41c.py')
def one(arg):
    name,cond=arg.split('=',1)
    env=dict(os.environ,D41C=cond,PPY=PPY)
    r=subprocess.run(['sh',S+'/tools/pb.sh',name,DEF],env=env,capture_output=True,text=True)
    if 'built' not in r.stdout: return name,cond,'NOBUILD '+r.stdout[-200:]
    cc=S+'/cc_'+name
    open(cc,'w').write('#!/bin/sh\nexec timeout -s KILL 60 %s -r %s/o_%s /bin/sccvc "$@"\n'%(E,S,name)); os.chmod(cc,0o755)
    env=dict(os.environ,RECON_PROG='cas',RECON_TMP=S+'/t_'+name,CCHOST=cc,CAS=S+'/cas_old')
    subprocess.run(['python3',W+'/host/recon/d12model.py','-q'],env=env,capture_output=True,cwd=W)
    out=subprocess.run(['python3',W+'/host/recon/strictcmp.py'],env=env,capture_output=True,text=True,cwd=W).stdout
    d=[(m.group(1),int(m.group(2))) for m in re.finditer(r'^(\w+)\s+\w+\s+\d+\s+\d+\s+\w+\s+DIFF (\d+)',out,re.M)]
    return name,cond,' '.join('%s:%d'%x for x in d)
with ThreadPoolExecutor(8) as ex:
    for name,cond,res in ex.map(one, sys.argv[1:]):
        print('%-6s %-40s %s'%(name,cond,res[:220]),flush=True)
