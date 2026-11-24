"""hplace.py: real builds of make (ts/make of setup.sh) by ccx with the headers of files.c (sys/types.h sys/stat.h
sys/dir.h pwd.h ar.h s.out.h nlist.h) each in defs or in the .c files that include it, s.out.h in four forms (full,
without s_head, without v_lines, without both); per combination the number of functions whose frame differs
(makecand.sh) and, for 3 or fewer, which"""
import itertools,subprocess,shutil,os,re
H=os.path.dirname(os.path.abspath(__file__)); WEGA=os.path.normpath(H+'/../../..')
W=os.environ.get('RS_WORK',os.path.normpath(WEGA+'/../tmp/tmp/regslots'))
I=os.environ.get('WEGA_ROOT',os.path.normpath(WEGA+'/../tmp/wega31_root'))+'/usr/include/s.out.h'
src=open(I).read()
SOUT={'full':src,
 'nohead':re.sub(r'/\* Complete s.out header.*?(?=/\* Line table)','',src,flags=re.S),
 'novl':src.split('/* Line table')[0],
 'both':src.split('/* Complete s.out')[0]}
def build(tag,types_d,stat_d,dir_d,pwd_d,ar_d,nl_d,sv):
    d=W+'/en_'+tag; shutil.rmtree(d,ignore_errors=True); shutil.copytree(W+'/ts/make',d)
    open(d+'/s.out.h','w').write(SOUT[sv])
    inc=[]
    if types_d: inc.append('#include <sys/types.h>')
    if stat_d: inc.append('#include <sys/stat.h>')
    if dir_d: inc.append('#include <sys/dir.h>')
    if pwd_d: inc.append('#include <pwd.h>')
    if ar_d: inc.append('#include <ar.h>')
    inc.append('#include "s.out.h"')
    if nl_d: inc.append('#include <nlist.h>')
    t=open(d+'/defs').read().replace('#include <stdio.h>\n','#include <stdio.h>\n'+'\n'.join(inc)+'\n',1)
    open(d+'/defs','w').write(t)
    drop={'sys/types.h':types_d,'sys/stat.h':stat_d,'sys/dir.h':dir_d,'pwd.h':pwd_d,'ar.h':ar_d,'s.out.h':True,'nlist.h':nl_d}
    for f in ('main.c','misc.c','files.c'):
        L=open(d+'/'+f).read().split('\n')
        L=[l for l in L if not any(re.match(r'#include\s*[<"]%s[>"]'%re.escape(h),l) and v for h,v in drop.items())]
        open(d+'/'+f,'w').write('\n'.join(L))
    r=subprocess.run([H+'/makecand.sh',d],capture_output=True,text=True).stdout
    shutil.rmtree(d)
    return r
for types_d,stat_d,dir_d,pwd_d,ar_d,nl_d in itertools.product([0,1],repeat=6):
    if (stat_d or dir_d) and not types_d: continue
    for sv in SOUT:
        tag='%d%d%d%d%d%d_%s'%(types_d,stat_d,dir_d,pwd_d,ar_d,nl_d,sv)
        r=build(tag,types_d,stat_d,dir_d,pwd_d,ar_d,nl_d,sv)
        w=int(r.split('WRONG')[-1])
        print(tag,w,' '.join(l.split(':')[0]+':'+l.split(':')[1].strip() for l in r.split('\n') if ':' in l)[:150] if w<=3 else '',flush=True)
