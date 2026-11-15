import sys, re
# d61.py DIR: trial patch of codgen (PPY for cg.sh): the registers that the left operand of an outermost && takes stay busy through the right operand when that left operand is a bare name (vl, doprnt of slibc: `_pfile && ferror(_pfile)`)
d=sys.argv[1]
p=d+'/regallo3.c'; s=open(p).read()
a="int\ngetreg(cls, reg, flag, sym)\n"
assert s.count(a)==1
s=s.replace(a,"int\ngetreg0(cls, reg, flag, sym)\n")
s+="""
int d61rec, d61n, d61reg[16], d61cls[16];
int
getreg(cls, reg, flag, sym)
	int cls, reg, flag;
	struct sym *sym;
{
	int r = getreg0(cls, reg, flag, sym);

	if (d61rec && r >= 0 && cls <= 4 && d61n < 16) {
		d61reg[d61n] = r;
		d61cls[d61n++] = cls;
	}
	return r;
}
"""
open(p,'w').write(s)
p=d+'/reader3.c'; s=open(p).read()
a="""			d54depth++;
			cbranch(p->in.left, -1, lab);
			cbranch(p->in.right, true, false);
			d54depth--;"""
assert s.count(a)==1
s=s.replace(a,"""			extern int d61rec, d61n, d61reg[], d61cls[];
			extern struct rs *rfind();
			int hreg[16], hcls[16], nh = 0, i, sv = d61rec, sn = d61n;

			d54depth++;
			{ NODE *q = p->in.left; d61rec = q->in.op == NAME; } d61n = 0;
			cbranch(p->in.left, -1, lab);
			d61rec = 0;
			for (i = 0; i < d61n; i++)
				if (rfind(d61reg[i], d61cls[i]) == 0 && getreg0(d61cls[i], d61reg[i], 0, 0) == d61reg[i]) {
					hreg[nh] = d61reg[i]; hcls[nh++] = d61cls[i];
				}
			d61rec = sv; d61n = sn;
			cbranch(p->in.right, true, false);
			for (i = 0; i < nh; i++)
				if (rfind(hreg[i], hcls[i]))
					rsreturn(hcls[i], hreg[i]);
			d54depth--;""")
open(p,'w').write(s)
