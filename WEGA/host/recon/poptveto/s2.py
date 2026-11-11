import sys,os
p=sys.argv[1]; s=open(p).read()
a='''					p->subop = revbr[(int)p->subop];
					nchange++;
					nskip++;
				}
			}
		}'''
assert s.count(a)==1
cond=os.environ.get('D41C','1')
s=s.replace(a,'''					p->subop = revbr[(int)p->subop];
					nchange++;
					nskip++;
				}
			}
			for (p1 = p->forw; p1->op == KCOMMENT; p1 = p1->forw)
				;
			if (p->op == CJP && p1->op == JP && p1->ref && p->ref && p1->ref != p->ref &&
			    (%s)) {
				rp = p->ref;
				do
					rp = rp->back;
				while (rp && (rp->op == LABEL || rp->op == KCOMMENT));
				if (rp && rp != p1 && (rp->op == JP || rp->op == RET)) {
					struct node *t2;
					int l2;

					int k2;
					static struct node *sw2[512];
					static int nsw2;

					for (k2 = 0; k2 < nsw2; k2++)
						if (sw2[k2] == p)
							break;
					if (k2 < nsw2 || nsw2 >= 512)
						goto nos2;
					sw2[nsw2++] = p;
					t2 = p->ref; p->ref = p1->ref; p1->ref = t2;
					l2 = p->labno; p->labno = p1->labno; p1->labno = l2;
					p->subop = revbr[(int)p->subop];
					nchange++;
				}
			}
		nos2:
			;
		}'''%cond)
open(p,'w').write(s)
