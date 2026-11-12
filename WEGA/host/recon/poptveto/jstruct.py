import sys, itertools; sys.path.insert(0,'/tmp/SCRATCH/tools')
from fsearch import *
src=open(W+'jpr.c').read(); i,j=func_span(src,'jpr'); f=src[i:j]
head=f[:f.index('\tncc = 0;')]
tail=f[f.index('\tif (type == 1) {'):].replace('\treturn;\nplain:\n\tjpput(tok, list);\n\treturn (0);\n}','\treturn;\n}')
PL='jpput(tok, list);\n%sreturn (0);\n'
def sw2(ind, o2, dfirst):
    b=ind
    P={'3':b+'case 3:\n'+b+'\tgoto plain;\n',
       '7':b+'case 7:\n'+b+'\tsym = e->n_u.pw.w;\n'+b+'\toff = 0;\n'+b+'\tbreak;\n',
       '19':b+'case 0x19:\n'+b+'\tif (type != 7 && type != 1)\n'+b+'\t\tgoto plain;\n'+b+'\tsym = (e->n_u.cp.p1)->n_u.pw.w;\n'+b+'\toff = (e->n_u.cp.p2)->n_u.l;\n'+b+'\tif (e->n_u.cc.c2 == 0x1b)\n'+b+'\t\toff = -off;\n'+b+'\tbreak;\n'}
    D=b+'default:\n'+b+'\terror(2, "Expressions used in jpr must be constant\\n");\n'
    body=(D+b+'\tbreak;\n' if dfirst else '')+''.join(P[k] for k in o2)+('' if dfirst else D)
    return b+'e = p->n_u.p;\n'+b+'type = exprtype(e);\n'+b+'switch (e->n_type) {\n'+body+b+'}\n'
def mk(loop, pl, nest, o1, o2, dfirst):
    cont = 'goto next' if loop=='while' else 'continue'
    b='\t\t'
    c36=b+'case 0x36:\n'+b+'\tif (tok == 0x51)\n'+b+'\t\terror(2, "Invalid operand combination\\n");\n'+b+'\tncc++;\n'+b+'\t'+cont+';\n'
    if pl=='lab36': c36+=b+'plain:\n'+b+'\t'+PL%(b+'\t')
    if nest: c56=b+'case 0x56:\n'+sw2(b+'\t',o2,dfirst)+b+'\tbreak;\n'
    else: c56=b+'case 0x56:\n'+b+'\tbreak;\n'
    cd=b+'default:\n'+((b+'plain:\n'+b+'\t'+PL%(b+'\t')) if pl=='def' else b+'\tgoto plain;\n')
    P={'a':c36,'b':c56,'d':cd}
    body=b[:-1]+'\tswitch (p->n_type) {\n'+''.join(P[k] for k in o1)+b+'}\n'
    if not nest: body+=sw2(b,o2,dfirst)
    if loop=='for': t='\tfor (p = list; p != 0; p = p->n_next) {\n'+body+'\t}\n'
    elif loop=='forb': t='\tfor (p = list; ; p = p->n_next) {\n\t\tif (p == 0)\n\t\t\tbreak;\n'+body+'\t}\n'
    else: t='\tp = list;\n\twhile (p != 0) {\n'+body+'\tnext:\n\t\tp = p->n_next;\n\t}\n'
    g=head+'\tncc = 0;\n'+t+tail
    if pl=='end': g=g[:-2]+'plain:\n\tjpput(tok, list);\n\treturn (0);\n}\n'
    return g
if __name__=='__main__':
    tasks=[]
    for loop in ('for','while','forb'):
      for pl in ('end','def','lab36'):
        for nest in (0,1):
          for o1 in itertools.permutations('abd'):
            for o2,df in ((('3','7','19'),0),(('3','7','19'),1),(('19','7','3'),0),(('7','3','19'),0)):
              tasks.append(('%s-%s-%d-%s-%s-%d'%(loop,pl,nest,''.join(o1),'.'.join(o2),df),'jpr',{'jpr.c':src[:i]+mk(loop,pl,nest,o1,o2,df)+src[j:]},sys.argv[1],('jpr',)))
    print(len(tasks),flush=True)
    res=go(tasks,15)
    import collections
    print(collections.Counter(min(x[0],99)//4*4 for x in res))
