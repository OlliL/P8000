import sys,os,re,subprocess
S='/tmp/SCRATCH'; W='/mnt/share/cvs/P8000/WEGA'
d=S+'/pq0'; V=sys.argv[1:]
open(d+'/root/tmp/veto.txt','w').write(' '.join(['0']+V)+'\n')
for f in ('casm','jpr.o','jpr.s'):
    try: os.remove(d+'/d12model_cas/'+f)
    except OSError: pass
env=dict(os.environ,RECON_PROG='cas',RECON_TMP=d,CCHOST=d+'/cc',CAS=S+'/cas_old',D12_SRC=d+'/src',D12_ONLY='jpr')
subprocess.run(['python3',W+'/host/recon/d12model.py','-q'],env=env,capture_output=True)
out=subprocess.run(['python3',W+'/host/recon/strictcmp.py','-v','jpr'],env=dict(env,STRICT_CTX='3'),capture_output=True,text=True).stdout
print('\n'.join(l for l in out.split('\n') if not re.search(r'(SAME|DIFF \d+)$',l) and not l.startswith('same'))[:3000])
