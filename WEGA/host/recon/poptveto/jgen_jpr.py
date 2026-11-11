import itertools, sys
sys.path.insert(0,'/tmp/claude-1000/-mnt-share-cvs-P8000/6b8d4c9d-ace2-40e9-b4e5-33a880fd019b/scratchpad')
import par
base=open(par.S+'/jpr.goto.c').read()
i=base.index("	ncc = 0;\n	for (p = list;"); j=base.index("	if (type == 1) {")
TAIL_OLD="""	return;
plain:
	jpput(tok, list);
	return (0);
}"""
assert TAIL_OLD in base
def gen(nest, o1, end36, pm, o2, d2e, loop='for'):
    def plain(ind, site):
        # site: 'd' default of sw1, '3' case 3, 't' type test
        if pm=='n' or pm==site:
            lab = (ind[:-1]+"plain:\n") if pm==site else ""
            return lab+ind+"jpput(tok, list);\n"+ind+"return (0);\n"
        return ind+"goto plain;\n"
    ind1='\t\t\t'
    c36="		case 0x36:\n			if (tok == 0x51)\n				error(2, \"Invalid operand combination\\n\");\n			ncc++;\n			%s;\n"%end36
    cdef="		default:\n"+plain(ind1,'d')
    bi = '\t\t\t' if nest else '\t\t'
    def sw2():
        t=bi+"e = p->n_u.p;\n"+bi+"type = exprtype(e);\n"+bi+"switch (e->n_type) {\n"
        parts={}
        parts['3']=bi+"case 3:\n"+plain(bi+'\t','3')
        parts['7']=bi+"case 7:\n"+bi+"	sym = e->n_u.pw.w;\n"+bi+"	off = 0;\n"+bi+"	break;\n"
        tt=plain(bi+'\t\t','t')
        if tt.count('\n')>1: tt=" {\n"+tt+bi+"	}\n"
        else: tt="\n"+tt
        parts['19']=bi+"case 0x19:\n"+bi+"	if (type != 7 && type != 1)"+tt+bi+"	sym = (e->n_u.cp.p1)->n_u.pw.w;\n"+bi+"	off = (e->n_u.cp.p2)->n_u.l;\n"+bi+"	if (e->n_u.cc.c2 == 0x1b)\n"+bi+"		off = -off;\n"+bi+"	break;\n"
        e = d2e
        if o2[-1]!='def' and e=='': e='break'
        parts['def']=bi+"default:\n"+bi+"	error(2, \"Expressions used in jpr must be constant\\n\");\n"+(bi+"	"+e+";\n" if e else "")
        for k in o2: t+=parts[k]
        t+=bi+"}\n"
        return t
    if nest:
        c56="		case 0x56:\n"+sw2()+"			break;\n"
    else:
        c56="		case 0x56:\n			break;\n"
    P={'36':c36,'56':c56,'def':cdef}
    head={'for':"	for (p = list; p != 0; p = p->n_next) {\n",'while':"	p = list;\n	while (p != 0) {\n"}[loop]
    t="	ncc = 0;\n"+head+"		switch (p->n_type) {\n"+''.join(P[k] for k in o1)+"		}\n"
    if not nest: t+=sw2()
    t+="	}\n"
    s=base[:i]+t+base[j:]
    if pm=='e':
        return s
    return s.replace(TAIL_OLD,"}")
def space():
    for nest in (0,1):
        for o1 in itertools.permutations(['36','56','def']):
            for end36 in (('continue','break') if nest else ('continue',)):
                for pm in ('n','d','e','3','t'):
                    for o2 in itertools.permutations(['3','7','19','def']):
                        for d2e in ('','continue'):
                            yield (nest,o1,end36,pm,o2,d2e)
if __name__=='__main__':
    popt=sys.argv[1]; out=open(par.S+'/jres_%s.txt'%popt,'w')
    tasks=[]
    for k in space():
        try: src=gen(*k)
        except Exception as ex: continue
        tasks.append((repr(k),'jpr',{'jpr.c':src},popt,('jpr','jprnew')))
    print(len(tasks),flush=True)
    for name,res,others in par.run(tasks):
        out.write('%s\t%s\t%s\n'%(res,others,name)); out.flush()
