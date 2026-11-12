"""tailvar.py IN.s OUTDIR : variants of the function entry/exit layout of _jpr"""
import sys,os,re
src=open(sys.argv[1]).read(); out=sys.argv[2]; os.makedirs(out,exist_ok=True)
a=src.index('_jpr::'); b=src.index('_jprnew::')
f=src[a:b]
pro_re=re.compile(r'L10001:\n\tsub\tfp,#~L2\n\tldm\t_stkseg\+18\(fp\),r8,#6\n\tldm\t\|_stkseg\+~L1\+0\|\(fp\),r4,#4\n\tjpr\tL10002\n')
m=pro_re.search(f); assert m
pro=m.group(0); f0=f[:m.start()]+f[m.end():]
pro_body='\tsub\tfp,#~L2\n\tldm\t_stkseg+18(fp),r8,#6\n\tldm\t|_stkseg+~L1+0|(fp),r4,#4\n'
ex_i=f0.index('L104:\nL10000:\n')
V={}
# T1: prologue before the exit code
V['T1_pro_before_exit']=f0[:ex_i]+pro+f0[ex_i:]
# T2: prologue at the top, no entry jump
g=f0.replace('\tjpr\tL10001\nL10002:\n',pro_body+'L10002:\n',1); assert g!=f0
V['T2_pro_top']=g
# T3: prologue at the top with its label, entry jump kept as jump to next
g=f0.replace('\tjpr\tL10001\nL10002:\n','\tjpr\tL10001\nL10001:\n'+pro_body+'L10002:\n',1)
V['T3_pro_top_lab']=g
# T4: exit code before the body end? exit right after entry jump (exit block first)
for k,v in V.items():
    open(os.path.join(out,k+'.s'),'w').write(src[:a]+v+src[b:])
print(' '.join(V))
