import sys
p=sys.argv[1]; s=open(p).read()
a='''	p1 = p;
	tl = insertl(p1);
	p3->subop = revbr[(int)p3->subop];'''
assert s.count(a)==1
s=s.replace(a,'''	{
		register struct node *q;
		int fw = 0, sn = 0, sc = 0, bj = 0;
		for (q = p; q; q = q->back)
			if (q == p3)
				break;
		fw = (q == 0);
		for (q = p2; q && q != p3->forw; q = q->forw) {
			if (q->op != LABEL && q->op != KCOMMENT)
				sn++;
			if (q->op == CJP)
				sc++;
		}
		for (q = p->back; q && (q->op == KCOMMENT); q = q->back)
			;
		bj = q ? q->op : -1;
		printf("/*INVF fw %d jp L%d next L%d refnext %d sn %d sc %d prevop %d tgtrefc %d*/\\n", fw, p->labno, p->forw->labno, p->forw->refc, sn, sc, bj, p->ref ? p->ref->refc : -1);
	}
	p1 = p;
	tl = insertl(p1);
	p3->subop = revbr[(int)p3->subop];''')
open(p,'w').write(s)
