import os,itertools
src=open('/home/olivleh1/git/P8000/WEGA/src/cmd/cas/jpr.c').read()
a=src.index('\tncc = 0;'); b=src.index('done:')
tail=src[src.index('\tif (type == 1) {',b):]
tail=tail.replace('\treturn;\nplain:\n\tjpput(tok, list);\n\treturn (0);\n}','\treturn;\n}',1)
def form(loop,plain,order,end):
    p='jpput(tok, list);\n\t\t\t\treturn (0);' if plain=='copy' else 'goto plain;'
    pl1='\t\tdefault:\n'+('' if plain=='copy' else '\t\tplain:\n')+'\t\t\tjpput(tok, list);\n\t\t\treturn (0);\n'
    c36='\t\tcase 0x36:\n\t\t\tif (tok == 0x51)\n\t\t\t\terror(2, "Invalid operand combination\\n");\n\t\t\tncc++;\n\t\t\tcontinue;\n'
    inner='\t\t\te = p->n_u.p;\n\t\t\ttype = exprtype(e);\n\t\t\tswitch (e->n_type) {\n\t\t\tcase 3:\n\t\t\t\t'+p+'\n\t\t\tcase 7:\n\t\t\t\tsym = e->n_u.pw.w;\n\t\t\t\toff = 0;\n\t\t\t\t'+end+';\n\t\t\tcase 0x19:\n\t\t\t\tif (type != 7 && type != 1) {\n\t\t\t\t\t'+p+'\n\t\t\t\t}\n\t\t\t\tsym = (e->n_u.cp.p1)->n_u.pw.w;\n\t\t\t\toff = (e->n_u.cp.p2)->n_u.l;\n\t\t\t\tif (e->n_u.cc.c2 == 0x1b)\n\t\t\t\t\toff = -off;\n\t\t\t\t'+end+';\n\t\t\tdefault:\n\t\t\t\terror(2, "Expressions used in jpr must be constant\\n");\n\t\t\t}\n'
    c56='\t\tcase 0x56:\n'+inner+('\t\t\tbreak;\n' if order!='ad5' else '')
    o={'ad5':c36+pl1+c56,'a5d':c36+c56+pl1,'5ad':c56+c36+pl1,'da5':pl1+c36+c56}[order]
    body='\t\tswitch (p->n_type) {\n'+o+'\t\t}\n'
    if loop=='for': l='\tfor (p = list; p; p = p->n_next) {\n'+body+'\t}\n'
    else: l='\tfor (p = list; ; p = p->n_next) {\n\t\tif (p == 0)\n\t\t\tgoto done;\n'+body+'\t}\ndone:\n'
    return src[:a]+'\tncc = 0;\n'+l+tail
for loop,plain,order,end in itertools.product(('for','gd'),('copy','goto'),('ad5','a5d','5ad','da5'),('break','continue')):
    d='%s_%s_%s_%s'%(loop,plain,order,end); os.makedirs(d,exist_ok=True)
    open(d+'/jpr.c','w').write(form(loop,plain,order,end))
