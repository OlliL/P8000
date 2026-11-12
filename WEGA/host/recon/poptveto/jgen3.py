import itertools,os,sys,subprocess
from concurrent.futures import ThreadPoolExecutor
S='/tmp/SCRATCH/'
src=open(S+'jpr_old.c').read()
a=src.index('jpr(tok, list)'); b=src.index('\n}\n',a)+3
pre,post=src[:a],src[b:]
HDR='''jpr(tok, list)
int tok;
NODE *list;
{
	register NODE *p;
	register NODE *e;
	register int ncc;
	register int type;
	int sym;
	int idx;
	int res;
	long off;

	ncc = 0;
'''
TAIL='''	if (type == 1) {
		res = jprange(tok, sym, off);
		switch (res) {
		case 0:
			jrput(tok, list);
			return;
		case 1:
			jpput(tok, list);
			return;
		case 2:
			break;
		default:
			return;
		}
	}
	idx = jprnew(tok, sym, off);
	jprput(idx, tok, ncc, list);
	return;
'''
def gen(loop, o1, dflt, o2, h):
    t='\t\t'
    c={}
    c['36']=[t+'case 0x36:',t+'\tif (tok == 0x51)',t+'\t\terror(2, "Invalid operand combination\\n");',t+'\tncc++;',t+'\tcontinue;']
    c['56']=[t+'case 0x56:',t+'\tbreak;']
    c['d']={'goto':[t+'default:',t+'\tgoto plain;'],
            'inline':[t+'default:','\tplain:' if False else t[:-1]+'plain:',t+'\tjpput(tok, list);',t+'\treturn (0);'],
            'copy':[t+'default:',t+'\tjpput(tok, list);',t+'\treturn (0);']}[dflt]
    if dflt=='inline': c['d']=[t+'default:',t+'plain:',t+'\tjpput(tok, list);',t+'\treturn (0);']
    s2={}
    s2['3']=[t+'case 3:',t+'\tgoto plain;']
    s2['7']=[t+'case 7:',t+'\tsym = e->n_u.pw.w;',t+'\toff = 0;',t+'\tbreak;']
    if h=='h1':
        s2['19']=[t+'case 0x19:',t+'\tif (type != 7 && type != 1) {',t+'\t\tgoto plain;',t+'\t}',t+'\tsym = (e->n_u.cp.p1)->n_u.pw.w;',t+'\toff = (e->n_u.cp.p2)->n_u.l;',t+'\tif (e->n_u.cc.c2 == 0x1b)',t+'\t\toff = -off;',t+'\tbreak;']
    else:
        s2['19']=[t+'case 0x19:',t+'\tif (type == 7 || type == 1) {',t+'\t\tsym = (e->n_u.cp.p1)->n_u.pw.w;',t+'\t\toff = (e->n_u.cp.p2)->n_u.l;',t+'\t\tif (e->n_u.cc.c2 == 0x1b)',t+'\t\t\toff = -off;',t+'\t\tbreak;',t+'\t}',t+'\tgoto plain;']
    s2['d']=[t+'default:',t+'\terror(2, "Expressions used in jpr must be constant\\n");']
    L=[]
    if loop=='for': L.append('\tfor (p = list; p != 0; p = p->n_next) {')
    else: L+=['\tfor (p = list; ; p = p->n_next) {','\t\tif (p == 0)','\t\t\tgoto done;']
    L.append('\t\tswitch (p->n_type) {')
    for k in o1: L+=c[k]
    L.append('\t\t}')
    L+=['\t\te = p->n_u.p;','\t\ttype = exprtype(e);','\t\tswitch (e->n_type) {']
    for k in o2: L+=s2[k]
    L+=['\t\t}','\t}']
    if loop!='for': L.append('done:')
    body=HDR+'\n'.join(L)+'\n'+TAIL
    if dflt!='inline': body+='plain:\n\tjpput(tok, list);\n\treturn (0);\n'
    body+='}\n'
    return pre+body+post
builds=sys.argv[1].split(',')
jobs=[]
for loop in ('for','gd'):
  for o1 in itertools.permutations(['36','56','d']):
    for dflt in ('goto','inline','copy'):
      for o2 in itertools.permutations(['3','7','19','d']):
        for h in ('h1','h2'):
          jobs.append((loop,o1,dflt,o2,h))
def run(j):
    name='%s_%s_%s_%s_%s'%(j[0],''.join(j[1]),j[2],'-'.join(j[3]),j[4])
    d=S+'g3/'+name; os.makedirs(d,exist_ok=True)
    for hh in ('cas.h','pstdata.h','stdio.h'):
        if not os.path.exists(d+'/'+hh): os.symlink(S+'raw/'+hh,d+'/'+hh)
    open(d+'/jpr.c','w').write(gen(*j))
    if not os.path.exists(d+'/jpr.s'):
        subprocess.run([S+'cc_q3','-S','jpr.c'],cwd=d,capture_output=True)
    if not os.path.exists(d+'/jpr.s'): return name,['FAIL']
    r=[]
    for bb in builds:
        o=subprocess.run(['python3','/mnt/share/cvs/P8000/WEGA/host/recon/poptveto/jfast.py',S+'o_%s/lib/vc/spopt'%bb,d+'/jpr.s'],capture_output=True,text=True).stdout.split()
        r.append(o[0] if o else '?')
    return name,r
out=open(S+'g3.txt','w')
with ThreadPoolExecutor(8) as ex:
    for name,r in ex.map(run,jobs):
        out.write('%s %s\n'%(name,' '.join(r))); out.flush()
