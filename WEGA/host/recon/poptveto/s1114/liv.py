import sys
f=sys.argv[1]; s=open(f).read()
a="""	for (;;) {
		if ((p3 = p3->forw) == 0 || p3 == p1 || --n == 0)
			return p1;
		if (p3->op == CJP && p3->labno == p1->forw->labno)
			break;
	}
"""
b="""#ifdef LIV
	{
	struct node *jf = 0, *ml = 0, *q, *w;
	for (;;) {
		if ((p3 = p3->forw) == 0 || p3 == p1 || --n == 0)
			return p1;
		if (p3->op == CJP && p3->labno == p1->forw->labno)
			break;
		if (p3->op == JP && p3->ref && jf == 0) {
			for (q = p3->ref; q->back && (q->back->op == LABEL || q->back->op == KCOMMENT); q = q->back)
				;
			if (q->back && (q->back->op == JP || q->back->op == RET) && q->back != p3) {
				for (w = p3; w && w != p1 && w != q; w = w->forw)
					;
				if (w == q) {
					jf = p3;
					ml = q->back;
					p3 = ml;
				}
			}
		}
	}
	if (jf) {
		/* the stretch jf .. ml (the followed jump and the blocks behind it) goes behind the jump left at the loop head */
		jf->back->forw = ml->forw;
		ml->forw->back = jf->back;
		livj = jf;
		livm = ml;
	} else
		livj = 0;
	}
#else
"""+a+"#endif\n"
assert a in s
s=s.replace(a,b,1)
a="""	decref(tl);
	if (tl->refc <= 0)
		nrlab--;
	loopiv++;
"""
b="""#ifdef LIV
	if (livj) {
		/* p1 now stands at the old loop head: put the stretch behind it */
		livm->forw = p1->forw;
		p1->forw->back = livm;
		p1->forw = livj;
		livj->back = p1;
		livj = 0;
	}
#endif
"""+a
assert a in s
s=s.replace(a,b,1)
a="\nstruct node *\ncodemove(p)\n"
assert a in s
s=s.replace(a,"\n#ifdef LIV\nstatic struct node *livj, *livm;\n#endif\n"+a,1)
open(f,'w').write(s)
