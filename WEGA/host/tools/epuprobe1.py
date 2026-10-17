#!/usr/bin/env python3
"""EPU probe step 1 (see epugen.py): run in an empty work directory,
WEGA=<original tree> python3 epuprobe1.py"""
import os
import subprocess,re,sys,json,binascii,itertools
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from sout import SOut
E=os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','z8k-emu','z8k-emu'); R=os.environ['WEGA']
mn=['fabs','fadd','fcp','fcpx','fdiv','fint','fld','fldctl','fmul','fneg','fnorm','fsub','fsqr','fldm','fldil','fldiq','fcpf','fclr','fcpz','fldbcd','fldtl','fldtq','fexm','fremstep','fresflg','frestrap','fsetflg','fsetmode','fsettrap','fldctlb']
names=[m+s for m in mn for s in ('','s','d','x')]
pats=['f0','f0,f1','f0,r4','r4,f0','f0,@r4','@r4,f0','f0,#%4120','f0,#%41200000','r4','@r4','f0,r4(#%10)','f0,@r4,#2','@r4,f0,#2','f0,@r4,#4','r4,flags','flags,r4','f0,flags','flags,f0','#%1234','f0,%1000','%1000,f0','f0,x(r4)','f0,|%1000|','ff0','f0,ff0','ff0,f0','f0,f1,f2','f0,r4,#2','r4,f0,#2','#2,f0','f0,#2','f0,#%1234','r4,#%1234','@r4,#%1234','f0,r4,#4']
def assemble(lines):
    src="t module\ninternal\nx procedure\n entry\n"+"\n".join("\t"+l for l in lines)+"\n\tret\nend x\n"
    open('c.s','w').write(src)
    p=subprocess.run([E,'-r',R,'/bin/as','-f','-o','c.o','c.s'],capture_output=True,text=True)
    return p.stdout
valid={}
import os
for n in names:
    for p in pats:
        out=assemble([n+'\t'+p])
        if 'ERROR' not in out.replace('ERROR       5',''):
            valid.setdefault(n,[]).append(p)
json.dump(valid,open('valid.json','w'),indent=0)
print(sum(len(v) for v in valid.values()))
