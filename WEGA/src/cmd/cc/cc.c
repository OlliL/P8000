/*
 * cc, scc - C compiler drivers (WEGA)
 *
 * Reconstructed from the original WEGA binaries /bin/cc and /bin/scc. The object
 * code produced by the original cc from this file matches the original binaries
 * (all functions, the data and the strings), so statement structure, declaration
 * order and the odd constructs follow the original. scc is the same source
 * compiled with -DSCC: it runs the segmented passes (scparse, scodgen, spopt,
 * sld, slibc.a, scrt0.o, ...), passes "-z" to the loader, "-Z" to gopt and
 * "-DNULL=0L" to cpp. Both are linked as non-segmented programs.
 *
 * Passes: cpp > cparse > [gopt] > codgen > [fpf] > [popt] > cas > ld, with temporary
 * files /tmp/cc.<pid>.<x>. Options: -c -S -Sl -E -P -O[0-5lr] -p -F -o file -D -I -U -C
 * -N{s,l} -a<ids> -B<dir> -T<x><path> -:<x><flags> (-:v -:d -:z -:n -:r), all others
 * go to ld.
 *
 * Things that look wrong but are what the original does:
 *  - "-o" looks at argv[i + 1] with an i that was never set (a register variable that
 *    still holds 0 at this point of main)
 *  - an error in a "-:" option falls through into the "-N" case
 *  - cutsuf() does not set r when the name has no '.'
 *  - several functions and variables are never used (closeout, nonempty, cantopen,
 *    any, fatal1, cbuf, xflags, ...); they are kept because they are part of the
 *    binary. error1/error2 and fatal1/fatal2 are two identical pairs of functions.
 *
 * The libc part of the original binaries (printf, fprintf, sprintf, _doprnt, malloc
 * and the string functions) comes from an older libc with floating point support
 * that is not available here, so a rebuilt cc is 112 bytes longer; everything that
 * comes from this file is identical (see host/Makefile, target verify-wega).
 */
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

/* signal.h declares signal() with a pointer type; the original compares the plain int */
#define SIGHUP	1
#define SIGINT	2
#define SIGQUIT	3
#define SIGTERM	15
#define SIG_IGN	1

extern int errno;
char *strcpy(), *strcat(), *calloc();
int strlen();

#ifdef SCC
#define SEG(c, s)	s
#else
#define SEG(c, s)	c
#endif

char	*libs[4] = { SEG("/lib/libc.a", "/lib/slibc.a") };
FILE	*diag = stderr;
static int	intign, quitign, hupign, termign;
static int	unused1, nfiles;
char	*fname = "basic";
char	*cpp		= "/lib/cpp";
#ifdef OLD_V0P
/* ccv0p (v0p/Makefile): the passes of the old compiler v0p are in /lib/v0p */
char	*cparse		= "/lib/v0p/cparse";
char	*gopt		= "/lib/gopt";
char	*codgen		= "/lib/v0p/codgen";
char	*popt		= "/lib/v0p/popt";
#else
#ifdef OLD_V0
/* ccv0 (v0/Makefile): the passes of the old compiler v0 are in /lib/v0 */
char	*cparse		= "/lib/v0/cparse";
char	*gopt		= "/lib/gopt";
char	*codgen		= "/lib/v0/codgen";
char	*popt		= "/lib/v0/popt";
#else
#ifdef OLD_V1
/* ccv1 (v1/Makefile): the passes of the old compiler v1 are in /lib/v1 */
char	*cparse		= "/lib/v1/cparse";
char	*gopt		= "/lib/gopt";
char	*codgen		= "/lib/v1/codgen";
char	*popt		= "/lib/v1/popt";
#else
char	*cparse		= SEG("/lib/cparse", "/lib/scparse");
char	*gopt		= "/lib/gopt";
char	*codgen		= SEG("/lib/codgen", "/lib/scodgen");
char	*popt		= SEG("/lib/popt", "/lib/spopt");
#endif
#endif
#endif
char	*clist		= "/lib/clist";
char	*cas		= "/bin/cas";
char	*ld		= SEG("/bin/ld", "/bin/sld");
char	*fpf		= "/z/bin/lib/fpf";
char	*fpe		= SEG("/z/bin/lib/fpe.o", "/z/bin/lib/sfpe.o");
char	*fpent		= SEG("/z/bin/lib/fpent.o", "/z/bin/lib/sfpent.o");
char	*libcfpf	= SEG("/z/bin/lib/libcfpf.a", "/z/bin/lib/slibcfpf.a");
char	*crt0		= SEG("/lib/crt0.o", "/lib/scrt0.o");
char	*mcrt0		= SEG("/lib/mcrt0.o", "/lib/smcrt0.o");
char	*m4		= "/usr/bin/m4";
char	*basicc		= SEG("/z/bin/basic", "/z/bin/sbasic");
char	*f77		= SEG("/z/bin/f77", "/z/bin/sf77");
char	*shell		= "/bin/sh";
char	*outfile	= "a.out";
char	*cfile		= "<command line>";

char	*cppflags	= " ";
char	*cparseflags	= " ";
char	*goptflags	= " ";
char	*codgenflags	= " ";
char	*poptflags	= " ";
char	*asflags	= " ";
char	*xflags	= " ";
char	*ldflags	= " ";
char	*f77flags	= " ";
char	*basicflags	= " ";
char	optlev		= '0';

static char	cparsebuf[50];
static char	f77buf[20] = "-";
static char	cbuf[20] = "-c";
static char	xopt[50] = "-x";
#ifdef SCC
static char	gbuf[4] = "-Z";
#else
static char	gbuf[4];
#endif
static char	Bbuf[50];
static char	abuf[10] = "p1O2o";
extern char	cppbuf[];
char	*cppp		= cppbuf;
char	*cparsep	= cparsebuf;
char	*f77p		= f77buf + 1;
char	*cbufp		= cbuf + 2;
char	*Bdir		= Bbuf;
char	*apass		= abuf;

static int	errflag;
static int	lflag;
static int	loadflag = 1;
static int	Sflag;
static int	cflag;
static int	pflag;
static int	oflag;
static int	dflag;
static int	zflag;
static int	nflag;
static int	rflag;
static int	Pflag;
static int	Eflag;
static int	vflag;
static int	unused2, unused3;
static int	Fflag;

int	pid;
char	cmd[200];
char	curpass[200];
char	fbuf[20];
char	t_I[26], t_D[26], t_s[26], t_p[26], t_o[26], t_z[26], t_S[26];
char	cppbuf[512];
char	*obj1;
char	**av;
char	**clp;

int	idexit();
char	*alloc(), *savestr(), *cat(), *basename(), *lastchar(), *setsuf(), *cutsuf();

main(argc, argv)
int	argc;
char	**argv;
{
	register int i;
	register int c;
	register char *t;
	register char *s;

	intign = signal(SIGINT, SIG_IGN) & 01;
	quitign = signal(SIGQUIT, SIG_IGN) & 01;
	hupign = signal(SIGHUP, SIG_IGN) & 01;
	termign = signal(SIGTERM, SIG_IGN) & 01;
	setsigs(idexit);
	av = (char **)alloc((argc + 20) * 2);
	av[1] = "-X";
	clp = av + 2;
	*clp++ = "-e";
	*clp++ = "start";
#ifdef SCC
	*clp++ = "-z";
#endif
	while (argc--, argv++, argc > 0 && argv[0][0] == '-' && argv[0][1]) {
		for (t = argv[0] + 1; *t; t++)
			switch (*t) {
			case 'B':
				if (t[1])
					Bdir = t + 1;
				else
					Bdir = "/copt/passes/";
				goto next;
			case 'a':
				if (t[1])
					apass = t + 1;
				else
					error1("Bad option '-a' must be followed by path identifiers, -ap1O2o assumed.");
				goto next;
			case 'T':
				t++;
				switch (*t) {
				case 'p':
					cpp = t + 1;
					goto next;
				case '1':
					cparse = t + 1;
					goto next;
				case 'O':
					gopt = t + 1;
					goto next;
				case '2':
					codgen = t + 1;
					goto next;
				case 'o':
					popt = t + 1;
					goto next;
				case 'a':
					cas = t + 1;
					goto next;
				case 'l':
					ld = t + 1;
					goto next;
				case 'b':
					basicc = t + 1;
					goto next;
				case 'f':
					f77 = t + 1;
					goto next;
				case 'F':
					crt0 = t + 1;
					goto next;
				case 'P':
					fpf = t + 1;
					goto next;
				default:
					error1("Bad option '-T%c'.", *t);
				}
				break;
			case ':':
				t++;
				switch (*t) {
				case 'p':
					cppflags = t + 1;
					goto next;
				case '1':
					cparseflags = t + 1;
					goto next;
				case 'O':
					goptflags = t + 1;
					goto next;
				case '2':
					codgenflags = t + 1;
					goto next;
				case 'o':
					poptflags = t + 1;
					goto next;
				case 'a':
					asflags = t + 1;
					goto next;
				case 'l':
					ldflags = t + 1;
					goto next;
				case 'f':
					f77flags = t + 1;
					goto next;
				case 'b':
					basicflags = t + 1;
					goto next;
				case 'v':
					vflag = 1;
					goto next;
				case 'd':
					dflag = 1;
				case 'z':
					zflag = 1;
					goto next;
				case 'r':
					rflag = 1;
					goto next;
				case 'n':
					nflag = 1;
					goto next;
				default:
					error1("Bad option '-:%c'.", *t);
				}
			case 'N':
				if (t[1] == 's') {
					*clp++ = "-Ns";
					*cparsep++ = '-';
					*cparsep++ = 'L';
					*cparsep++ = ' ';
					goto next;
				}
				if (t[1] == 'l')
					goto next;
				error1("Bad option '-N%c'.", t[1]);
				goto next;
			case 'O':
				if (t[1] == 0) {
					oflag = 1;
					goto next;
				}
				if (isdigit(t[1]) || t[1] == 'l' || t[1] == 'r') {
					oflag = 1;
					t++;
					if (isdigit(*t))
						optlev = *t;
					else if (*t == 'l')
						optlev = '4';
					else if (*t == 'r')
						optlev = '5';
				}
				if (t[1] == 'p')
					oflag = 1;
				goto next;
			case 'F':
				Fflag = 1;
				libs[0] = fpe;
				libs[1] = fpent;
				libs[2] = libcfpf;
				break;
			case 'S':
				Sflag = 1;
				if (t[1] == 'l') {
					lflag = 1;
					t++;
				}
				break;
			case 'P':
				Pflag = 1;
				loadflag = 0;
				*cppp++ = '-';
				*cppp++ = 'P';
				*cppp++ = ' ';
				break;
			case 'E':
				Pflag = 1;
				loadflag = 0;
				Eflag = 1;
				*cppp++ = '-';
				*cppp++ = 'E';
				*cppp++ = ' ';
				break;
			case 'C':
				*cppp++ = '-';
				*cppp++ = 'C';
				*cppp++ = ' ';
				break;
			case 'D':
			case 'I':
			case 'U':
				*cppp++ = '-';
				do
					*cppp++ = *t;
				while (*++t);
				*cppp++ = ' ';
				goto next;
			case 'c':
				loadflag = 0;
				cflag = 1;
				break;
			case 'p':
				pflag = 1;
				*cparsep++ = '-';
				*cparsep++ = 'p';
				*cparsep++ = ' ';
				*f77p++ = 'p';
				break;
			case 'o':
				if ((char)getsuf(argv[i + 1]) == 'c') {
					fprintf(diag, "would overwrite %s\n", argv[i + 1]);
					exit(1);
				}
				argv++;
				outfile = argv[0];
				argc--;
				break;
			case 'l':
				t[-1] = '-';
				*clp++ = t - 1;
				goto next;
			default:
				xopt[1] = *t;
				*clp++ = savestr(xopt);
				break;
			}
	next:	;
	}
	pid = getpid();
	mktmps();
	*cppp = 0;
	*cparsep = 0;
	if (*Bdir)
		while (*apass)
			switch (*apass++) {
			case 'p':
				cpp = cat(Bdir, basename(cpp));
				break;
			case '1':
				cparse = cat(Bdir, basename(cparse));
				break;
			case 'O':
				gopt = cat(Bdir, basename(gopt));
				break;
			case '2':
				codgen = cat(Bdir, basename(codgen));
				break;
			case 'o':
				popt = cat(Bdir, basename(popt));
				break;
			case 'a':
				cas = cat(Bdir, basename(cas));
				break;
			case 'l':
				ld = cat(Bdir, basename(ld));
				break;
			case 'b':
				basicc = cat(Bdir, basename(basicc));
				break;
			case 'f':
				f77 = cat(Bdir, basename(f77));
				break;
			case 'F':
				crt0 = cat(Bdir, basename(crt0));
				break;
			default:
				error1("Bad option '-a%c'.", apass[-1]);
			}
	av[0] = ld;
	*clp++ = pflag ? mcrt0 : crt0;
	for (i = 0; i < argc; i++) {
		switch (c = getsuf(cfile = argv[i])) {
		case 'b':
		case 'c':
		case 'f':
		case 'p':
		case 's':
			nfiles++;
		case 'o':
			break;
		}
	}
	if (nfiles > 1)
		cflag = 1;
	for (i = 0; i < argc; i++) {
		switch (c = getsuf(cfile = argv[i])) {
		case 'c':
			if (nfiles > 1)
				fprintf(diag, "%s:\n", argv[i]);
			else if (nfiles == 1)
				obj1 = setsuf(savestr(argv[i]));
			if (testfile(argv[i]) != 0) {
				errflag = 1;
				break;
			}
			if (compile(argv[i]) != 0) {
				errflag = 1;
				break;
			}
			if (cflag) {
				if (nodup(s = setsuf(argv[i])))
					*clp++ = s;
			} else
				*clp++ = t_o;
			break;
		case 'f':
			if (testfile(argv[i]) != 0) {
				errflag = 1;
				break;
			}
			if (nfiles > 1)
				fprintf(diag, "%s:\n", argv[i]);
			else if (nfiles == 1)
				obj1 = setsuf(savestr(argv[i]));
			dbg("F77.");
			sprintf(cmd, "%s %s %s %s", f77, f77buf, f77flags, argv[i]);
			if (callsys(cmd) != 0) {
				errflag = 1;
				break;
			}
			if (cflag) {
				if (nodup(s = setsuf(argv[i])))
					*clp++ = s;
			} else
				*clp++ = t_o;
			break;
		case 's':
			if (Sflag)
				break;
			if (testfile(argv[i]) != 0) {
				errflag = 1;
				break;
			}
			if (cflag) {
				strcpy(t_o, basename(argv[i]));
				*lastchar(t_o) = 'o';
			}
			if (nfiles > 1)
				fprintf(diag, "%s:\n", argv[i]);
			dbg("ASM.");
			sprintf(cmd, "%s %s -u -o %s %s", cas, asflags, t_o, argv[i]);
			if (callsys(cmd) != 0) {
				errflag = 1;
				break;
			}
			if (nfiles == 1 && !cflag)
				obj1 = setsuf(argv[i]);
			if (cflag) {
				if (nodup(s = setsuf(argv[i])))
					*clp++ = s;
			} else
				*clp++ = t_o;
			break;
		case 'o':
			if (nodup(argv[i]))
				*clp++ = argv[i];
			break;
		case 'b':
		case 'p':
			break;
		default:
			if (strcmp(argv[i], "-o") == 0)
				i++, outfile = argv[i];
			else
				*clp++ = argv[i];
			break;
		}
	}
	if (loadflag && !errflag && !Sflag)
		load(av, clp);
	dexit(errflag);
}

/* preprocess, parse, optimize, generate and assemble one C file; returns 0, 1 (error) or 2 (compiler error) */
compile(f)
char	*f;
{
	register int i;
	int x;

	cfile = f;
	if (cflag || Sflag) {
		strcpy(t_o, basename(cfile));
		*lastchar(t_o) = 'o';
	}
	fname = basename(cfile);
	i = 0;
	while (*fname != '.')
		fbuf[i++] = *fname++;
	fbuf[i++] = 0;
	fname = fbuf;
	if (Eflag)
		t_p[0] = 0;
	else if (Pflag) {
		strcpy(t_p, basename(cfile));
		*lastchar(t_p) = 'i';
	}
	dbg("CPP.");
	sprintf(cmd, SEG("%s %s %s  %s %s", "%s -DNULL=0L %s %s  %s %s"), cpp, cppflags, cppbuf, cfile, t_p);
	if (callsys(cmd) != 0)
		goto err;
	if (Pflag)
		goto ok;
	dbg("CPARSE.");
	sprintf(cmd, "%s %s %s -E %s <%s", cparse, cparsebuf, cparseflags, cutsuf(t_I), t_p);
	switch (callsys(cmd)) {
	case 0:
		if (gopass() != 0)
			goto bad;
		asspass();
	ok:
		x = 0;
	done:
		return (x);
	case 1:
	err:
		error("Error.  No assembly.");
		x = 1;
		goto done;
	default:
	bad:
		fatal("Compiler error.");
		x = 2;
		goto done;
	}
}

/* global optimizer (-O1 ... -O5) and code generator */
gopass()
{
	static int gonce = 1;

	if (optlev != '0') {
		dbg("GOPT.");
		sprintf(cmd, "%s %s %s -%c -E %s >%s", gopt, gbuf, goptflags, optlev, cutsuf(t_I), t_z);
		if (access(gopt, 0) != 0)
			fprintf(diag, "Global optimizer not on system- no optimization performed\n");
		else if (callsys(cmd) != 0)
			fprintf(diag, "Global optimizer error - no optimization performed\n");
		else {
			dbg("MOVE.");
			sprintf(cmd, "/bin/mv %s %s", t_z, t_I);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
	}
	if (gonce && lflag)
		codgenflags = cat(codgenflags, " -l");
	if (gonce && pflag)
		codgenflags = cat(codgenflags, " -p");
	if (gonce && (optlev == '3' || optlev == '5'))
		codgenflags = cat(codgenflags, " -R");
	gonce = 0;
	dbg("CODGEN.");
	sprintf(cmd, "%s %s -E %s <%s >%s", codgen, codgenflags, cutsuf(t_D), t_I, t_s);
	return (callsys(cmd));
}

/* floating point filter, peephole optimizer, then assembler or listing (-S, -Sl) */
asspass()
{
	char	buf[26];

	if (Fflag) {
		dbg("FPF.");
		sprintf(cmd, "%s <%s >%s", fpf, t_s, t_z);
		if (access(fpf, 0) != 0)
			fprintf(diag, "Floating point filter not on system- no optimization performed\n");
		else if (callsys(cmd) != 0)
			fprintf(diag, "Floating point filter error - no optimization performed\n");
		else {
			dbg("MOVE.");
			sprintf(cmd, "/bin/mv %s %s", t_z, t_s);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
	}
	if (oflag) {
		dbg("POPT.");
		sprintf(cmd, "%s <%s >%s", popt, t_s, t_z);
		if (callsys(cmd) != 0)
			fprintf(diag, "Peephole optimizer error - no optimization performed\n");
		else {
			dbg("MOVE.");
			sprintf(cmd, "/bin/mv %s %s", t_z, t_s);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
	}
	if (Sflag) {
		*lastchar(t_o) = 's';
		if (lflag) {
			dbg("LIST.");
			sprintf(cmd, "%s %s %s", clist, t_s, t_o);
			if (callsys(cmd) != 0)
				fatal(" Cannot list file");
		} else {
			dbg("MOVE.");
			sprintf(cmd, "/bin/mv %s %s", t_s, t_o);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
	} else {
		strcpy(buf, t_s);
		dbg("ASM.");
		if (dflag)
			sprintf(cmd, "%s %s -u -v -o %s %s", cas, asflags, t_o, buf);
		else
			sprintf(cmd, "%s %s -u -o %s %s", cas, asflags, t_o, buf);
		if (callsys(cmd) != 0)
			fatal("assembler error");
	}
}

/* run the loader: libraries, "-o outfile" and the command line v up to e */
load(v, e)
register char **v, **e;
{
	register char **p;
	int	i;

	for (p = libs; *p; )
		*e++ = *p++;
	*e++ = "-o";
	*e++ = outfile;
	*e = 0;
	dbg("LOAD.");
	if (zflag) {
		for (p = v; p < e; p++)
			fprintf(diag, "%s ", *p);
		fprintf(diag, "\n");
	}
	if (nflag)
		return;
	i = fork();
	if (i == 0) {
		setsigs(0);
		execv(ld, v);
		fatal2("couldn't load %s", ld);
	}
	if (await(i) != 0) {
		errflag = 1;
		if (nfiles == 1 && !cflag) {
			dbg("MOVE.");
			sprintf(cmd, "/bin/mv %s %s", t_o, obj1);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
	}
}

/* run the command line s (words, "<in", ">out", ">>out") and return its exit code; -1 for an empty line */
callsys(s)
char	*s;
{
	char	*argv[100];
	int	app;
	int	i;
	register int c;
	register char *p;
	register char *in, *out;
	register int n;

	if (zflag)
		fprintf(diag, "%s\n", s);
	out = in = 0;
	argv[0] = shell;
	n = 1;
	p = s;
	while (isspace(*p))
		p++;
	while (*p) {
		if (*p == '<')
			in = p + 1;
		else if (*p == '>') {
			if (p[1] == '>') {
				app = 1;
				out = p + 2;
			} else {
				app = 0;
				out = p + 1;
			}
		} else
			argv[n++] = p;
		for (; !isspace(*p); p++)
			if (*p == 0)
				break;
		if (*p) {
			*p++ = 0;
			while (isspace(*p))
				p++;
		}
	}
	if (n == 1)
		return (-1);
	argv[n] = 0;
	if (nflag)
		return (0);
	strcpy(curpass, argv[1]);
	i = fork();
	if (i == 0) {
		if (in)
			freopen(in, "r", stdin);
		if (out)
			freopen(out, app ? "a" : "w", stdout);
		setsigs(0);
		doexec(argv[1], argv);
		fatal2("cannot load %s", argv[1]);
	}
	return (await(i));
}

/* exec the file; a file that is not a program goes to the shell */
doexec(file, argv)
char	*file;
char	**argv;
{
	execv(file, argv + 1);
	if (errno == ENOEXEC) {
		argv[1] = file;
		execv(shell, argv);
		fatal("No shell!");
	}
	if (errno == ENOMEM)
		fatal2("%s: too large", file);
}

dbg(s)
char	*s;
{
	if (vflag)
		fprintf(diag, "\r %s\t\t\r", s);
}

/* remove the temporary files and exit; -:r runs the result */
dexit(x)
int	x;
{
	static int dexited;

	if (!dexited) {
		dexited = 1;
		cleanup();
	}
	dbg("\t\t");
	if (rflag && !x)
		execl(outfile, outfile, 0);
	exit(x);
}

setsigs(f)
int	(*f)();
{
	if (!intign)
		signal(SIGINT, f);
	if (!quitign)
		signal(SIGQUIT, f);
	if (!hupign)
		signal(SIGHUP, f);
	if (!termign)
		signal(SIGTERM, f);
}

idexit()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	dexit(2);
}

/* wait for the child p and return its exit code */
await(p)
int	p;
{
	int	w, status;

	while ((w = wait(&status)) != p)
		if (w == -1)
			fatal("Bad wait code.");
	setsigs(idexit);
	if (status & 0377) {
		if (status == 0x8b)
			fatal2("Segmentation violation in %s", curpass);
		else if (status != 2)
			fatal3("Termination code : %o in %s", status, curpass);
		dexit(3);
	}
	return (status >> 8);
}

/* can the file be read? error exit if not */
testfile(f)
register char *f;
{
	register FILE *fp;

	fp = fopen(f, "r");
	if (fp) {
		fclose(fp);
		return (0);
	}
	error2("Cannot read file '%s'", f);
	return (1);
}

/* unused */
closeout(fpp)
FILE	**fpp;
{
	if (fpp && *fpp && *fpp != stdout) {
		if (ferror(*fpp))
			fatal("writing error");
		fclose(*fpp);
	}
	*fpp = 0;
}

cleanup()
{
	unl(t_I);
	unl(t_D);
	unl(t_s);
	if (!Pflag)
		unl(t_p);
	unl(t_z);
	unl(t_S);
	if (!cflag && !Sflag)
		unl(t_o);
}

/* unused: -1 if f cannot be examined, else whether it is not empty */
nonempty(f)
char	*f;
{
	struct stat buf;

	if (stat(f, &buf) < 0)
		return (-1);
	return (buf.st_size > 0);
}

mktmps()
{
	nameit(t_I, "I");
	nameit(t_D, "D");
	nameit(t_s, "s");
	nameit(t_o, "o");
	nameit(t_p, "p");
	nameit(t_z, "z");
	nameit(t_S, "S");
}

unl(f)
register char *f;
{
	if (!dflag && f && *f)
		unlink(f);
}

nameit(buf, suffix)
char	*buf, *suffix;
{
	if (!dflag)
		sprintf(buf, "/tmp/cc.%d.%s", pid, suffix);
	else
		sprintf(buf, "cc.%d.%s", pid, suffix);
}

getsuf(s)
register char *s;
{
	for (; *s; s++)
		if (*s == '.' && s[1] && !s[2])
			return (s[1]);
	return (0);
}

char *
basename(s)
register char *s;
{
	register char *p;

	p = s;
	for (; *s; s++)
		if (*s == '/')
			p = s + 1;
	return (p);
}

char *
lastchar(s)
register char *s;
{
	while (*s)
		s++;
	return (s - 1);
}

char *
setsuf(s)
register char *s;
{
	*lastchar(s) = 'o';
	return (basename(s));
}

/* unused */
cantopen(s)
char	*s;
{
	fatal2("cannot open intermediate file %s", s);
}

char *
alloc(n)
int	n;
{
	register char *p;
	int	i;

	p = calloc(1, n);
	if (p == 0)
		fatal("out of memory");
}

char *
savestr(s)
char	*s;
{
	return (strcpy(alloc(strlen(s) + 1), s));
}

char *
cutsuf(s)
char	*s;
{
	register char *p, *q, *r;

	p = strcpy(alloc(strlen(s) + 1), s);
	for (q = p; *q; q++)
		if (*q == '.')
			r = q;
	*r = 0;
	return (p);
}

char *
cat(a, b)
char	*a, *b;
{
	char	*p;

	p = strcpy(alloc(strlen(a) + strlen(b) + 1), a);
	return (strcat(p, b));
}

/* unused: is c in the string s */
any(c, s)
register int c;
register char *s;
{
	while (*s)
		if (c == *s++)
			return (1);
	return (0);
}

/* is the name not yet in the argument list for ld? */
nodup(l)
char	*l;
{
	register char **p;

	for (p = av; p < clp; p++)
		if (strcmp(*p, l) == 0)
			return (0);
	return (1);
}

error(s)
char	*s;
{
	fprintf(diag, "Error in file %s: %s\n", cfile, s);
	dexit(1);
	if (dflag)
		abort();
	exit(1);
}

error1(s, x)
char	*s, *x;
{
	char	buf[100];

	sprintf(buf, s, x);
	error(buf);
}

error2(s, x)
char	*s, *x;
{
	char	buf[100];

	sprintf(buf, s, x);
	error(buf);
}

fatal(s)
char	*s;
{
	fprintf(diag, "Compiler error in file %s: %s\n", cfile, s);
	dexit(1);
	if (dflag)
		abort();
	exit(1);
}

fatal1(s, x)
char	*s, *x;
{
	char	buf[100];

	sprintf(buf, s, x);
	fatal(buf);
}

fatal2(s, x)
char	*s, *x;
{
	char	buf[100];

	sprintf(buf, s, x);
	fatal(buf);
}

fatal3(s, x, y)
char	*s, *x, *y;
{
	char	buf[100];

	sprintf(buf, s, x, y);
	fatal(buf);
}
