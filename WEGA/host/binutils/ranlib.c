/*
 * ranlib - create the table of contents (__.SYMDEF) of an archive; host version
 * of the WEGA /usr/bin/ranlib (ranlib.c 1.1 07/24/82), derived from WEGA/src/cmd/ranlib.c.
 * The file structures are byte arrays in the Z8000 layout (see sobj.h), so the
 * table and the archive are written exactly as on WEGA.
 *
 * Like the original it lets the program ar insert the table: "ar rlb first
 * archive __.SYMDEF" or "ar rl __.SYMDEF archive". Here that is the host ar (host/ar), the
 * file ranlib.conf next to the executable can name another one:
 *     ar = /path/to/ar
 * Without it ../ar/ar relative to the executable is used.
 */
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

static unsigned char archdr[ARHDR_SIZE];
static unsigned char exp_[SEXEC_SIZE];
static FILE *fi, *fo;
static int new_, longnm;
static int32_t off, oldoff, offdelta;
static unsigned char tab[(TABSZ + 130) * ENTRY];	/* room for the long names that spill over */
static int tnum;
static char tempnm[] = "__.SYMDEF";
static char firstname[18];
static char *av0;
static char *arprog;
static unsigned char sym[NLIST_SIZE + 128 + 14];	/* the symbol and, behind it, the pieces of a long name */

#define	ARSIZE		((int32_t)sobj_be32(archdr + 22))

static char *
trim(char *s)
{
	char *p1, *p2;

	p1 = s;
	p2 = s;
	while (*p2) {
		if (*p2++ == '/')
			if (*p2)
				p1 = p2;
	}
	return p1;
}

static int
nextel(FILE *af)
{
	size_t r;

	oldoff = off;
	fseek(af, off, 0);
	r = fread(archdr, 1, sizeof archdr, af);
	if (r <= 0)
		return 0;
	if (ARSIZE & 1)
		sobj_put32(archdr + 22, ARSIZE + 1);
	off = ftell(af) + ARSIZE;
	return 1;
}

/*
 * Read one symbol (and the entries of a long name): the number of extra entries, -1 at the
 * end. This is the readsym of the OLD libc that is linked into the original ranlib: it reads
 * the continuation entries over the symbol itself (not behind it as the readsym of the
 * shipped libc.a does). So after a long name sym holds the first piece of the name text, and
 * main tests and stashes that; most long names are lost and some enter the table with
 * garbage. Kept as it is: the host ranlib has to write the same __.SYMDEF.
 */
static int
readsym(unsigned char *sym, FILE *stream)
{
	int foo2, len;

	if (fread(sym, 1, NLIST_SIZE, stream) != NLIST_SIZE)
		return -1;
	if (!(sym[6] & 128))
		return 0;
	len = (sym[6] & 127) - 8;
	foo2 = (len + 13) / 14;
	if ((int)fread(sym, NLIST_SIZE, foo2, stream) != foo2)
		foo2 = -1;
	return foo2;
}

static void
stash(unsigned char *s)
{
	int i, n;
	unsigned char *e;

	if (tnum >= TABSZ) {
		fprintf(stderr, "symbol table overflow\n");
		exit(1);
	}
	if (longnm == 0)
		n = SHORTNAME;
	else
		n = s[6] & 0x7F;
	e = tab + tnum * ENTRY;
	for (i = 0; i < n; i++)
		e[4 + i] = s[6 + i];
	sobj_put32(e, oldoff);
	tnum++;
	if (longnm) {
		n -= SHORTNAME;
		tnum += (n + 11) / 12;
	}
}

static int
fnew(void)
{
	int i, n;

	offdelta = tnum * ENTRY + ARHDR_SIZE;
	off = 2L;
	nextel(fi);
	if (strncmp((char *)archdr, tempnm, 14) == 0) {
		new_ = 0;
		offdelta -= ARSIZE + ARHDR_SIZE;
	} else {
		new_ = 1;
		memset(firstname, 0, sizeof firstname);
		strncpy(firstname, (char *)archdr, 14);
	}
	for (i = 0; i < tnum; i++) {
		unsigned char *e = tab + i * ENTRY;

		sobj_put32(e, sobj_be32(e) + offdelta);
		if (sobj_swap_flg) {
			sobj_swab(e, 2);
			sobj_swab(e + 2, 2);
		}
		if (e[4] & 0x80) {
			n = e[4] & 0x7F;
			n -= SHORTNAME;
			i += (n + 11) / 12;
		}
	}
	return new_;
}

static void
fixdate(char *s)
{
	int fd;
	unsigned char date[4];

	fd = open(s, 1);
	if (fd < 0) {
		fprintf(stderr, "can't reopen %s\n", s);
		return;
	}
	sobj_put32(date, (uint32_t)time(0) + 5);
	lseek(fd, 14 + 2, 0);			/* ar_date of the first member + the magic */
	write(fd, date, 4);
	close(fd);
}

/* the ar program: ranlib.conf next to the executable, else ../ar/ar */
static char *
findar(const char *argv0)
{
	char *path = conf_sibling(argv0, "ranlib.conf"), *a = 0;
	struct conf *c;

	if (path) {
		c = conf_read(path);
		if (c == 0) {
			if (errno != ENOENT)
				exit(1);
		} else {
			const char *v = conf_get(c, "ar");

			if (v)
				a = strdup(v);
			conf_free(c);
		}
		free(path);
	}
	if (a == 0) {
		char *rel = conf_sibling(argv0, "../ar/ar");

		a = rel ? rel : strdup("ar");
	}
	return a;
}

int
main(int argc, char **argv)
{
	char cmdbuf[1024];
	int32_t o;
	int n;
	unsigned flag, syms, segt;

	av0 = trim(argv[0]);
	arprog = findar(argv[0]);
	--argc;
	while (argc--) {
		fi = fopen(*++argv, "r");
		if (fi == NULL) {
			fprintf(stderr, "%s: cannot open %s\n", av0, *argv);
			continue;
		}
		off = 2L;
		fread(exp_, 1, 2, fi);		/* the buffer of the member headers, as in the original */
		if (sobj_be16(exp_) != ARMAG) {
			fprintf(stderr, "not archive: %s\n", *argv);
			continue;
		}
		fseek(fi, 0L, 0);
		new_ = tnum = 0;
		if (nextel(fi) == 0) {
			fclose(fi);
			continue;
		}
		do {
			fread(exp_, 1, SEXEC_SIZE, fi);
			if (!sobj_goodmagic(sobj_be16(exp_)))
				continue;
			if (sobj_swap_flg)
				sobj_swab(exp_, SEXEC_SIZE);
			o = sobj_be32(exp_ + 2);		/* s_imsize */
			flag = sobj_be16(exp_ + 18);
			if (!(flag & SF_STRIP))
				o *= 2;
			segt = sobj_be16(exp_ + 10);
			o += segt;
			fseek(fi, o, 1);
			syms = sobj_be16(exp_ + 12);
			n = syms / NLIST_SIZE;
			if (n == 0) {
				fprintf(stderr, "%s: %s-- no name list\n", av0, (char *)archdr);
				continue;
			}
			while (--n >= 0) {
				longnm = readsym(sym, fi);
				if (longnm < 0)
					break;			/* the original does not stop here */
				n -= longnm;
				if (sym[4] & N_EXT)
					if (sym[4] & N_TYPE)
						stash(sym);
			}
		} while (nextel(fi));
		new_ = fnew();
		fclose(fi);
		fo = fopen(tempnm, "w");
		if (fo == NULL) {
			fprintf(stderr, "can't create temporary\n");
			exit(1);
		}
		fwrite(tab, tnum, ENTRY, fo);
		fclose(fo);
		if (new_)
			sprintf(cmdbuf, "'%s' rlb %s '%s' %s\n", arprog, firstname, *argv, tempnm);
		else
			sprintf(cmdbuf, "'%s' rl '%s' %s\n", arprog, *argv, tempnm);
		if (system(cmdbuf))
			fprintf(stderr, "can't execute %s\n", cmdbuf);
		else
			fixdate(*argv);
		unlink(tempnm);
	}
	exit(0);
}
