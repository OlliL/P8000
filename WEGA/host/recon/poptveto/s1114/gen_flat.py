import os,re,itertools
src=open('/home/olivleh1/git/P8000/WEGA/src/cmd/cas/jpr.c').read()
a=src.index('\tncc = 0;'); b=src.index('done:')
tail=src[src.index('\tif (type == 1) {',b):]
tail=tail.replace('\treturn;\nplain:\n\tjpput(tok, list);\n\treturn (0);\n}','\treturn;\n}',1)
PL={'copy':'jpput(tok, list);\n\t\t\treturn (0);','goto':'goto plain;'}
def form(loop,plain,dfl):
    pl1='\t\tdefault:\n\t\t\tjpput(tok, list);\n\t\t\treturn (0);\n' if plain=='copy' else '\t\tdefault:\n\t\tplain:\n\t\t\tjpput(tok, list);\n\t\t\treturn (0);\n'
    c56='\t\tcase 0x56:\n\t\t\tbreak;\n'
    c36='\t\tcase 0x36:\n\t\t\tif (tok == 0x51)\n\t\t\t\terror(2, "Invalid operand combination\\n");\n\t\t\tncc++;\n\t\t\tcontinue;\n'
    order={'mid':c36+pl1+c56,'last':c36+c56+pl1,'first':pl1+c36+c56}[dfl]
    p=PL[plain]
    body='\t\tswitch (p->n_type) {\n'+order+'\t\t}\n\t\te = p->n_u.p;\n\t\ttype = exprtype(e);\n\t\tswitch (e->n_type) {\n\t\tcase 3:\n\t\t\t'+p+'\n\t\tcase 7:\n\t\t\tsym = e->n_u.pw.w;\n\t\t\toff = 0;\n\t\t\tbreak;\n\t\tcase 0x19:\n\t\t\tif (type != 7 && type != 1) {\n\t\t\t\t'+p+'\n\t\t\t}\n\t\t\tsym = (e->n_u.cp.p1)->n_u.pw.w;\n\t\t\toff = (e->n_u.cp.p2)->n_u.l;\n\t\t\tif (e->n_u.cc.c2 == 0x1b)\n\t\t\t\toff = -off;\n\t\t\tbreak;\n\t\tdefault:\n\t\t\terror(2, "Expressions used in jpr must be constant\\n");\n\t\t}\n'
    if loop=='for': l='\tfor (p = list; p; p = p->n_next) {\n'+body+'\t}\n'
    elif loop=='gd': l='\tfor (p = list; ; p = p->n_next) {\n\t\tif (p == 0)\n\t\t\tgoto done;\n'+body+'\t}\ndone:\n'
    elif loop=='while': l='\tp = list;\n\twhile (p) {\n'+body.replace('ncc++;\n\t\t\tcontinue;','ncc++;\n\t\t\tp = p->n_next;\n\t\t\tcontinue;')+'\t\tp = p->n_next;\n\t}\n'
    return src[:a]+'\tncc = 0;\n'+l+tail
for loop,plain,dfl in itertools.product(('for','gd','while'),('copy','goto'),('mid','last','first')):
    d='%s_%s_%s'%(loop,plain,dfl); os.makedirs(d,exist_ok=True)
    open(d+'/jpr.c','w').write(form(loop,plain,dfl))
