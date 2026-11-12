import sys, itertools; sys.path.insert(0,'/tmp/SCRATCH/tools')
from fsearch import *
exec(open('/tmp/SCRATCH/jstruct.py').read().split("if __name__")[0].split("def mk(")[0])
def mk(variant, pl, o1, o2, dfirst):
    b='\t\t'
    c36=b+'case 0x36:\n'+b+'\tif (tok == 0x51)\n'+b+'\t\terror(2, "Invalid operand combination\\n");\n'+b+'\tncc++;\n'+b+'\tgoto next;\n'
    if pl=='lab36': c36+=b+'plain:\n'+b+'\t'+PL%(b+'\t')
    c56=b+'case 0x56:\n'+b+'\tbreak;\n'
    cd=b+'default:\n'+((b+'plain:\n'+b+'\t'+PL%(b+'\t')) if pl=='def' else b+'\tgoto plain;\n')
    P={'a':c36,'b':c56,'d':cd}
    body='\tswitch (p->n_type) {\n'.replace('\t','\t',1)
    body='\t\tswitch (p->n_type) {\n'+''.join(P[k] for k in o1)+b+'}\n'+sw2(b,o2,dfirst)
    if variant=='g1':   # p = list; goto test; loop: body; next: p = p->n_next; test: if (p) goto loop;
        t='\tp = list;\n\tgoto test;\nloop:\n'+body+'next:\n\tp = p->n_next;\ntest:\n\tif (p != 0)\n\t\tgoto loop;\n'
    elif variant=='g2': # p = list; test: if (p == 0) goto done; body; next: p = p->n_next; goto test; done:
        t='\tp = list;\ntest:\n\tif (p == 0)\n\t\tgoto out;\n'+body+'next:\n\tp = p->n_next;\n\tgoto test;\nout:\n'
    else:               # p = list; goto test; next: p = p->n_next; test: if (p == 0) goto out; body; goto next; out:
        t='\tp = list;\n\tgoto test;\nnext:\n\tp = p->n_next;\ntest:\n\tif (p == 0)\n\t\tgoto out;\n'+body+'\tgoto next;\nout:\n'
    t=t.replace('\n\t\t','\n\t').replace('\n\t\t','\n\t') if False else t
    g=head+'\tncc = 0;\n'+t+tail
    if pl=='end': g=g[:-2]+'plain:\n\tjpput(tok, list);\n\treturn (0);\n}\n'
    return g
#MAIN
if True:
    tasks=[]
    for v in ('g1','g2','g3'):
      for pl in ('end','def','lab36'):
        for o1 in itertools.permutations('abd'):
          for o2,df in ((('3','7','19'),0),(('3','7','19'),1),(('19','7','3'),0),(('7','3','19'),0)):
            tasks.append(('%s-%s-%s-%s-%d'%(v,pl,''.join(o1),'.'.join(o2),df),'jpr',{'jpr.c':src[:i]+mk(v,pl,o1,o2,df)+src[j:]},sys.argv[1],('jpr',)))
    print(len(tasks),flush=True)
    res=go(tasks,15)
    import collections
    print(collections.Counter(min(x[0],99)//4*4 for x in res))
