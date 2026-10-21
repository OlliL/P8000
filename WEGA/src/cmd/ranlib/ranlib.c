/*
 * ranlib - create table of contents (__.SYMDEF) for an archive.
 * Reconstruction of WEGA ranlib 1.1 (07/24/82), derived from the
 * 4.1BSD ranlib adapted to the s.out object format.
 */
static char sccsid[] = "@[$]ranlib.c\t1.1  07/24/82 02:49:23 - 87wega3.2";

#include <stdio.h>
#include <ar.h>
#include <s.out.h>

struct	ar_hdr	archdr;
struct	s_exec	exp;
long	arsize;
FILE	*fi, *fo;
int	new, longnm;
long	off, oldoff, offdelta;
#define	TABSZ	700
struct	tab {
	long	cloc;
	char	cname[8];
} tab[TABSZ];
int	tnum;
char	tempnm[] = "__.SYMDEF";
char	firstname[18];
char	*av0;
extern	int swap_flg;

char	*trim();
long	ftell();
int	longswap();

char *
trim(s)
char *s;
{
	register char *p1, *p2;

	p1 = s;
	p2 = s;
	while(*p2) {
		if(*p2++ == '/')
			if(*p2)
				p1 = p2;
	}
	return(p1);
}

main(argc, argv)
int argc;
char **argv;
{
	char cmdbuf[256];
	long o;
	struct s_nlist sym;
	char more[128];
	register n;

	av0 = trim(argv[0]);
	--argc;
	while(argc--) {
		fi = fopen(*++argv, "r");
		if(fi == NULL) {
			fprintf(stderr, "%s: cannot open %s\n", av0, *argv);
			continue;
		}
		off = 2L;
		fread((char *)&exp, 1, 2, fi);
		if(exp.s_magic != ARMAG) {
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
			fread((char *)&exp, 1, sizeof(struct s_exec), fi);
			if(!goodmagic(exp.s_magic))
				continue;
			if(swap_flg)
				swab((char *)&exp, (char *)&exp, sizeof(struct s_exec));
			o = exp.s_imsize;
			if(!(exp.s_flag & SF_STRIP))
				o *= 2;
			o += exp.s_segt;
			fseek(fi, o, 1);
			n = exp.s_syms / sizeof(struct s_nlist);
			if(n == 0) {
				fprintf(stderr, "%s: %s-- no name list\n", av0, archdr.ar_name);
				continue;
			}
			while(--n >= 0) {
				longnm = readsym(&sym, fi);
				n -= longnm;
				if(sym.sn_type & N_EXT)
					if(sym.sn_type & N_TYPE)
						stash(&sym);
			}
		} while(nextel(fi));
		new = fnew();
		fclose(fi);
		fo = fopen(tempnm, "w");
		if(fo == NULL) {
			fprintf(stderr, "can't create temporary\n");
			exit(1);
		}
		fwrite((char *)tab, tnum, sizeof(struct tab), fo);
		fclose(fo);
		if(new)
			sprintf(cmdbuf, "ar rlb %s %s %s\n", firstname, *argv, tempnm);
		else
			sprintf(cmdbuf, "ar rl %s %s\n", *argv, tempnm);
		if(system(cmdbuf))
			fprintf(stderr, "can't execute %s\n", cmdbuf);
		else
			fixdate(*argv);
		unlink(tempnm);
	}
	exit(0);
}

nextel(af)
FILE *af;
{
	register r;

	oldoff = off;
	fseek(af, off, 0);
	r = fread((char *)&archdr, 1, sizeof(archdr), af);
	if(r <= 0)
		return(0);
	if(archdr.ar_size & 1)
		archdr.ar_size++;
	off = ftell(af) + archdr.ar_size;
	return(1);
}

stash(s)
struct s_nlist *s;
{
	int i, n;

	if(tnum >= TABSZ) {
		fprintf(stderr, "symbol table overflow\n");
		exit(1);
	}
	if(longnm == 0)
		n = SHORTNAME;
	else
		n = s->sn_name[0] & 0x7F;
	for(i = 0; i < n; i++)
		tab[tnum].cname[i] = s->sn_name[i];
	tab[tnum].cloc = oldoff;
	tnum++;
	if(longnm) {
		n -= SHORTNAME;
		tnum += (n + 11) / 12;
	}
}

fnew()
{
	int i, n;

	offdelta = tnum * sizeof(struct tab) + sizeof(archdr);
	off = 2L;
	nextel(fi);
	if(strncmp(archdr.ar_name, tempnm, sizeof(archdr.ar_name)) == 0) {
		new = 0;
		offdelta -= archdr.ar_size + sizeof(archdr);
	} else {
		new = 1;
		strncpy(firstname, archdr.ar_name, sizeof(archdr.ar_name));
	}
	for(i = 0; i < tnum; i++) {
		tab[i].cloc += offdelta;
		if(swap_flg)
			longswap(&tab[i]);
		if(tab[i].cname[0] & 0x80) {
			n = tab[i].cname[0] & 0x7F;
			n -= SHORTNAME;
			i += (n + 11) / 12;
		}
	}
	return(new);
}

fixdate(s)
char *s;
{
	register fd;
	long date;

	fd = open(s, 1);
	if(fd < 0) {
		fprintf(stderr, "can't reopen %s\n", s);
		return;
	}
	date = time((long *)0) + 5;
	lseek(fd, (long)((char *)&archdr.ar_date - (char *)&archdr) + 2, 0);
	write(fd, (char *)&date, 4);
	close(fd);
}

