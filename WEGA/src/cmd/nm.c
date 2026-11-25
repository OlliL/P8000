static char sccsid[] = "@[$]nm.c\t2.6 11/18/83 13:50:10 - 87wega3.2";

/*
**	print symbol tables for
**	object or archive files
**
**	nm [-glnoprsuv] [name ...]
**
**	WEGA: nm.c of System III rewritten for s.out: getopt, long names (a name table), -v (the VDB types),
**	-l (the line table), the segmented formats, the byte-swapped files of goodmagic (swap_flg).
**	The names of the functions are guesses.
*/

#include	<ar.h>
#include	<s.out.h>
#include	<stdio.h>
#include	<ctype.h>

#define	SELECT	arch_flg ? arp.ar_name : fname
#define	NAMES	8000

char	names[NAMES + 20];
unsigned namoff = 0;

char	numsort_flg;
char	undef_flg;
int	revsort_flg = 1;
char	globl_flg;
char	nosort_flg;
char	arch_flg;
char	prep_flg;
char	size_flg;
char	swap_flg;	/* sic: the int of goodmagic; only its high byte is tested */
char	v_flg;
char	l_flg;
char	sep_id;

struct	ar_hdr	arp;
struct	s_exec	exp;

FILE	*fi;

long	off;
long	ftell();

char	*malloc();
char	*realloc();

char	*progname;
char	*fname;
struct	s_nlist	sym;
char	symrest[LONGNAME + 1];	/* the entries that follow sym for a long name (name guessed) */

extern	int optind;
char	*basename();
unsigned datasize();

main(argc, argv)
char **argv;
{
	int	narg;
	int	optc;
	int	compare();

	progname = basename(argv[0]);
	while ((optc = getopt(argc, argv, "ngurposvl")) != EOF)
		switch (optc) {
		case 'n':		/* sort numerically */
			numsort_flg++;
			break;

		case 'g':		/* globl symbols only */
			globl_flg++;
			break;

		case 'u':		/* undefined symbols only */
			undef_flg++;
			break;

		case 'r':		/* sort in reverse order */
			revsort_flg = -1;
			break;

		case 'p':		/* don't sort -- symbol table order */
			nosort_flg++;
			break;

		case 'o':		/* prepend a name to each line */
			prep_flg++;
			break;

		case 's':
			size_flg++;
			break;

		case 'v':
			v_flg++;
			break;

		case 'l':
			l_flg++;
			break;

		case '?':		/* oops */
			fprintf(stderr, "usage: nm [-glnoprsuv] [file ...]\n");
			exit(2);
		}
	if (size_flg) {
		numsort_flg++;
		globl_flg++;
		undef_flg = 0;
		nosort_flg = 0;
	}
	if (v_flg) {
		numsort_flg = 0;
		globl_flg = 0;
		undef_flg = 0;
		revsort_flg = 1;
		nosort_flg++;
		size_flg = 0;
	}
	if (argc == optind) {
		argc = optind + 1;
		argv[optind] = swap_flg ? "s.out" : "a.out";
	}
	narg = argc - optind;
	while (optind < argc) {
		fname = argv[optind++];
		fi = fopen(fname, "r");
		if (fi == NULL) {
			fprintf(stderr, "nm: cannot open %s\n", fname);
			continue;
		}
		off = 2L;
		fread(&exp, sizeof(exp.s_magic), 1, fi);	/* try for an archive magic # */
		if (exp.s_magic == ARMAG)
			arch_flg++;
		else if (!goodmagic(exp.s_magic)) {
			fprintf(stderr, "nm: %s-- bad format\n", fname);
			fclose(fi);
			continue;
		}
		if ((exp.s_magic == S_MAGIC3 || exp.s_magic == N_MAGIC3 || exp.s_magic == X_MAGIC3) && size_flg)
			sep_id++;
		fseek(fi, 0L, 0);
		if (arch_flg) {
			nextel(fi);
			if (narg > 1)
				printf("\n%s:\n", fname);
		}
		do {
			long o;
			register i, n, c;
			char nlen;
			struct s_nlist *symp;
			struct s_nlist *end;
			struct s_nlist *sp;
			unsigned dsize;

			symp = NULL;
			fread(&exp, sizeof(struct s_exec), 1, fi);
			if (!goodmagic(exp.s_magic))	/* archive element not in  */
				continue;		/* proper format - skip it */
			if (swap_flg)
				swaphdr(&exp);
			o = exp.s_imsize;
			if ((exp.s_flag & SF_STRIP) == 0)
				o *= 2;
			o += exp.s_segt;
			fseek(fi, o, 1);
			n = exp.s_syms / sizeof(struct s_nlist);
			if (n == 0) {
				fprintf(stderr, "nm: %s-- no name list\n", SELECT);
				continue;
			}
			if ((exp.s_magic == S_MAGIC1 || exp.s_magic == S_MAGIC3 || exp.s_magic == S_MAGIC4) && size_flg) {
				fprintf(stderr, "nm: -s option does not work on segmented object files\n");
				continue;
			}
			i = 0;
			while (--n >= 0) {
				if (fread(&sym, sizeof(sym), 1, fi) != 1) {
					fprintf(stderr, "Warning:Premature EOF. %s: Missing %d symbols\n", fname, n + 1);
					break;
				}
				if (swap_flg)
					swab(&sym, &sym, 4);
				nlen = SHORTNAME;
				if (sym.sn_name[0] & LONGBIT) {
					nlen = sym.sn_name[0] & LONGMASK;
					end = (struct s_nlist *)&sym.sn_name[nlen];
					for (sp = &sym; ++sp < end; ) {
						fread(sp, sizeof(struct s_nlist), 1, fi);
						n--;
					}
					nlen--;
				}
				if (globl_flg && (sym.sn_type & N_EXT) == 0)
					continue;
				if (symp == NULL)
					symp = (struct s_nlist *)malloc(sizeof(struct s_nlist));
				else
					symp = (struct s_nlist *)realloc(symp, (i+1)*sizeof(struct s_nlist));
				if (symp == NULL) {
					fprintf(stderr, "nm: out of memory on %s\n", fname);
					exit(2);
				}
				if (nlen > SHORTNAME)
					savename(nlen);
				symp[i++] = sym;
			}
			if (nosort_flg == 0) {
				qsort(symp, i, sizeof(struct s_nlist), compare);
				if (size_flg) {
					dsize = datasize(fname);
					dsize += exp.s_bss;
					for (n = 0; n < i; n++) {
						if ((c = symp[n+1].sn_value - symp[n].sn_value) < 0)
							c = -c;
						if (sep_id && (symp[n+1].sn_type & N_CODE)) {
							c = dsize - symp[n].sn_value;
							sep_id = dsize = 0;
						}
						if (n == i - 1)
							c = exp.s_imsize + exp.s_bss - symp[n].sn_value;
						symp[n].sn_value = c;
					}
					qsort(symp, i, sizeof(struct s_nlist), compare);
				}
			}
			if ((arch_flg || narg > 1) && prep_flg == 0)
				printf("\n%s:\n", SELECT);
			for (n = 0; n < i; n++) {
				if (prep_flg) {
					if (arch_flg)
						printf("%s:", fname);
					printf("%s:", SELECT);
				}
				switch (symp[n].sn_type & N_TYPE) {
				case N_UNDF:
					c = 'u';
					if (symp[n].sn_value)
						c = 'c';
					break;

				case N_ABS:
					c = 'a';
					break;

				case N_CODE:
					c = 't';
					break;

				case N_DATA:
					c = 'd';
					break;

				case N_BSS:
					c = 'b';
					break;

				case N_FN:
					c = 'f';
					break;

				case N_SN:
					c = 's';
					break;

				case N_SIDENT:
					if (!v_flg)
						continue;
					c = 'n';
					break;

				case N_LENGTH:
					if (!v_flg)
						continue;
					c = 'h';
					break;

				case N_MEMBER:
					if (!v_flg)
						continue;
					c = 'm';
					break;

				case N_BMEMBER:
					if (!v_flg)
						continue;
					c = 'x';
					break;

				case N_RBRACKET:
					if (!v_flg)
						continue;
					c = '}';
					break;

				case N_LBRACKET:
					if (!v_flg)
						continue;
					c = '{';
					break;

				case N_ENTRY:
					if (!v_flg)
						continue;
					c = 'e';
					break;

				case N_SFN:
					if (!v_flg)
						continue;
					c = '+';
					break;

				case N_IFN:
					if (!v_flg)
						continue;
					c = '#';
					break;

				case N_REGPARM:
					if (!v_flg)
						continue;
					c = 'p';
					break;

				case N_ABSPARM:
					if (!v_flg)
						continue;
					c = '@';
					break;

				case N_PREG:
					if (!v_flg)
						continue;
					c = '&';
					break;

				case N_SPREG:
					if (!v_flg)
						continue;
					c = '$';
					break;

				case N_AREG:
					if (!v_flg)
						continue;
					c = '*';
					break;

				case N_AABS:
					if (!v_flg)
						continue;
					c = 'q';
					break;

				case N_RABS:
					if (!v_flg)
						continue;
					c = '^';
					break;

				case N_ISTATIC:
					if (!v_flg)
						continue;
					c = 'i';
					break;

				case N_LSTATIC:
					if (!v_flg)
						continue;
					c = 'l';
					break;

				case N_ABSS:
					if (!v_flg)
						continue;
					c = 'b';
					break;

				case N_GLOBAL:
					if (!v_flg)
						continue;
					c = 'd';
					break;
				}
				if (undef_flg && c != 'u')
					continue;
				if (symp[n].sn_type & N_EXT)
					c = toupper(c);
				if (!undef_flg) {
					if (c == 'u' || c == 'U')
						printf("\t");
					else if (symp[n].sn_value & 0xffff0000)
						printf("%08lx", symp[n].sn_value);
					else
						printf("    %04x", (int)symp[n].sn_value);
					printf(" %c ", c);
				}
				putname(symp[n].sn_name);
				printf("\n");
			}
			if (symp)
				free((char *)symp);
			namoff = 0;
		} while (arch_flg && nextel(fi));
		if (l_flg) {
			register i;
			struct v_lines line;

			if (exp.s_lines == 0)
				fprintf(stderr, "nm:%s -- no line table\n", fname);
			else for (i = 0; i < exp.s_lines; i++) {
				if (fread(&line, sizeof(line), 1, fi) != 1) {
					fprintf(stderr, "WARNING: Premature EOF. %s Missing %d line entries\n", fname, exp.s_lines - i + 1);
					break;
				}
				printf(line.l_addr & 0xffff0000 ? "%08lx" : "    %04x", line.l_addr);
				printf("\t %u\n", line.l_num);
			}
		}
		fclose(fi);
	}
	exit(0);
}

compare(p1, p2)
struct s_nlist *p1, *p2;
{
	register i;
	register char *cp1, *cp2;
	char l1, l2;
	int x;

	if (sep_id) {
		if ((p1->sn_type & N_CODE) && (p2->sn_type & N_CODE) == 0)
			return(revsort_flg);
		if ((p2->sn_type & N_CODE) && (p1->sn_type & N_CODE) == 0)
			return(-revsort_flg);
	}
	if (numsort_flg) {
		if (p1->sn_value > p2->sn_value)
			return(revsort_flg);
		if (p1->sn_value < p2->sn_value)
			return(-revsort_flg);
	}
	l1 = l2 = SHORTNAME;
	cp1 = p1->sn_name;
	cp2 = p2->sn_name;
	if (*cp1 & LONGBIT) {
		l1 = (*cp1 & LONGMASK) - 1;
		cp1++;
		x = *cp1++;
		x = (x & 0377) + (*cp1 << 8);
		cp1 = &names[x];
	}
	if (*cp2 & LONGBIT) {
		l2 = (*cp2 & LONGMASK) - 1;
		cp2++;
		x = *cp2++;
		x = (x & 0377) + (*cp2 << 8);
		cp2 = &names[x];
	}
	for (i = 0; i < l1 && i < l2; i++)
		if (*cp1++ != *cp2++) {
			if (*--cp1 > *--cp2)
				return(revsort_flg);
			else
				return(-revsort_flg);
		}
	if (l1 == l2)
		return(0);
	if (l1 > l2)
		return(revsort_flg);
	return(-revsort_flg);
}

/* a long name of sym to the name table; sym gets its offset there */
savename(len)
char len;
{
	register char *to, *from;
	char n;

	n = len;
	to = &names[namoff];
	from = &sym.sn_name[1];
	while (n--)
		*to++ = *from++;
	sym.sn_name[1] = namoff & 0377;
	sym.sn_name[2] = namoff >> 8;
	namoff += len;
	if (namoff > NAMES) {
		fprintf(stderr, "%s: too many long names\n", progname, fname);	/* sic: one argument too many */
		exit(2);
	}
}

nextel(af)
FILE *af;
{
	register r;

	fseek(af, off, 0);
	r = fread((char *)&arp, sizeof(struct ar_hdr), 1, af);  /* read archive header */
	if (r <= 0)
		return(0);
	if (arp.ar_size & 1)
		++arp.ar_size;
	off = ftell(af) + arp.ar_size;	/* offset to next element */
	return(1);
}

/* the header of a byte-swapped file */
swaphdr(hp)
char *hp;
{
	register char *p;
	register i, t;

	p = hp;
	i = sizeof(struct s_exec) / 2;
	do {
		t = *p;
		*p = p[1];
		p[1] = t;
		p += 2;
	} while (--i);
}

char *
basename(p)
register char *p;
{
	register char *s;

	s = p;
	while (*s)
		if (*s++ == '/' && *s)
			p = s;
	return(p);
}

/* a name of the symbol table: eight characters or a long one of the name table */
putname(cp)
char *cp;
{
	register char *p;
	char n;
	int x;
	char trunc;

	p = cp;
	trunc = 0;
	n = *p & LONGMASK;
	if (n != *p) {
		n--;
		if (n > 44) {
			trunc++;
			n = 40;
		}
		p++;
		x = *p++;
		x = (x & 0377) + (*p++ << 8);
		p = &names[x];
	} else
		n = SHORTNAME;
	while (n--) {
		if (*p == 0)
			break;
		putchar(*p);
		p++;
	}
	if (trunc)
		printf("...");
}

/* the size of the data of the segments of a file (sic: the sum is not set to 0 first) */
unsigned
datasize(name)
char *name;
{
	struct segt seg;
	FILE *f;
	int i;
	unsigned sum;

	f = fopen(name, "r");
	fseek(f, (long)sizeof(struct s_exec), 0);
	for (i = 1; i <= exp.s_segt / sizeof(struct segt); i++) {
		fread(&seg, sizeof(struct segt), 1, f);
		sum += seg.sg_data;
	}
	fclose(f);
	return(sum);
}
