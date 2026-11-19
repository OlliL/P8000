/*
 * reader2.c - gopt: main, the reader of the intermediate file and the pass control. Written from /lib/gopt (no source known);
 * the names of the callees in other modules are placeholders (fnXXXX = the address in /lib/gopt, segment 4) until their modules exist.
 */
#include "gopt.h"

static char sccsid[] = "@[$]reader2.c\t1.5  03/04/83 16:01:42 Copr. 1981 87wega3.2.";

extern char ifname[];
extern int ifline, ifline2;
extern char *ifnamep;
extern FILE *infile;
extern long ifpos;
extern long head;
extern int eflag, qflag, tflag, s4d92, nerrors;
extern char *arhigh[];
extern int ntmp;
extern NODE *saved[];
extern NODE *curproc;
extern long **lastp;
extern long vb64, vb04, v06, vb94;
extern int nconst, nfconst, v0e, vafc, v00, v04, vafe, vb5e;
extern int s4d7a, s0078, s4d74, s4d76, ntl, s4d78, s4d94, s0240, nctmp, s0244;
extern long s4d96;
extern char ctneg[], ctpos[];
extern char *opst[];
extern long ftell();
extern char *taballoc();
extern void tabfree();
extern long rdin();
extern NODE *eread();
extern struct sym *mapn();
extern void mkhash(), nlcheck(), prtags(), fwalk(), mapnm(), ncopy(), tprint();
extern NODE *tcopy();
extern long addquad();
extern int constlook();
extern void genquads(), aliasm(), prquads(), fnf5fe(), fn5_0000(), fn5_6eda(), writeil(), mapall();
extern int fn9108(), fn927e(), fnc65a(), fnc69e(), rdsym();
extern int dope[];

static void dotree(), dotree2(), cutree(), passes(), abortpass(), prtbl();
extern void pr();

main(argc, argv)
	int argc;
	char **argv;
{
	register int c;
	register char *q;
	register NODE *p;
	int lno;
	int i;
	struct qa s1, s2, s3;
	struct qa *p1, *p2, *p3;
	int v1, v2;
	char ch;
	int c2, c3;

	lno = 1;
	p1 = &s1;
	p2 = &s2;
	p3 = &s3;
	fnc69e(argc, argv);
	tinit();
	for (;;) {
		c = getc(infile);
		if (c <= 0)
			break;
		switch (c) {
		case '{':
			s4d7a = 0;
			s0078 = 0;
			vb64 = 0;
			nconst = 0;
			nfconst = 0;
			vb04 = 0;
			vb64 = 0;
			v0e = 1;
			tinit();
			s4d74 = 0;
			s4d76 = 0;
			ntl = 0;
			s4d78 = 0;
			vafc = 0;
			lno = 1;
			s4d94 = 0;
			s0240 = 0;
			nctmp = 0;
			s0244 = 0;
			for (i = 0; i <= 1000; i++) {
				ctneg[i] = 0;
				ctpos[i] = 0;
			}
			mkhash();
			v00 = rdin(10);
			vb94 = rdin(10);
			v06 = rdin(10);
			v04 = rdin(10);
			nlcheck();
			if (!rdsym())
				cerror("error in reading symbol table file");
			head = addquad(0x2e, 0L, 0L, 0L);
			break;
		case '}':
			addquad(0x11, 0L, 0L, 0L);
			nlcheck();
			passes();
			tabfree(0, 0L);
			tabfree(1, 0L);
			tabfree(2, 0L);
			tabfree(3, 0L);
			tabfree(4, 0L);
			tabfree(5, 0L);
			tabfree(6, 0L);
			tabfree(7, 0L);
			tabfree(8, 0L);
			break;
		case ']':
			if (!addquad(0x0f, 0L, 0L, 0L))
				goto err;
			nlcheck();
			break;
		case '[':
			s4d96 = rdin(10);
			v1 = rdin(10);
			v2 = rdin(10);
			if (vafc == 0)
				vafc = lno;
			p1->b2 = 7;
			p1->w4 = constlook(s4d96);
			p2->b2 = 1;
			p2->w4 = v1;
			p3->b2 = 1;
			p3->w4 = v2;
			p1->w0 = lno;
			lno++;
			if (!addquad(6, p1, p2, p3))
				goto err;
			nlcheck();
			break;
		case '.':
			ifline = rdin(10);
			ifline2 = ifline;
			ifnamep = ifname;
			q = ifname;
			for (;;) {
				if ((*q = getc(infile)) == '\n')
					break;
				q++;
			}
			*q = 0;
			vafe = 0;
			p = eread();
			dotree(p);
			if (vafe) {
		err:
				abortpass();
				fseek(infile, ifpos, 0);
				tabfree(1, head);
				head = taballoc(0, 1);
			}
			break;
		case 'L':
			p1->b2 = 1;
			p1->w4 = rdin(10);
			if (!addquad(0x1c, p1, 0L, 0L))
				goto err;
			nlcheck();
			break;
		case 'B':
			p1->b2 = 1;
			p1->w4 = rdin(10);
			if (!addquad(0x1a, p1, 0L, 0L))
				goto err;
			nlcheck();
			ifpos = ftell(infile);
			break;
		case 'C':
			while ((ch = getc(infile)) == ' ')
				;
			ungetc(ch, infile);
			p1->b2 = 1;
			p1->w4 = rdin(10);
			if (!addquad(9, p1, 0L, 0L))
				goto err;
			while ((ch = getc(infile)) != '\n') {
				ungetc(ch, infile);
				p1->b2 = 1;
				p1->w4 = rdin(10);
				if (!addquad(10, p1, 0L, 0L))
					goto err;
			}
			if (!addquad(11, 0L, 0L, 0L))
				goto err;
			ifpos = ftell(infile);
			break;
		case '/':
			putc(c, stdout);
			while ((c3 = putc(c2 = getc(infile), stdout)) != '/n')
				;
			break;
		default:
			cerror("intermediate file format error - %c", c);
		}
	}
	return nerrors;
}

/* a tree of the intermediate file: a function head (PROC) or a statement */
static void
dotree(p)
	NODE *p;
{
	if (p->in.op == 0x76) {
		if (eflag)
			fwalk(p, pr, 0);
		fwalk(p, mapnm);
		if (eflag)
			fwalk(p, pr, 0);
	} else if (p->in.op == 0x77) {
		if (eflag)
			fwalk(p, pr, 0);
		mapnm(p->in.left);
	} else {
		mapall(p);
		dotree2(p);
		return;
	}
	curproc = p;
}

static void
dotree2(p)
	register NODE *p;
{
	register int i;

	ntmp = 0;
	if (eflag)
		fwalk(p, pr, 0);
	fwalk(p, mapnm);
	cutree(p);
	genquads(p);
	p->in.op = FREE;
	for (i = 0; i < ntmp; i++) {
		genquads(saved[i]);
		saved[i]->in.op = FREE;
	}
}

static void
cutree(p)
	register NODE *p;
{
	register int op, kind;
	register NODE *q;

	op = p->in.op;
	kind = optype(op);
	switch (op) {
	case 0x46: case 0x48: case 0x67: case 0x69:
		s4d7a = 1;
		return;
	case 0x4c: case 0x73:
		return;
	case 0x4e: case 0x4f:
		if (ntmp < 20) {
			saved[ntmp] = tcopy(p);
			ntmp++;
			q = p->in.left;
			p->in.right->in.op = FREE;
			ncopy(p, q);
			q->in.op = FREE;
			return;
		}
	}
	if (kind == BITYPE)
		cutree(p->in.right);
	if (kind != LTYPE)
		cutree(p->in.left);
}

/* print a node (fwalk callback, debug) */
void
pr(p, down, down1, down2)
	NODE *p;
	int down;
	int *down1, *down2;
{
	*down1 = *down2 = down + 1;
	for (; down >= 2; down -= 2)
		putchar('\t');
	if (down--)
		printf("    ");
	printf("%%(8)%0.2o) %s", p, opst[p->in.op]);
	switch (p->in.op) {
	case NAME: case ICON:
		putchar(' ');
		break;
	case REG:
		break;
	case STASG: case STARG: case STCALL: case USTCALL:
		printf(" size = %d", p->stn.stsize);
		printf(" align = %d", p->stn.stalign);
		break;
	}
	printf(", ");
	tprint(p->in.type, stdout);
	if (optype(p->in.op) == UTYPE)
		printf(", rval = %d\n", p->tn.rval);
	else
		printf("\n");
}

static void
abortpass()
{
	s0078 = 1;
	*lastp = 0;
	passes();
}

static void
passes()
{
	if (s0078 == 0) {
		aliasm();
		if (qflag > 4) {
			printf("// quads before aliasm\n");
			prquads();
		}
		if (fn9108() && qflag > 4)
			prquads();
		if (fn927e()) {
			fnf5fe();
			if (vb5e > 1)
				fn5_0000();
			if (vb5e >= 3 && fnc65a() == 0)
				;
			if (vb5e == 3 || vb5e > 4)
				fn5_6eda();
		}
	}
	if (tflag == 0)
		writeil();
	if (s4d92)
		prtbl();
}

static void
prtbl()
{
	int i;

	for (i = 0; i < 11; i++)
		printf("// table no. = %d, space used = %d\n", i, (int)arhigh[i]);
}
