/*
 * ranlib - create table of contents (__.SYMDEF) for an archive.
 * Reconstruction of WEGA ranlib 1.1 (07/24/82), derived from the
 * 4.1BSD ranlib adapted to the s.out object format.
 *
 * Host version (Linux, gcc): made from WEGA/src/cmd/ranlib.c by the port rules in
 * host/HANDOFF_wega_disasm.md. The file structures are byte arrays in the Z8000 layout
 * (see sobj.h), so the table and the archive are written exactly as on WEGA.
 *
 * Like the original it lets the program ar insert the table: "ar rlb first
 * archive __.SYMDEF" or "ar rl __.SYMDEF archive". Here that is the host ar (host/ar); the
 * file ranlib.conf next to the executable can name another one:
 *     ar = /path/to/ar
 * Without it ../ar/ar relative to the executable is used. The environment variable RANLIB_AR wins over both.
 */
static char sccsid[] __attribute__((unused)) = "@[$]ranlib.c\t1.1  07/24/82 02:49:23 - 87wega3.2";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include "sobj.h"
#include "conf.h"

#define	TABSZ	700
#define	ENTRY	12			/* struct tab: long cloc; char cname[8] */

FILE	*fo;
int	longnm, new;
FILE	*fi;
unsigned char tab[(TABSZ + 130) * ENTRY];	/* room for the long names that spill over */
unsigned char archdr[ARHDR_SIZE];
int32_t	off, offdelta;
char	*av0;
char	firstname[18];
int32_t	oldoff;
unsigned char exp_[SEXEC_SIZE];			/* struct s_exec exp; */
int	tnum;
char	tempnm[] = "__.SYMDEF";
char	*arprog;
unsigned char sym[NLIST_SIZE + 128 + 14];	/* the symbol and, behind it, the pieces of a long name */

#define	ARSIZE		((int32_t)sobj_be32(archdr + 22))	/* archdr.ar_size */

static char *trim(char *);
static int readsym(unsigned char *, FILE *);
static int nextel(FILE *);
static void stash(unsigned char *);
static int fnew(void);
static void fixdate(char *);
static char *findar(const char *);

static char *
trim(char *p1)
{
	char *p2;

	p2 = p1;
	while(*p2) {
		if(*p2++ == '/')
			if(*p2)
				p1 = p2;
	}
	return(p1);
}

int
main(int argc, char **argv)
{
	char cmdbuf[1024];
	int32_t o;
	int n;
	unsigned flag, syms, segt;
	size_t got;

	av0 = trim(argv[0]);
	arprog = findar(argv[0]);
	--argc;
	while(argc--) {
		fi = fopen(*++argv, "r");
		if(fi == NULL) {
			fprintf(stderr, "%s: cannot open %s\n", av0, *argv);
			continue;
		}
		off = 2L;
		got = fread(exp_, 1, 2, fi);		/* the buffer of the member headers, as in the original */
		(void)got;
		if(sobj_be16(exp_) != ARMAG) {
			fprintf(stderr, "not archive: %s\n", *argv);
			continue;
		}
		fseek(fi, 0L, 0);
		new = tnum = 0;
		if(nextel(fi) == 0) {
			fclose(fi);
			continue;
		}
		do {
			got = fread(exp_, 1, SEXEC_SIZE, fi);
			(void)got;
			if(!sobj_goodmagic(sobj_be16(exp_)))
				continue;
			if(sobj_swap_flg)
				sobj_swab(exp_, SEXEC_SIZE);
			o = sobj_be32(exp_ + 2);		/* s_imsize */
			flag = sobj_be16(exp_ + 18);		/* s_flag */
			if((flag & SF_STRIP) == 0)
				o *= 2;
			segt = sobj_be16(exp_ + 10);		/* s_segt */
			o += segt;
			fseek(fi, o, 1);
			syms = sobj_be16(exp_ + 12);		/* s_syms */
			n = syms / NLIST_SIZE;
			if(n == 0) {
				fprintf(stderr, "%s: %s-- no name list\n", av0, (char *)archdr);
				continue;
			}
			while(--n >= 0) {
				longnm = readsym(sym, fi);
				if(longnm < 0)
					break;			/* the original does not stop here */
				n -= longnm;
				if((sym[4] & N_EXT) != 0)		/* sn_type */
					if((sym[4] & N_TYPE) != 0)
						stash(sym);
			}
		} while(nextel(fi));
		new = fnew();
		fclose(fi);
		fo = fopen(tempnm, "w");
		if(fo == NULL) {
			fprintf(stderr, "can't create temporary\n");
			exit(1);
		}
		fwrite(tab, tnum, ENTRY, fo);
		fclose(fo);
		if(new)
			sprintf(cmdbuf, "'%s' rlb %s '%s' %s\n", arprog, firstname, *argv, tempnm);
		else
			sprintf(cmdbuf, "'%s' rl '%s' %s\n", arprog, *argv, tempnm);
		if(system(cmdbuf))
			fprintf(stderr, "can't execute %s\n", cmdbuf);
		else
			fixdate(*argv);
		unlink(tempnm);
	}
	exit(0);
}

/*
 * readsym of the OLD libc that is linked into the original ranlib (src/lib/oldlibc/readsym):
 * it reads the continuation entries over the symbol itself (not behind it as the readsym of
 * the shipped libc.a does). So after a long name sym holds the first piece of the name text,
 * and main tests and stashes that; most long names are lost and some enter the table with
 * garbage. Kept as it is: the host ranlib has to write the same __.SYMDEF. Returns -1 at
 * the end of the file (the original: 0xffff).
 */
static int
readsym(unsigned char *sym, FILE *stream)
{
	int foo2, len;

	if(fread(sym, 1, NLIST_SIZE, stream) != NLIST_SIZE)
		return(-1);
	if(!(sym[6] & 128))
		return(0);
	len = (sym[6] & 127) - 8;
	foo2 = (len + 13) / 14;
	if((int)fread(sym, NLIST_SIZE, foo2, stream) != foo2)
		foo2 = -1;
	return(foo2);
}

static int
nextel(FILE *af)
{
	size_t r;

	oldoff = off;
	fseek(af, off, 0);
	r = fread(archdr, 1, sizeof(archdr), af);
	if(r <= 0)
		return(0);
	if((ARSIZE & 1) != 0)
		sobj_put32(archdr + 22, ARSIZE + 1);
	off = ftell(af) + ARSIZE;
	return(1);
}

static void
stash(unsigned char *s)
{
	int i, n;
	unsigned char *e;

	if(tnum >= TABSZ) {
		fprintf(stderr, "symbol table overflow\n");
		exit(1);
	}
	if(longnm == 0)
		n = SHORTNAME;
	else
		n = s[6] & 0x7F;			/* sn_name[0] */
	e = tab + tnum * ENTRY;
	for(i = 0; i < n; i++)
		e[4 + i] = s[6 + i];			/* cname[i] = sn_name[i] */
	sobj_put32(e, oldoff);				/* cloc */
	tnum++;
	if(longnm) {
		tnum += ((n = n - SHORTNAME) + 11) / 12;
	}
}

static int
fnew(void)
{
	int i, n;

	offdelta = tnum * ENTRY + ARHDR_SIZE;
	off = 2L;
	nextel(fi);
	if(strncmp((char *)archdr, tempnm, 14) == 0) {
		new = 0;
		offdelta -= ARSIZE + ARHDR_SIZE;
	} else {
		new = 1;
		memset(firstname, 0, sizeof firstname);
		memcpy(firstname, archdr, strnlen((char *)archdr, 14));	/* strncpy(firstname, ar_name, 14): firstname is zeroed */
	}
	for(i = 0; i < tnum; i++) {
		unsigned char *e = tab + i * ENTRY;

		sobj_put32(e, sobj_be32(e) + offdelta);
		if(sobj_swap_flg) {
			sobj_swab(e, 2);			/* longswap of the entry */
			sobj_swab(e + 2, 2);
		}
		if((e[4] & 0x80) != 0) {
			n = e[4] & 0x7F;
			i += ((n = n - SHORTNAME) + 11) / 12;
		}
	}
	return(new);
}

static void
fixdate(char *s)
{
	int fd;
	unsigned char date[4];
	ssize_t w;

	fd = open(s, 1);
	if(fd < 0) {
		fprintf(stderr, "can't reopen %s\n", s);
		return;
	}
	sobj_put32(date, (uint32_t)time(0) + 5);
	lseek(fd, 14 + 2, 0);			/* ar_date of the first member + the magic */
	w = write(fd, date, 4);
	(void)w;
	close(fd);
}

/* the ar program: ranlib.conf next to the executable, else ../ar/ar */
static char *
findar(const char *argv0)
{
	char *path = conf_sibling(argv0, "ranlib.conf"), *a = 0;
	struct conf *c;

	if (getenv("RANLIB_AR"))		/* the environment wins over ranlib.conf */
		return strdup(getenv("RANLIB_AR"));
	if(path) {
		c = conf_read(path);
		if(c == 0) {
			if(errno != ENOENT)
				exit(1);
		} else {
			const char *v = conf_get(c, "ar");

			if(v)
				a = strdup(v);
			conf_free(c);
		}
		free(path);
	}
	if(a == 0) {
		char *rel = conf_sibling(argv0, "../ar/ar");

		a = rel ? rel : strdup("ar");
	}
	return a;
}
