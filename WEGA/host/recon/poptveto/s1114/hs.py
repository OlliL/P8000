import sys
f=sys.argv[1]; s=open(f).read()
a="""			if (p1->op == JP) {
				rp = p->ref;
				do
					rp = rp->back;
				while (rp->op == LABEL || rp->op == KCOMMENT);
				if (rp == p1) {
					decref(p->ref);"""
b="""#ifdef HSTAR
			if (p1->op == JP && p1->ref) {
				rp = p->ref;
				do
					rp = rp->back;
				while (rp->op == LABEL || rp->op == KCOMMENT);
				if (rp == p1) {
#ifdef HS_NOIV
					hs_noiv = 1;
#endif
					if (codemove(p1) != p1)
						p1 = p;
					hs_noiv = 0;
				}
			}
#endif
"""+a
assert a in s
s=s.replace(a,b,1)
a="	/* loop inversion */\n"
assert a in s
s=s.replace(a,a+"#ifdef HSTAR\n\tif (hs_noiv)\n\t\treturn p1;\n#endif\n",1)
a="\nvoid\niterate()\n"
assert a in s
s=s.replace(a,"\n#ifdef HSTAR\nint hs_noiv;\n#endif\n"+a,1)
# full D20 in spopt: allow OLD_D20 without OLD_D20S clash is fine (separate ifdefs)
open(f,'w').write(s)
