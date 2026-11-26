static char sccsid[] = "@[$]pstat.c	3.2  11/18/83 16:28:14 - 87wega3.2";
/*
 * Print system stuff
 */

#include <stdio.h>
#include <nlist.h>
#include <sys/param.h>
#include <sys/conf.h>
#include <sys/tty.h>
#include <sys/inode.h>
#include <sys/text.h>
#include <sys/proc.h>
#include <sys/dir.h>
#include <sys/s.out.h>
#include <sys/mmu.h>
#include <sys/user.h>
#include <sys/file.h>

char	*fcore	= "/dev/kmem";
char	*fnlist	= "/wega";

struct nlist setup[] = {
#define	SINODE	0
	{ "_inode" },
#define	SNINODE	1
	{ "_Ninode" },
#define	STEXT	2
	{ "_text" },
#define	SNTEXT	3
	{ "_Ntext" },
#define	SPROC	4
	{ "_proc" },
#define	SNPROC	5
	{ "_Nproc" },
#define	SSIO	6
	{ "_sio" },
#define	SNSIO	7
	{ "_nsiochan" },
#define	SFIL	8
	{ "_file" },
#define	SNFIL	9
	{ "_Nfile" },
	{ "" },
};

#define	SEEK(a)	if (kflg) kvseek(fc, (a)); else lseek(fc, (a), 0)

char	*myname;
int	fc;
int	inof;
int	txtf;
int	prcf;
int	ttyf;
int	usrf;
long	ubase;
int	kflg;
int	filf;
int	allflg;

char	*calloc();
long	lseek();

main(argc, argv)
char **argv;
{
	int i;

	myname = argv[0];
	if (!(argc-1)) {
		printf("usage: pstat -[a]p or pstat -[ixt] [corefl]\n");
		exit(1);
	}
	while (--argc && **++argv == '-') {
		while (*++*argv)
		switch (**argv) {

		case 'a':
			allflg++;
			break;

		case 'i':
			inof++;
			break;

		case 'x':
			txtf++;
			break;
		case 'p':
			prcf++;
			break;

		case 't':
			ttyf++;
			break;

		case 'u':
			if (--argc == 0)
				break;
			usrf++;
			ubase = oatoi(*++argv);
			fcore = "/dev/mem";
			break;

		case 'f':
			filf++;
			break;
		}
	}
	if (argc>0) {
		fcore = argv[0];
		kflg++;
	}
	if ((fc = open(fcore, 0)) < 0) {
		printf("Can't find %s\n", fcore);
		exit(1);
	}
	if (argc>1)
		fnlist = argv[1];
	nlist(fnlist, setup);
	if (setup[SINODE].nl_type == -1) {
		printf("no namelist\n");
		exit(1);
	}
	if (inof)
		doinode();
	if (txtf)
		dotext();
	if (ttyf)
		dotty();
	if (prcf)
		doproc();
	if (usrf)
		dousr();
	if (filf)
		dofil();
}

long
swapb(l)
long l;
{
	union {
		long	l;
		char	c[4];
	} u;
	char c;

	u.l = l;
	c = u.c[0];
	u.c[0] = u.c[1];
	u.c[1] = c;
	printf("-> %X <-", u.l);
	return(u.l);
}

getnum(i)
register i;
{
	register long a;
	int n;
	long x;

	if (a = setup[i].nl_value) {
		SEEK(a);
		if (read(fc, (char *)&n, sizeof(n)) != sizeof(n))
			return(0);
		return(n);
	}
	return(0);
}

doinode()
{
	register struct inode *ip;
	struct inode *xinode;
	register int nin, ninode;
	register long loc;

	nin = 0;
	if ((ninode = getnum(SNINODE)) == 0) {
		fprintf(stderr, "%s: cannot determine size of inode table.\n", myname);
		exit(1);
	}
	if ((xinode = (struct inode *)calloc(ninode, sizeof(struct inode))) == NULL) {
		fprintf(stderr, "%s: calloc: no memory for inode table.\n", myname);
		exit(1);
	}
	xinode = (struct inode *)calloc(ninode, sizeof(struct inode));
	loc = setup[SINODE].nl_value;
	SEEK(loc);
	read(fc, (char *)xinode, ninode * sizeof(struct inode));
	for (ip = xinode; ip < &xinode[ninode]; ip++)
		if (ip->i_count)
			nin++;
	printf("%d active inodes\n", nin);
	printf("       LOC  FLAGS  CNT   DEVICE   INO   MODE   NLK  UID  SIZE/DEV\n");
	loc = setup[SINODE].nl_value;
	for (ip = xinode; ip < &xinode[ninode]; ip++, loc += sizeof(xinode[0])) {
		if (ip->i_count == 0)
			continue;
		printf("%10lx ", loc);
		putf(ip->i_flag&ILOCK, 'L');
		putf(ip->i_flag&IUPD, 'U');
		putf(ip->i_flag&IACC, 'A');
		putf(ip->i_flag&IMOUNT, 'M');
		putf(ip->i_flag&IWANT, 'W');
		putf(ip->i_flag&ITEXT, 'T');
		printf("%5d", ip->i_count&0377);
		printf("  %3d,%3d", major(ip->i_dev), minor(ip->i_dev));
		printf("%6u", ip->i_number);
		printf("%7o", ip->i_mode);
		printf("%6d", ip->i_nlink);
		printf("%5u", ip->i_uid);
		if ((ip->i_mode&IFMT)==IFBLK || (ip->i_mode&IFMT)==IFCHR)
			printf("%6d,%3d", major(ip->i_rdev), minor(ip->i_rdev));
		else
			printf("%10ld", ip->i_size);
		printf("\n");
	}
	free(xinode);
}

putf(v, n)
char n;
{
	if (v)
		printf("%c", n);
	else
		printf(" ");
}

dotext()
{
	register struct text *xp;
	register long loc;
	register int ntext;
	struct text *xtext;
	int ntx;

	ntx = 0;
	if ((ntext = getnum(SNTEXT)) == 0) {
		fprintf(stderr, "%s: cannot determine size of text table.\n", myname);
		exit(1);
	}
	if ((xtext = (struct text *)calloc(ntext, sizeof(struct text))) == NULL) {
		fprintf(stderr, "%s: calloc: no memory for text table.\n", myname);
		exit(1);
	}
	loc = setup[STEXT].nl_value;
	SEEK(loc);
	read(fc, (char *)xtext, ntext * sizeof(struct text));
	for (xp = xtext; xp < &xtext[ntext]; xp++)
		if (xp->x_iptr!=NULL)
			ntx++;
	printf("%d text segments\n", ntx);
	printf("       LOC FLAGS DADDR   CADDR  SIZE    IPTR  CNT CCNT\n");
	for (xp = xtext; xp < &xtext[ntext]; xp++, loc+=sizeof(xtext[0])) {
		if (xp->x_iptr == NULL)
			continue;
		printf("%10lx", loc);
		printf(" ");
		putf(xp->x_flag&XTRC, 'T');
		putf(xp->x_flag&XWRIT, 'W');
		putf(xp->x_flag&XLOAD, 'L');
		putf(xp->x_flag&XLOCK, 'K');
		putf(xp->x_flag&XWANT, 'w');
		printf("%6u", xp->x_daddr);
		printf("%8lx", xp->x_caddr);
		printf("%6d", xp->x_size);
		printf("%8lx", xp->x_iptr);
		printf("%5d", xp->x_count&0377);
		printf("%5d", xp->x_ccount);
		printf("\n");
	}
	free(xtext);
}

doproc()
{
	register struct proc *pp;
	register long loc;
	register int nproc, np;
	struct proc *xproc;

	np = 0;
	if ((nproc = getnum(SNPROC)) == 0) {
		fprintf(stderr, "%s: cannot determine size of proc table.\n", myname);
		exit(1);
	}
	if ((xproc = (struct proc *)calloc(nproc, sizeof(struct proc))) == NULL) {
		fprintf(stderr, "%s: calloc: no memory for proc table.\n", myname);
		exit(1);
	}
	loc = setup[SPROC].nl_value;
	SEEK(loc);
	read(fc, (char *)xproc, nproc * sizeof(struct proc));
	for (pp=xproc; pp < &xproc[nproc]; pp++)
		if (pp->p_stat)
			np++;
	printf("%d processes\n", np);
	printf("     LOC S  F  PR SG UID TIM CPU NI  PGRP   PID  PPID ADDR SZE     WCHN    LINK   TEXTP  CLKT\n");
	for (loc=setup[SPROC].nl_value,pp=xproc; pp<&xproc[nproc]; pp++,loc+=sizeof(xproc[0])) {
		if (pp->p_stat==0 && allflg==0)
			continue;
		printf("%8x", loc);
		printf("%2d", (unsigned char)pp->p_stat);
		printf("%3x", pp->p_flag);
		printf("%4d", pp->p_pri);
		printf("%3d", pp->p_sig);
		printf("%4d", pp->p_uid&0377);
		printf("%4d", pp->p_time&0377);
		printf("%4d", pp->p_cpu&0377);
		printf("%3d", pp->p_nice);
		printf("%6d", pp->p_pgrp);
		printf("%6d", pp->p_pid);
		printf("%6d", pp->p_ppid);
		printf("%5x", pp->p_addr);
		printf("%4x", pp->p_size);
		printf("%9X", pp->p_wchan);
		printf("%8X", pp->p_link);
		printf("%8X", pp->p_textp);
		printf("%6u", pp->p_clktim);
		printf("\n");
	}
	free(xproc);
}

dotty()
{
	register int nsio;
	register struct tty *tp;
	register int i;
	register char *mesg;
	struct tty *xsio;
	int x;
	long loc;

	if ((nsio = getnum(SNSIO)) == 0) {
		fprintf("%s: cannot determine the number of sio ports.\n", myname);
		exit(1);
	}
	xsio = (struct tty *)calloc(nsio, sizeof(struct tty));
	loc = setup[SSIO].nl_value;
	SEEK(loc);
	read(fc, (char *)xsio, nsio * sizeof(struct tty));
	printf("%d sio lines\n", nsio);
	printf(" # RAW CAN OUT  IMODE   OMODE   CMODE  LDMODE  DEL COL  STATE  PGRP\n");
	for (tp = xsio, i = 0; tp < &xsio[nsio]; tp++, i++) {
		printf("%2d", i);
		printf("%4d", tp->t_rawq.c_cc);
		printf("%4d", tp->t_canq.c_cc);
		printf("%4d", tp->t_outq.c_cc);
		printf("%7o ", tp->t_iflag);
		printf("%7o ", tp->t_oflag);
		printf("%7o ", tp->t_cflag);
		printf("%7o ", tp->t_lflag);
		printf("%4d", (unsigned char)tp->t_delct);
		printf("%4u ", (unsigned char)tp->t_col);
		putf((int)(tp->t_state&TIMEOUT), 'T');
		putf((int)(tp->t_state&WOPEN), 'W');
		putf((int)(tp->t_state&ISOPEN), 'O');
		putf((int)(tp->t_state&CARR_ON), 'C');
		putf((int)(tp->t_state&BUSY), 'B');
		putf((int)(tp->t_state&TACT), 'H');
		printf("%6d", tp->t_pgrp);
		printf("\n");
	}
	free(xsio);
}

dousr()
{
	union {
		struct	user rxu;
		char	fxu[ctob(USIZE)];
	} xu;
	register struct user *up;
	register i;
	register long addr;

	addr = ubase<<8;
	if (kflg && addr)
		kpseek(fc, addr);
	else
		lseek(fc, addr, 0);
	lseek(fc, ubase<<8, 0);
	read(fc, (char *)&xu, sizeof(xu));
	up = &xu.rxu;
	printf("rsav ");
	for (i=0; i<9; i++)
		printf("%x ", up->u_rsav[i]);
	printf("\nsegflg, error %d, %d\n", up->u_segflg, up->u_error);
	printf("uids %d,%d,%d,%d\n", up->u_uid,up->u_gid,up->u_ruid,up->u_rgid);
	printf("procp %.1x\n", up->u_procp);
	printf("base %x , count %x , offset %ld\n", up->u_base,
		up->u_count, up->u_offset);
	printf("cdir %x\n", up->u_cdir);
	printf("dbuf %s\n", up->u_dbuf);
	printf("dirp %x\n", up->u_dirp);
	printf("dent %d %s\n", up->u_dent.d_ino, up->u_dent.d_name);
	printf("pdir %x\n", up->u_pdir);
	printf("\tbase\tlimit\tattr\n");
	printf("tsegd\t%x\t%x\t%x\n", up->u_nsucode.sg_base, up->u_nsucode.sg_limit, up->u_nsucode.sg_attr);
	printf("dsegd\t%x\t%x\t%x\n", up->u_segmts[63].sg_base, up->u_segmts[63].sg_limit, up->u_segmts[63].sg_attr);
	printf("ssegd\t%x\t%x\t%x\n", up->u_segmts[127].sg_base, up->u_segmts[127].sg_limit, up->u_segmts[127].sg_attr);
	printf("\nfiles");
	for (i=0; i<NOFILE; i++)
		if (up->u_ofile[i])
			printf("  (%d)%4x", i, up->u_ofile[i]);
	printf("\n    ");
	printf("\nargs");
	for (i=0; i<5; i++)
		printf(" %6x", up->u_arg[i]);
	printf("\nsizes %x %x %x\n", up->u_tsize, up->u_dsize, up->u_ssize);
	printf("sep %d\n", (unsigned char)up->u_sep);
	printf("qsav ");
	for (i=0; i<9; i++)
		printf("%x ", up->u_qsav[i]);
	printf("\nssav ");
	for (i=0; i<9; i++)
		printf("%x ", up->u_ssav[i]);
	printf("\nsigs");
	for (i=0; i<NSIG; i++)
		printf(" %.1x", up->u_signal[i]);
	printf("\ntimes %ld %ld\n", up->u_utime/60, up->u_stime/60);
	printf("ctimes %ld %ld\n", up->u_cutime/60, up->u_cstime/60);
	printf("state %.1x\n", up->u_state);
	printf("prof base %x size %u offset %u scal %u\n", up->u_prof[0].pr_base, up->u_prof[0].pr_size,
		up->u_prof[0].pr_off, up->u_prof[0].pr_scale);
	printf("intflg %d\n", (unsigned char)up->u_intflg);
	printf("ttyp %.1x\n", up->u_ttyp);
	printf("ttydev %d,%d\n", major(up->u_ttyd), minor(up->u_ttyd));
	printf("comm %s\n", up->u_comm);
}

oatoi(s)
char *s;
{
	register v;

	v = 0;
	while (*s) {
		v = (v<<4) + *s - (*s>='a' && *s<='f' ? 'a'-10 : (*s>='A' && *s<='F' ? 'A'-10 : '0'));
		s++;
	}
	return(v);
}

dofil()
{
	register struct file *fp;
	struct file *xfile;
	register nf, nfile;
	register long loc;

	nf = 0;
	if ((nfile = getnum(SNFIL)) == 0) {
		fprintf(stderr, "%s: cannot determine size of file table.\n", myname);
		exit(1);
	}
	if ((xfile = (struct file *)calloc(nfile, sizeof(struct file))) == NULL) {
		fprintf(stderr, "%s: calloc: no memory for file table.\n", myname);
		exit(1);
	}
	loc = setup[SFIL].nl_value;
	SEEK(loc);
	read(fc, (char *)xfile, nfile * sizeof(struct file));
	for (fp=xfile; fp < &xfile[nfile]; fp++)
		if (fp->f_count)
			nf++;
	printf("%d open files\n", nf);
	printf("  LOC   FLG CNT   IPTR   OFFS\n");
	for (fp=xfile,loc=setup[SFIL].nl_value; fp < &xfile[nfile]; fp++,loc+=sizeof(xfile[0])) {
		if (fp->f_count==0)
			continue;
		printf("%5x    ", loc);
		putf(fp->f_flag&FREAD, 'R');
		putf(fp->f_flag&FWRITE, 'W');
		putf(fp->f_flag&FPIPE, 'P');
		printf("%2d", fp->f_count&0377);
		printf("%8x", fp->f_inode);
		printf("   %ld\n", fp->f_un.f_off);
	}
	free(xfile);
}
