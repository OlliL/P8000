/*
 * reader2.c - gopt: main, the reader of the intermediate file and the pass control. Written from /lib/gopt (no source known);
 * the names of the callees in other modules are placeholders (fnXXXX = the address in /lib/gopt, segment 4) until their modules exist.
 */
static char sccsid[] __attribute__((unused)) = "@[$]reader2.c\t1.5  03/04/83 16:01:42 Copr. 1981 87wega3.2.";
#include "mfile2.h"
static void prtbl(void);
static void passes(void);
static void abortpass(void);
static void cutree(NODE *p);

int aborted = 0;

extern int nerrors;
extern int ntmp;
extern NODE *saved[];
extern long maxoff;
extern int vafe;
extern int aborted;
extern long baseoff;
extern int dope[];

void dotree(), dotree2(), cutree(), passes(), abortpass(), prtbl();

int main(int argc, char **argv)
{
	register int c;
	register char *q;
	register NODE *p;
	int lno;
	int v1, v2;
	int i;
	struct qa s1, s2, s3;
	struct qa *p1, *p2, *p3;
	char ch;

	lno = 1;
	p1 = &s1;
	p2 = &s2;
	p3 = &s3;
	initopts(argc, argv);
	tinit();
	for (;;) {
		c = getc(infile);
		if (c <= 0)
			break;
		switch (c) {
		case '{':
			hascall = 0;
			aborted = 0;
			qtail = 0;
			nconst = 0;
			nfconst = 0;
			qfree = 0;
			qtail = 0;
			nexttmp = 1;
			tinit();
			nlabrm = 0;
			njmprm = 0;
			ntl = 0;
			ndeadrm = 0;
			vafc = 0;
			lno = 1;
			maxtmp = 0;
			ctmpoff = 0;
			nctmp = 0;
			nctmpuses = 0;
			for (i = 0; i <= 1000; i++) {
				ctneg[i] = 0;
				ctpos[i] = 0;
			}
			mkhash();
			nlabsave = rdin(10);
			frame0 = rdin(10);
			maxoff = rdin(10);
			nparm = rdin(10);
			nlcheck();
			if (!rdsym())
				cerror("error in reading symbol table file");
			qhead = addquad(QSTART, 0L, 0L, 0L);
			break;
		case '}':
			addquad(QEND, 0L, 0L, 0L);
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
			if (!addquad(QEBLOCK, 0L, 0L, 0L))
				goto err;
			nlcheck();
			break;
		case '[':
			baseoff = rdin(10);
			v1 = rdin(10);
			v2 = rdin(10);
			if (vafc == 0)
				vafc = lno;
			p1->b2 = OALCON;
			p1->w4 = constlook(baseoff);
			p2->b2 = OAICON;
			p2->w4 = v1;
			p3->b2 = OAICON;
			p3->w4 = v2;
			p1->w0 = lno;
			lno++;
			if (!addquad(QBLOCK, p1, p2, p3))
				goto err;
			nlcheck();
			break;
		case '.':
			lineno = rdin(10);
			curpos.line = lineno;
			curpos.name = ftitle;
			q = ftitle;
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
				tabfree(1, (char *)qhead);
				qhead = (struct quad *)tabzalloc(0, 0, 1);
			}
			break;
		case 'L':
			p1->b2 = OAICON;
			p1->w4 = rdin(10);
			if (!addquad(QLABEL, p1, 0L, 0L))
				goto err;
			nlcheck();
			break;
		case 'B':
			p1->b2 = OAICON;
			p1->w4 = rdin(10);
			if (!addquad(QJP, p1, 0L, 0L))
				goto err;
			nlcheck();
			ifpos = ftell(infile);
			break;
		case 'C':
			while ((ch = getc(infile)) == ' ')
				continue;
			ungetc(ch, infile);
			p1->b2 = OAICON;
			p1->w4 = rdin(10);
			if (!addquad(QCTABLE, p1, 0L, 0L))
				goto err;
			while ((ch = getc(infile)) != '\n') {
				ungetc(ch, infile);
				p1->b2 = OAICON;
				p1->w4 = rdin(10);
				if (!addquad(QCASEL, p1, 0L, 0L))
					goto err;
			}
			if (!addquad(QCEND, 0L, 0L, 0L))
				goto err;
			ifpos = ftell(infile);
			break;
		case '/':
			putc(c, stdout);
			while (putc(getc(infile), stdout) != (('/' << 8) | 'n'))	/* the original: '/n' (a two-character constant: never equal) */
				;
			break;
		default:
			cerror("intermediate file format error - %c", c);
		}
	}
	return nerrors;
}

/* a tree of the intermediate file: a function qhead (PROC) or a statement */
void dotree(NODE *p)
{
	if (p->in.op == PROC) {
		if (eflag)
			fwalk(p, pr, 0);
		fwalk(p, mapnm, 0);	/* the original passes no down: mapnm() does not use it */
		if (eflag)
			fwalk(p, pr, 0);
		curproc = p;
	} else if (p->in.op == UPROC) {
		if (eflag)
			fwalk(p, pr, 0);
		mapnm(p->in.left);
		curproc = p;
	} else {
		mapall(p);
		dotree2(p);
	}
}

void dotree2(NODE *p)
{
	register int i;

	ntmp = 0;
	if (eflag)
		fwalk(p, pr, 0);
	fwalk(p, mapnm, 0);	/* the original passes no down: mapnm() does not use it */
	cutree(p);
	genquads(p);
	p->in.op = FREE;
	for (i = 0; i < ntmp; i++) {
		genquads(saved[i]);
		saved[i]->in.op = FREE;
	}
}

static void cutree(NODE *p)
{
	register int op, kind;
	register NODE *q;

	op = p->in.op;
	kind = optype(op);
	switch (op) {
	case CALL: case UCALL: case STCALL: case USTCALL:
		hascall = 1;
		return;
	case NOT: case CBRANCH:
		return;
	case INCR: case DECR:
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
void pr(NODE *p, int down, int *down1, int *down2)
{
	*down1 = *down2 = down + 1;
	for (; down >= 2; down -= 2)
		putchar('\t');
	if (down--)
		printf("    ");
	printf("%%(8)%o) %s", (unsigned)(uintptr_t)(p), opst[p->in.op]);
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

static void abortpass(void)
{
	aborted = 1;
	lastq->next = 0;
	passes();
}

static void passes(void)
{
	if (aborted == 0) {
		aliasm();
		if (fdebug > 4) {
			printf("// quads before aliasm");
			prquads();
		}
		if (aliasmat() && fdebug > 4)
			prquads();
		if (flow_graph()) {
			betainit();
			if (olevel > 1)
				ebbopt();
			if (olevel >= 3 && domloops() == 0) {
			}
			if (olevel == 3 || olevel > 4)
				optimize();
		}
	}
	if (tflag == 0)
		writeil();
	if (sdebug)
		prtbl();
}

static void prtbl(void)
{
	int i;

	for (i = 0; i < 11; i++)
		printf("// table no. = %d, space used = %d\n", i, (int)(arhigh[i] - tabbase(i)));
}
