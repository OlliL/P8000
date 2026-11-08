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
 * Host version (Linux, gcc): made from WEGA/src/cmd/cc/cc.c by the port rules in host/HANDOFF_wega_disasm.md.
 * The pass programs are called by absolute path (/lib/cpp, ...) unless cc.conf (scc.conf for scc), which lies
 * next to the executable, names others (see cc.conf.example); a pass may be a command with arguments (an
 * emulator and the program). The OLD_* switches of the old compilers are not part of it.
 */
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/wait.h>

#include "conf.h"

#ifdef SCC
#define CONFNAME	"scc.conf"
#define SEG(c, s)	s
#else
#define CONFNAME	"cc.conf"
#define SEG(c, s)	c
#endif

typedef void (*sigfn)(int);

char	*libs[4];			/* libraries for ld, libs[0] set in main */
FILE	*diag;				/* stderr, set in main */
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

char	t_I[256], t_D[256], t_s[256], t_o[256], t_p[256], t_z[256], t_S[256];
char	curpass[256];			/* name of the pass running */
char	cmd[2048];
char	*obj1;				/* object name for a single file */
char	**av;				/* argument vector for ld */
char	**clp;				/* its end */
int	pid;

char	*fname	= "basic";		/* base name of the file being compiled, without suffix */
char	fbuf[256];			/* the original has 20 */

int main(int argc, char **argv);
int compile(char *f);
int gopass(void);
void asspass(void);
int passexists(char *p);
void ldexec(char **v);
void loadconf(char *argv0);
void load(char **v, char **e);
int callsys(char *s);
void doexec(char *file, char **argv);
void dbg(char *s);
void dexit(int x);
void setsigs(sigfn f);
void idexit(int sig);
int await(int p);
int testfile(char *f);
void closeout(FILE **fpp);
void cleanup(void);
int nonempty(char *f);
void mktmps(void);
void unl(char *f);
void nameit(char *buf, char *suffix);
int getsuf(char *s);
char * basename(char *s);
char * lastchar(char *s);
char * setsuf(char *s);
void cantopen(char *s);
char * alloc(int n);
char * savestr(char *s);
char * cutsuf(char *s);
char * cat(char *a, char *b);
int any(int c, char *s);
int nodup(char *l);
int error(char *s);
int error1(char *s, ...);
int error2(char *s, ...);
int fatal(char *s);
int fatal1(char *s, ...);
int fatal2(char *s, ...);
int fatal3(char *s, ...);

/*
 * A pass may be given as a command with arguments in cc.conf (for example an
 * emulator with its options and the program).  Such a pass cannot be checked
 * with access(): the program is not a file of the host.
 */
int
passexists(char *p)
{
	if (strchr(p, ' '))
		return (1);
	return (access(p, 0) == 0);
}

/* exec the loader ld (a command with arguments, see above) with the arguments of v */
void
ldexec(char **v)
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
void
loadconf(char *argv0)
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

int
main(int argc, char **argv)
{
	int i = 0;		/* uninitialised in the original: a register that holds 0 here ("-o" looks at argv[i + 1]) */
	int c;
	char *t;
	char *s;

	diag = stderr;
	loadconf(argv[0]);
	libs[0] = libc;
	intign = signal(SIGINT, SIG_IGN) == SIG_IGN;
	quitign = signal(SIGQUIT, SIG_IGN) == SIG_IGN;
	hupign = signal(SIGHUP, SIG_IGN) == SIG_IGN;
	termign = signal(SIGTERM, SIG_IGN) == SIG_IGN;
	setsigs(idexit);
	av = (char **)alloc((argc + 20) * sizeof (char *));
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
					/* fall through - -:d also echoes the commands */
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
				/* fall through - does not happen: error1 exits */
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
				if (isdigit((unsigned char)t[1]) || t[1] == 'l' || t[1] == 'r') {
					oflag = 1;
					t++;
					if (isdigit((unsigned char)*t))
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
int
compile(char *f)
{
	int i;
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
int
gopass(void)
{
	static int gonce = 1;

	if (optlev != '0') {
		dbg("GOPT.");
		sprintf(cmd, "%s %s %s -%c -E %s >%s", gopt, gbuf, goptflags, optlev, cutsuf(t_I), t_z);
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
	sprintf(cmd, "%s %s -E %s <%s >%s", codgen, codgenflags, cutsuf(t_D), t_I, t_s);
	return (callsys(cmd));
}

/* floating point filter, peephole optimizer, then assembler or listing (-S, -Sl) */
void
asspass(void)
{
	char	buf[256];		/* the original has 26 */

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
void
load(char **v, char **e)
{
	char **p;
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
		ldexec(v);
		fatal2("couldn't load %s", ld);
	}
	if (await(i) != 0) {
		errflag = 1;
		if (nfiles == 1 && !cflag) {
			dbg("MOVE.");
			sprintf(cmd, "%s %s %s", mv, t_o, obj1);
			if (callsys(cmd) != 0)
				fatal(" Cannot move files");
		}
	}
}

/* run the command line s (words, "<in", ">out", ">>out") and return its exit code; -1 for an empty line */
int
callsys(char *s)
{
	char	*argv[100];
	FILE	*fp = NULL;
	int	app;
	int	i;
	char *p;
	char *in, *out;
	int n;

	if (zflag)
		fprintf(diag, "%s\n", s);
	out = in = 0;
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
		for (; !isspace((unsigned char)*p); p++)
			if (*p == 0)
				break;
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
	i = fork();
	if (i == 0) {
		if (in)
			fp = freopen(in, "r", stdin);		/* the original does not look at the result */
		if (out)
			fp = freopen(out, app ? "a" : "w", stdout);
		(void)fp;
		setsigs(0);
		doexec(argv[1], argv);
		fatal2("cannot load %s", argv[1]);
	}
	return (await(i));
}

/* exec the file; a file that is not a program goes to the shell */
void
doexec(char *file, char **argv)
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

void
dbg(char *s)
{
	if (vflag)
		fprintf(diag, "\r %s\t\t\r", s);
}

/* remove the temporary files and exit; -:r runs the result */
void
dexit(int x)
{
	static int dexited;

	if (!dexited) {
		dexited = 1;
		cleanup();
	}
	dbg("\t\t");
	if (rflag && !x)
		execl(outfile, outfile, (char *)0);
	exit(x);
}

void
setsigs(sigfn f)
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

void
idexit(int sig)
{
	(void)sig;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	dexit(2);
}

/* wait for the child p and return its exit code */
int
await(int p)
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
int
testfile(char *f)
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
void
closeout(FILE **fpp)
{
	if (fpp && *fpp && *fpp != stdout) {
		if (ferror(*fpp))
			fatal("writing error");
		fclose(*fpp);
	}
	*fpp = 0;
}

void
cleanup(void)
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
int
nonempty(char *f)
{
	struct stat buf;

	if (stat(f, &buf) < 0)
		return (-1);
	return (buf.st_size > 0);
}

void
mktmps(void)
{
	nameit(t_I, "I");
	nameit(t_D, "D");
	nameit(t_s, "s");
	nameit(t_o, "o");
	nameit(t_p, "p");
	nameit(t_z, "z");
	nameit(t_S, "S");
}

void
unl(char *f)
{
	if (!dflag && f && *f)
		unlink(f);
}

void
nameit(char *buf, char *suffix)
{
	if (!dflag)
		sprintf(buf, "%s/cc.%d.%s", tmpdir, pid, suffix);
	else
		sprintf(buf, "cc.%d.%s", pid, suffix);
}

int
getsuf(char *s)
{
	if (s == 0)		/* the original reads a zero byte at address 0 */
		return (0);
	for (; *s; s++)
		if (*s == '.' && s[1] && !s[2])
			return (s[1]);
	return (0);
}

char *
basename(char *s)
{
	char *p;

	p = s;
	for (; *s; s++)
		if (*s == '/')
			p = s + 1;
	return (p);
}

char *
lastchar(char *s)
{
	while (*s)
		s++;
	return (s - 1);
}

char *
setsuf(char *s)
{
	*lastchar(s) = 'o';
	return (basename(s));
}

/* unused */
void
cantopen(char *s)
{
	fatal2("cannot open intermediate file %s", s);
}

char *
alloc(int n)
{
	char *p;

	p = calloc(1, n);
	if (p == 0)
		fatal("out of memory");
	return (p);		/* the original returns p by chance: it is still in the return register */
}

char *
savestr(char *s)
{
	return (strcpy(alloc(strlen(s) + 1), s));
}

char *
cutsuf(char *s)
{
	char *p, *q, *r;

	p = strcpy(alloc(strlen(s) + 1), s);
	r = p;		/* uninitialised in the original: without a "." it cuts the string off at its start */
	for (q = p; *q; q++)
		if (*q == '.')
			r = q;
	*r = 0;
	return (p);
}

char *
cat(char *a, char *b)
{
	char	*p;

	p = strcpy(alloc(strlen(a) + strlen(b) + 1), a);
	return (strcat(p, b));
}

/* unused: is c in the string s */
int
any(int c, char *s)
{
	while (*s)
		if (c == *s++)
			return (1);
	return (0);
}

/* is the name not yet in the argument list for ld? */
int
nodup(char *l)
{
	char **p;

	for (p = av; p < clp; p++)
		if (strcmp(*p, l) == 0)
			return (0);
	return (1);
}

int
error(char *s)
{
	fprintf(diag, "Error in file %s: %s\n", cfile, s);
	dexit(1);
	if (dflag)
		abort();
	exit(1);
}

int
error1(char *s, ...)
{
	char	buf[256];		/* the original has 100 */
	va_list	ap;

	va_start(ap, s);
	vsnprintf(buf, sizeof buf, s, ap);
	va_end(ap);
	error(buf);
	return 0;
}

int
error2(char *s, ...)
{
	char	buf[256];		/* the original has 100 */
	va_list	ap;

	va_start(ap, s);
	vsnprintf(buf, sizeof buf, s, ap);
	va_end(ap);
	error(buf);
	return 0;
}

int
fatal(char *s)
{
	fprintf(diag, "Compiler error in file %s: %s\n", cfile, s);
	dexit(1);
	if (dflag)
		abort();
	exit(1);
}

int
fatal1(char *s, ...)
{
	char	buf[256];		/* the original has 100 */
	va_list	ap;

	va_start(ap, s);
	vsnprintf(buf, sizeof buf, s, ap);
	va_end(ap);
	fatal(buf);
	return 0;
}

int
fatal2(char *s, ...)
{
	char	buf[256];		/* the original has 100 */
	va_list	ap;

	va_start(ap, s);
	vsnprintf(buf, sizeof buf, s, ap);
	va_end(ap);
	fatal(buf);
	return 0;
}

int
fatal3(char *s, ...)
{
	char	buf[256];		/* the original has 100 */
	va_list	ap;

	va_start(ap, s);
	vsnprintf(buf, sizeof buf, s, ap);
	va_end(ap);
	fatal(buf);
	return 0;
}
