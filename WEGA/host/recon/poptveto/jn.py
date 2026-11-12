import sys, itertools; sys.path.insert(0,'/tmp/SCRATCH/tools')
from fsearch import *
src=open(W+'jpr.c').read(); i,j=func_span(src,'jpr'); f=src[i:j]
head=f[:f.index('\tfor (p = list;')]
tail=f[f.index('\tif (type == 1) {'):].replace('\treturn;\nplain:\n\tjpput(tok, list);\n\treturn (0);\n}','\treturn;\n}')
def sw2(o2):
    b='\t\t\t'
    P={'3':b+'case 3:\n'+b+'\tgoto plain;\n',
       '7':b+'case 7:\n'+b+'\tsym = e->n_u.pw.w;\n'+b+'\toff = 0;\n'+b+'\tbreak;\n',
       '19':b+'case 0x19:\n'+b+'\tif (type != 7 && type != 1)\n'+b+'\t\tgoto plain;\n'+b+'\tsym = (e->n_u.cp.p1)->n_u.pw.w;\n'+b+'\toff = (e->n_u.cp.p2)->n_u.l;\n'+b+'\tif (e->n_u.cc.c2 == 0x1b)\n'+b+'\t\toff = -off;\n'+b+'\tbreak;\n',
       'd':b+'default:\n'+b+'\terror(2, "Expressions used in jpr must be constant\\n");\n'}
    s=''.join(P[k] for k in o2)
    if o2[-1]!='d': s=s.replace(P['d'],P['d']+b+'\tbreak;\n')
    return b+'e = p->n_u.p;\n'+b+'type = exprtype(e);\n'+b+'switch (e->n_type) {\n'+s+b+'}\n'
def mk(o1,o2):
    P1={'a':'\t\tcase 0x36:\n\t\t\tif (tok == 0x51)\n\t\t\t\terror(2, "Invalid operand combination\\n");\n\t\t\tncc++;\n\t\t\tcontinue;\n',
        'b':'\t\tcase 0x56:\n'+sw2(o2)+'\t\t\tbreak;\n',
        'd':'\t\tdefault:\n\t\tplain:\n\t\t\tjpput(tok, list);\n\t\t\treturn (0);\n'}
    return head+'\tfor (p = list; p != 0; p = p->n_next) {\n\t\tswitch (p->n_type) {\n'+''.join(P1[k] for k in o1)+'\t\t}\n\t}\n'+tail
if __name__=='__main__':
    tasks=[]
    for p in sys.argv[1].split(','):
        for o1 in itertools.permutations('abd'):
            for o2 in itertools.permutations(['3','7','19','d']):
                tasks.append(('%s-%s-%s'%(p,''.join(o1),'.'.join(o2)),'jpr',{'jpr.c':src[:i]+mk(o1,o2)+src[j:]},p,('jpr',)))
    go(tasks,10)
