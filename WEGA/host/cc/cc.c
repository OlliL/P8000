/*
 * cc, scc - C compiler drivers (WEGA 3.2)
 *
 * Reconstructed from the original WEGA binaries /bin/cc and /bin/scc by
 * decompilation (no other source was used).  The two drivers are the same
 * program: scc (compile with -DSCC) runs the segmented passes (scparse,
 * scodgen, spopt, sld, slibc.a, scrt0.o, ...), passes "-z" to the loader
 * and "-DNULL=0L" to cpp, and "-Z" to gopt; everything else is identical.
 * The driver runs the compiler passes
 * (cpp, cparse, gopt, codgen, fpf, popt, cas, ld) as separate processes
 * and passes temporary files between them.  The option letters, the
 * command lines it builds and its messages are those of the original.
 *
 * Options (all others are passed on to ld, one letter at a time):
 *   -c -S -Sl -E -P -O[0-5lr] -p -F -o file -D -I -U -C -N{s,l}
 *   -a<ids> -B<dir> -T<x><path> -:<x><flags>
 *   -:v verbose  -:d keep temporaries  -:z echo commands
 *   -:n do not execute  -:r run the result
 *
 * The pass programs are called by absolute path (/lib/cpp, ...) unless cc.conf
 * (scc.conf for scc), which lies next to the executable, names others (see cc.conf.example).
 * K&R C; build with cc -std=gnu89 -w or with the WEGA cc.
 */
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "conf.h"

#ifdef SCC
#define CONFNAME	"scc.conf"
#define SEG(c, s)	s
#else
#define CONFNAME	"cc.conf"
#define SEG(c, s)	c
#endif

typedef void (*sigfn)();

char	*libs[4];			/* libraries for ld, libs[0] set in main */
FILE	*diag;				/* stderr */
int	intign, quitign, hupign, termign;	/* signals ignored on entry */
int	nfiles;				/* number of source files */

/* pass programs */
char	*cpp		= "/lib/cpp";
char	*cparse		= SEG("/lib/cparse", "/lib/scparse");
char	*gopt		= "/lib/gopt";
char	*codgen		= SEG("/lib/codgen", "/lib/scodgen");
char	*popt		= SEG("/lib/popt", "/lib/spopt");
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
char	*mv		= "/bin/mv";
char	*tmpdir		= "/tmp";	/* directory of the temporary files */
char	*libc		= SEG("/lib/libc.a", "/lib/slibc.a");
char	*outfile	= "a.out";
char	*cfile		= "<command line>";

/* flag strings for the passes */
char	*cppflags	= " ";		/* -:p */
char	*cparseflags	= " ";		/* -:1 */
char	*goptflags	= " ";		/* -:O */
char	*codgenflags	= " ";		/* -:2 */
char	*poptflags	= " ";		/* -:o */
char	*asflags	= " ";		/* -:a */
char	*ldflags	= " ";		/* -:l */
char	*f77flags	= " ";		/* -:f */
char	*basicflags	= " ";		/* -:b */
char	optlev		= '0';		/* gopt level */

char	cparsebuf[256];			/* flags collected for cparse */
char	f77buf[32]	= "-";		/* flags collected for f77 */
char	xopt[4]		= "-x";		/* unknown option, passed to ld */
#ifdef SCC
char	gbuf[4]		= "-Z";		/* flags of gopt: segmented */
#else
char	gbuf[4];			/* flags buffer of gopt (empty) */
#endif
char	cppbuf[512];			/* flags collected for cpp */
char	*cppp		= cppbuf;
char	*cparsep	= cparsebuf;
char	*f77p		= f77buf + 1;
char	*Bdir		= "";		/* -B directory */
char	*apass		= "p1O2o";	/* -a pass ids */

int	errflag;
int	lflag;				/* -Sl */
int	loadflag	= 1;		/* run ld */
int	Sflag;				/* -S */
int	cflag;				/* -c */
int	pflag;				/* -p */
int	oflag;				/* -O: run popt */
int	dflag;				/* -:d */
int	zflag;				/* -:z */
int	nflag;				/* -:n */
int	rflag;				/* -:r */
int	Pflag;				/* -P or -E */
int	Eflag;				/* -E */
int	vflag;				/* -:v */
int	Fflag;				/* -F */
int	gonce		= 1;		/* codgen flags not yet completed */
int	dexited;

char	t_I[256], t_D[256], t_s[256], t_o[256], t_p[256], t_z[256], t_S[256];
char	curpass[256];			/* name of the pass running */
char	cmd[2048];
char	*obj1;				/* object name for a single file */
char	**av;				/* argument vector for ld */
char	**clp;				/* its end */
int	pid;

int	getsuf(), testfile(), callsys(), await(), nodup(), compile();
int	idexit(), dexit(), gopass(), asspass();
char	*setsuf(), *basename(), *lastchar(), *savestr(), *alloc();
char	*cat(), *cutsuf();

/* the suffix letter of a name like x.c, else 0 */
int
getsuf(s)
char	*s;
{
	if (s == 0)		/* the original reads a zero byte at address 0 */
		return (0);
	for (; *s; s++)
		if (s[0] == '.' && s[1] != 0 && s[2] == 0)
			return (s[1]);
	return (0);
}

/* the last component of a path name */
char *
basename(s)
char	*s;
{
	char	*t;

	t = s;
	for (; *s; s++)
		if (*s == '/')
			t = s + 1;
	return (t);
}

char *
lastchar(s)
char	*s;
{
	while (*s)
		s++;
	return (s - 1);
}

/* make the suffix of s ".o" (in place); return the base name */
char *
setsuf(s)
char	*s;
{
	*lastchar(s) = 'o';
	return (basename(s));
}

char *
alloc(n)
int	n;
{
	char	*p;

	p = (char *)calloc(1, n);
	if (p == 0)
		fatal("out of memory");
	return (p);
}

char *
savestr(s)
char	*s;
{
	char	*p;

	p = alloc(strlen(s) + 1);
	return (strcpy(p, s));
}

/* copy of s without its last ".suffix" */
char *
cutsuf(s)
char	*s;
{
	char	*p, *q, *r;

	p = alloc(strlen(s) + 1);
	strcpy(p, s);
	q = p;
	r = p;
	for (; *q; q++)
		if (*q == '.')
			r = q;
	*r = 0;
	return (p);
}

/* new string a followed by b */
char *
cat(a, b)
char	*a, *b;
{
	char	*p;

	p = alloc(strlen(a) + strlen(b) + 1);
	strcpy(p, a);
	return (strcat(p, b));
}

/* is the name not yet in the list for ld? */
int
nodup(l)
char	*l;
{
	char	**p;

	for (p = av; p < clp; p++)
		if (strcmp(*p, l) == 0)
			return (0);
	return (1);
}

dbg(s)
char	*s;
{
	if (vflag)
		fprintf(diag, "\r %s\t\t\r", s);
}

error(fmt, a, b)
char	*fmt, *a, *b;
{
	char	buf[256];

	sprintf(buf, fmt, a, b);
	fprintf(diag, "Error in file %s: %s\n", cfile, buf);
	dexit(1);
	if (dflag)
		abort();
	exit(1);
}

fatal(fmt, a, b)
char	*fmt, *a, *b;
{
	char	buf[256];

	sprintf(buf, fmt, a, b);
	fprintf(diag, "Compiler error in file %s: %s\n", cfile, buf);
	dexit(1);
	if (dflag)
		abort();
	exit(1);
}

unl(f)
char	*f;
{
	if (!dflag && f && *f)
		unlink(f);
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

setsigs(f)
sigfn	f;
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

int
idexit()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	dexit(2);
}

int
dexit(x)
int	x;
{
	if (!dexited) {
		dexited = 1;
		cleanup();
	}
	dbg("\t\t");
	if (rflag && x == 0)
		execl(outfile, outfile, (char *)0);
	exit(x);
}

/* wait for the child p and return its exit code */
int
await(p)
int	p;
{
	int	w, status;

	while ((w = wait(&status)) != p)
		if (w == -1)
			fatal("Bad wait code.");
	setsigs((sigfn)idexit);
	if ((status & 0xff) != 0) {
		if (status == 0x8b)
			fatal("Segmentation violation in %s", curpass);
		else if (status != 2)
			fatal("Termination code : %o in %s", (char *)(long)status, curpass);
		dexit(3);
	}
	return ((signed char)((status >> 8) & 0xff));
}

/* can the file be read? */
int
testfile(f)
char	*f;
{
	FILE	*fp;

	fp = fopen(f, "r");
	if (fp) {
		fclose(fp);
		return (0);
	}
	error("Cannot read file '%s'", f);
	return (1);
}

/* run the file; scripts without #! go to the shell */
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
		fatal("%s: too large", file);
}

/*
 * Run the command line s (a word list with <in, >out and >>out) through
 * the program named by its first word.  Returns the exit code, -1 when
 * the line is empty.
 */
int
callsys(s)
char	*s;
{
	char	*argv[100];
	char	*in, *out;
	char	*p;
	int	n, app;

	if (zflag)
		fprintf(diag, "%s\n", s);
	in = 0;
	out = 0;
	app = 0;
	argv[0] = shell;
	n = 1;
	p = s;
	while (isspace((unsigned char)*p))
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
		while (*p && !isspace((unsigned char)*p))
			p++;
		if (*p) {
			*p++ = 0;
			while (isspace((unsigned char)*p))
				p++;
		}
	}
	if (n == 1)
		return (-1);
	argv[n] = 0;
	if (nflag)
		return (0);
	strcpy(curpass, argv[1]);
	pid = fork();
	if (pid == 0) {
		if (in)
			freopen(in, "r", stdin);
		if (out)
			freopen(out, app ? "a" : "w", stdout);
		setsigs((sigfn)0);
		doexec(argv[1], argv);
		fatal("cannot load %s", argv[1]);
	}
	return (await(pid));
}

nameit(buf, suffix)
char	*buf, *suffix;
{
	if (!dflag)
		sprintf(buf, "%s/cc.%d.%s", tmpdir, pid, suffix);
	else
		sprintf(buf, "cc.%d.%s", pid, suffix);
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

/* global optimizer and code generator */
int
gopass()
{
	char	*t;

	if (optlev != '0') {
		dbg("GOPT.");
		t = cutsuf(t_I);
		sprintf(cmd, "%s %s %s -%c -E %s >%s", gopt, gbuf, goptflags,
		    optlev, t, t_z);
		if (!passexists(gopt))
			fprintf(diag, "Global optimizer not on system- no optimization performed\n");
		else if (callsys(cmd) != 0)
			fprintf(diag, "Global optimizer error - no optimization performed\n");
		else {
			dbg("MOVE.");
			sprintf(cmd, "%s %s %s", mv, t_z, t_I);
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
	t = cutsuf(t_D);
	sprintf(cmd, "%s %s -E %s <%s >%s", codgen, codgenflags, t, t_I, t_s);
	return (callsys(cmd));
}

/* floating point filter, peephole optimizer, assembler or listing */
int
asspass()
{
	char	buf[64];

	if (Fflag) {
		dbg("FPF.");
		sprintf(cmd, "%s <%s >%s", fpf, t_s, t_z);
		if (!passexists(fpf))
			fprintf(diag, "Floating point filter not on system- no optimization performed\n");
		else if (callsys(cmd) != 0)
			fprintf(diag, "Floating point filter error - no optimization performed\n");
		else {
			dbg("MOVE.");
			sprintf(cmd, "%s %s %s", mv, t_z, t_s);
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
			sprintf(cmd, "%s %s %s", mv, t_z, t_s);
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
			sprintf(cmd, "%s %s %s", mv, t_s, t_o);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
		return (0);
	}
	strcpy(buf, t_s);
	dbg("ASM.");
	if (dflag)
		sprintf(cmd, "%s %s -u -v -o %s %s", cas, asflags, t_o, buf);
	else
		sprintf(cmd, "%s %s -u -o %s %s", cas, asflags, t_o, buf);
	if (callsys(cmd) != 0)
		fatal("assembler error");
	return (0);
}

/* compile one C file up to the object file; 0 when done */
int
compile(f)
char	*f;
{
	int	r;
	char	*t;

	cfile = f;
	if (cflag || Sflag) {
		strcpy(t_o, basename(f));
		*lastchar(t_o) = 'o';
	}
	if (Eflag)
		t_p[0] = 0;
	else if (Pflag) {
		strcpy(t_p, basename(f));
		*lastchar(t_p) = 'i';
	}
	dbg("CPP.");
	sprintf(cmd, SEG("%s %s %s  %s %s", "%s -DNULL=0L %s %s  %s %s"), cpp, cppflags, cppbuf, cfile, t_p);
	if (callsys(cmd) != 0)
		error("Error.  No assembly.");
	if (Pflag)
		return (0);
	dbg("CPARSE.");
	t = cutsuf(t_I);
	sprintf(cmd, "%s %s %s -E %s <%s", cparse, cparsebuf, cparseflags, t, t_p);
	r = callsys(cmd);
	if (r != 0) {
		if (r == 1)
			error("Error.  No assembly.");
		fatal("Compiler error.");
	}
	if (gopass() != 0)
		fatal("Compiler error.");
	asspass();
	return (0);
}

/*
 * A pass may be given as a command with arguments in cc.conf (for example an
 * emulator with its options and the program).  Such a pass cannot be checked
 * with access(): the program is not a file of the host.
 */
int
passexists(p)
char	*p;
{
	if (strchr(p, ' '))
		return (1);
	return (access(p, 0) == 0);
}

/* exec the loader ld (a command with arguments, see above) with the arguments of v */
ldexec(v)
char	**v;
{
	char	*w[100], *s, *sp;
	char	**q;
	int	n;

	n = 0;
	for (s = savestr(ld); *s; s = sp) {
		w[n++] = s;
		sp = strchr(s, ' ');
		if (sp == 0)
			break;
		*sp++ = 0;
		while (*sp == ' ')
			sp++;
	}
	for (q = v + 1; *q && n < 98; q++)
		w[n++] = *q;
	w[n] = 0;
	execv(w[0], w);
}

/* the settings that cc.conf may give */
struct conf_var	confvars[] = {
	{ "cpp", &cpp }, { "cparse", &cparse }, { "gopt", &gopt },
	{ "codgen", &codgen }, { "popt", &popt }, { "clist", &clist },
	{ "cas", &cas }, { "ld", &ld }, { "fpf", &fpf }, { "fpe", &fpe },
	{ "fpent", &fpent }, { "libcfpf", &libcfpf }, { "crt0", &crt0 },
	{ "mcrt0", &mcrt0 }, { "libc", &libc }, { "m4", &m4 },
	{ "basic", &basicc }, { "f77", &f77 }, { "shell", &shell },
	{ "mv", &mv }, { "tmpdir", &tmpdir }, { 0, 0 }
};

/* defaults, overridden by cc.conf next to the executable, then by -T */
loadconf(argv0)
char	*argv0;
{
	struct conf	*c;
	char	*path;

	path = conf_sibling(argv0, CONFNAME);
	if (path == 0)
		return;
	c = conf_read(path);
	if (c == 0) {
		if (errno != ENOENT)
			exit(1);		/* the message is printed */
		return;
	}
	if (conf_apply(c, confvars))
		exit(1);
	conf_free(c);
}

/* run the loader: argument vector v up to e */
load(v, e)
char	**v, **e;
{
	char	**p, **q;
	int	child;

	q = e;
	for (p = libs; *p; p++)
		*q++ = *p;
	*q++ = "-o";
	*q++ = outfile;
	*q = 0;
	dbg("LOAD.");
	if (zflag) {
		for (p = v; p < q; p++)
			fprintf(diag, "%s ", *p);
		fprintf(diag, "\n");
	}
	if (nflag)
		return;
	child = fork();
	if (child == 0) {
		setsigs((sigfn)0);
		ldexec(v);
		fatal("couldn't load %s", ld);
	}
	if (await(child) != 0) {
		errflag = 1;
		if (nfiles == 1 && !cflag) {
			dbg("MOVE.");
			sprintf(cmd, "%s %s %s", mv, t_o, obj1);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
	}
}

/* the path variable for a pass letter of -a / -T */
char **
passvar(c)
int	c;
{
	switch (c) {
	case 'p':	return (&cpp);
	case '1':	return (&cparse);
	case 'O':	return (&gopt);
	case '2':	return (&codgen);
	case 'o':	return (&popt);
	case 'a':	return (&cas);
	case 'l':	return (&ld);
	case 'b':	return (&basicc);
	case 'f':	return (&f77);
	case 'F':	return (&crt0);
	case 'P':	return (&fpf);
	}
	return (0);
}

/* the flag variable for a pass letter of -: */
char **
flagvar(c)
int	c;
{
	switch (c) {
	case 'p':	return (&cppflags);
	case '1':	return (&cparseflags);
	case 'O':	return (&goptflags);
	case '2':	return (&codgenflags);
	case 'o':	return (&poptflags);
	case 'a':	return (&asflags);
	case 'l':	return (&ldflags);
	case 'f':	return (&f77flags);
	case 'b':	return (&basicflags);
	}
	return (0);
}

main(argc, argv)
int	argc;
char	**argv;
{
	char	*p, *s;
	char	**v;
	int	i, c;

	diag = stderr;
	loadconf(argv[0]);
	libs[0] = libc;
	intign = signal(SIGINT, SIG_IGN) == SIG_IGN;
	quitign = signal(SIGQUIT, SIG_IGN) == SIG_IGN;
	hupign = signal(SIGHUP, SIG_IGN) == SIG_IGN;
	termign = signal(SIGTERM, SIG_IGN) == SIG_IGN;
	setsigs((sigfn)idexit);
	av = (char **)alloc((argc + 20) * sizeof (char *));
	av[1] = "-X";
	clp = av + 2;
	*clp++ = "-e";
	*clp++ = "start";
#ifdef SCC
	*clp++ = "-z";
#endif

	/* options */
	for (;;) {
		argc--;
		argv++;
		if (argc <= 0 || argv[0][0] != '-' || argv[0][1] == 0)
			break;
		p = argv[0] + 1;
		while (*p) {
			switch (*p) {
			case 'B':
				if (p[1])
					Bdir = p + 1;
				else
					Bdir = "/copt/passes/";
				goto nextarg;
			case 'a':
				if (!p[1])
					error("Bad option '-a' must be followed by path identifiers, -ap1O2o assumed.");
				apass = p + 1;
				goto nextarg;
			case 'T':
				p++;
				v = passvar(*p);
				if (v == 0)
					error("Bad option '-T%c'.", (char *)(long)*p);
				else
					*v = p + 1;
				goto nextarg;
			case ':':
				p++;
				v = flagvar(*p);
				if (v) {
					*v = p + 1;
					goto nextarg;
				}
				switch (*p) {
				case 'v':	vflag = 1; break;
				case 'd':	dflag = 1; zflag = 1; break;
				case 'z':	zflag = 1; break;
				case 'n':	nflag = 1; break;
				case 'r':	rflag = 1; break;
				default:
					error("Bad option '-:%c'.", (char *)(long)*p);
				}
				goto nextarg;
			case 'N':
				if (p[1] == 's') {
					*clp++ = "-Ns";
					*cparsep++ = '-';
					*cparsep++ = 'L';
					*cparsep++ = ' ';
					goto nextarg;
				}
				if (p[1] == 'l')
					goto nextarg;
				error("Bad option '-N%c'.", (char *)(long)p[1]);
				goto nextarg;
			case 'O':
				if (p[1] == 0) {
					oflag = 1;
					goto nextarg;
				}
				if (isdigit((unsigned char)p[1]) || p[1] == 'l' || p[1] == 'r') {
					oflag = 1;
					p++;
					if (isdigit((unsigned char)*p))
						optlev = *p;
					else if (*p == 'l')
						optlev = '4';
					else if (*p == 'r')
						optlev = '5';
				}
				if (p[1] == 'p')
					oflag = 1;
				goto nextarg;
			case 'F':
				Fflag = 1;
				libs[0] = fpe;
				libs[1] = fpent;
				libs[2] = libcfpf;
				break;
			case 'S':
				Sflag = 1;
				if (p[1] == 'l') {
					lflag = 1;
					p++;
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
					*cppp++ = *p++;
				while (*p);
				*cppp++ = ' ';
				goto nextarg;
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
				if (getsuf(argv[1]) == 'c') {
					fprintf(diag, "would overwrite %s\n", argv[1]);
					exit(1);
				}
				argv++;
				outfile = *argv;
				argc--;
				break;
			case 'l':
				p[-1] = '-';
				*clp++ = p - 1;
				goto nextarg;
			default:
				xopt[1] = *p;
				*clp++ = savestr(xopt);
				break;
			}
			p++;
		}
	nextarg:
		;
	}

	pid = getpid();
	mktmps();
	*cppp = 0;
	*cparsep = 0;

	/* -B: take the passes named by -a from that directory */
	if (*Bdir) {
		for (p = apass; *p; ) {
			c = *p++;
			v = passvar(c);
			if (v == 0 || c == 'P')
				error("Bad option '-a%c'.", (char *)(long)c);
			*v = cat(Bdir, basename(*v));
		}
	}
	av[0] = ld;
	*clp++ = pflag ? mcrt0 : crt0;

	/* count the source files */
	for (i = 0; i < argc; i++) {
		switch (getsuf(argv[i])) {
		case 'b':
		case 'c':
		case 'f':
		case 'p':
		case 's':
			nfiles++;
		}
	}
	if (nfiles > 1)
		cflag = 1;

	/* process the files */
	for (i = 0; i < argc; i++) {
		cfile = argv[i];
		c = getsuf(cfile);
		switch (c) {
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
				s = setsuf(argv[i]);
				if (nodup(s))
					*clp++ = s;
			} else
				*clp++ = t_o;
			break;
		case 'f':
			if (nfiles > 1)
				fprintf(diag, "%s:\n", argv[i]);
			else if (nfiles == 1)
				obj1 = setsuf(savestr(argv[i]));
			if (testfile(argv[i]) != 0) {
				errflag = 1;
				break;
			}
			dbg("F77.");
			sprintf(cmd, "%s %s %s %s", f77, f77buf, f77flags, argv[i]);
			if (callsys(cmd) != 0) {
				errflag = 1;
				break;
			}
			if (cflag) {
				s = setsuf(argv[i]);
				if (nodup(s))
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
				s = setsuf(argv[i]);
				if (nodup(s))
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
			if (strcmp(argv[i], "-o") == 0) {
				i++;
				outfile = argv[i];
			} else
				*clp++ = argv[i];
			break;
		}
	}
	if (loadflag && !errflag && !Sflag)
		load(av, clp);
	dexit(errflag);
}
