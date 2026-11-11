import sys,os
p=sys.argv[1]; s=open(p).read()
old='''				ncmot++;
				nchange++;
				p1->back->forw = p2;'''
assert s.count(old)==1
feat='''				{
					register struct node *q;
					tn = tc = 0;
					for (q = p2; q && q != p3->forw; q = q->forw) {
						if (q->op != LABEL && q->op != KCOMMENT)
							tn++;
						if (q->op == CJP)
							tc++;
					}
					jn = jc = jl = pre = 0;
					for (q = p1->back; q && q->op != LABEL; q = q->back) {
						if (q->op == KCOMMENT)
							continue;
						jn++;
						if (q->op == CJP)
							jc++;
						if (q->op == JP || q->op == RET || q->op == JSW)
							break;
					}
					if (q && q->op == LABEL) {
						jl = q->refc;
						while (q->back && (q->back->op == LABEL || q->back->op == KCOMMENT))
							q = q->back;
						pre = q->back && (q->back->op == JP || q->back->op == RET);
					}
					lr = p1->ref->refc;
				}
'''
s=s.replace(old,feat+old)
s=s.replace('''	struct node *t, *tl;
	int n;
''','''	struct node *t, *tl;
	int n;
	int tn, tc, jn, jc, jl, pre, lr;
''',1)
old2='''				p3->forw = p1->forw;
				decref(p1->ref);
				return p2;'''
assert s.count(old2)==1
s=s.replace(old2,'''				p3->forw = p1->forw;
				if (!(p1->labno >= 10000 && p1->labno < 20000) && (%s)) {
					decref(p1->ref);
					return p3;
				}
				decref(p1->ref);
				return p2;'''%os.environ['D41C'])
open(p,'w').write(s)
