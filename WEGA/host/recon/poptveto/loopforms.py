import re,sys,os,subprocess
S='/tmp/SCRATCH/'
def variants(src):
    L=src.split('\n')
    i=[k for k,l in enumerate(L) if re.match(r'\tfor \(p = list;',l)][0]
    j=[k for k in range(i,len(L)) if L[k]=='\tif (type == 1) {'][0]-1
    assert L[j]=='\t}'
    head=L[:i]; body=L[i+1:j]; tail=L[j+1:]
    V={}
    V['goto_done']=head+['\tfor (p = list; ; p = p->n_next) {','\t\tif (p == 0)','\t\t\tgoto done;']+body+['\t}','done:']+tail
    V['break']=head+['\tfor (p = list; ; p = p->n_next) {','\t\tif (p == 0)','\t\t\tbreak;']+body+['\t}']+tail
    V['label_after']=head+[L[i]]+body+['\t}','done:']+tail
    V['goto_done_nz']=head+['\tfor (p = list; ; p = p->n_next) {','\t\tif (!p)','\t\t\tgoto done;']+body+['\t}','done:']+tail
    return {k:'\n'.join(v) for k,v in V.items()}
for f in sys.argv[1:]:
    for k,v in variants(open(S+f).read()).items():
        d=S+'lf/%s_%s'%(f[:-2],k); os.makedirs(d,exist_ok=True)
        for h in ('cas.h','pstdata.h','stdio.h'): open(d+'/'+h,'w').write(open(S+'raw/'+h).read())
        open(d+'/jpr.c','w').write(v)
        r=subprocess.run([S+'cc_q3','-S','jpr.c'],cwd=d,capture_output=True,text=True)
        if not os.path.exists(d+'/jpr.s'): print('FAIL',d,r.stderr[:200]); continue
        res=[]
        for b in ('w24','ec1','eri','erid'):
            o=subprocess.run(['python3','/home/olivleh1/git/P8000/WEGA/host/recon/poptveto/jfast.py',S+'o_%s/lib/vc/spopt'%b,d+'/jpr.s'],capture_output=True,text=True).stdout.split()
            res.append('%s:%s'%(b,o[0] if o else '?'))
        print('%-40s'%(f[:-2]+'_'+k),' '.join(res),flush=True)
