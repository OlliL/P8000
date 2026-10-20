static char sccsid[] = "@[$]/as/as/ofile.c\t1.2 05/27/83 15:49:59 - 87wega3.2";

/*
 * ofile - the object file
 *
 * The header and the segment table are made from the location counters at the end of
 * pass 1; the symbol file of pass 1 is turned into the symbol table (s_nlist entries,
 * names longer than 8 characters take more than one), and at the end the relocation
 * information and the symbols are appended to the object file.
 */
#include "cas.h"
#include <s.out.h>

extern FILE *osymf;
extern char *outname, *rfname, *ofname, *sfname;
extern NODE *fn8358();			/* read an expression from the symbol file (getops.c) */
extern char *calloc();
extern unsigned char sgetb();
extern int sgetw();

int imoff;				/* size of header and segment table: where the image starts */
char stabname[128];
struct s_exec hdr;
struct seekent *seektab;
struct {
	long	v;
	char	t;
	char	s;
	char	name[128];
} nl;
struct segt *segtab;

/* the header and the segment table from the sizes of the sections */
ohdrinit()
{
	register int i;
	register int seeksz;
	register long imsize, bss;
	register struct segt *p;

	if (segmode)
		hdr.s_magic = 0xE607;
	else
		hdr.s_magic = 0xE711;
	imsize = 0;
	bss = 0;
	hdr.s_segt = nsect << 4;
	segtab = (struct segt *)calloc(1, hdr.s_segt);
	if (segtab == 0) {
		perror("segment table");
		done(-1);
	}
	p = segtab;
	seeksz = nsect << 3;
	seektab = (struct seekent *)calloc(1, seeksz);
	if (seektab == 0) {
		perror("seek offset table");
		done(-1);
	}
	for (i = 0; i < nsect; i++) {
		if (i != 0) {
			seektab[i].sk_off += seektab[i - 1].sk_end;
			seektab[i].sk_end = seektab[i].sk_off;
		}
		p->sg_segno = i;
		imsize += lctab[i][0];
		seektab[i].sk_end += lctab[i][0];
		if (lctab[i][0] != 0) {
			p->sg_code = lctab[i][0];
			p->sg_atr |= SG_CODE;
		}
		lctab[i][0] = 0;
		imsize += lctab[i][1];
		if (i + 1 < nsect)
			seektab[i + 1].sk_off = lctab[i][1];
		if (lctab[i][1] != 0) {
			p->sg_data = lctab[i][1];
			p->sg_atr |= SG_DATA;
		}
		lctab[i][1] = 0;
		bss += lctab[i][2];
		if (lctab[i][2] != 0) {
			p->sg_bss = lctab[i][2];
			p->sg_atr |= SG_BSS;
		}
		lctab[i][2] = 0;
		p++;
	}
	hdr.s_imsize = imsize;
	hdr.s_bss = bss;
	imoff = hdr.s_segt + 0x18;
}

/* write the header and the segment table */
ohdrput()
{
	if (fseek(outf, 0L, 0) == -1) {
		perror(outname);
		done(-1);
	}
	if (fwrite(&hdr, 0x18, 1, outf) != 1) {
		perror(outname);
		done(-1);
	}
	if (fwrite(segtab, hdr.s_segt, 1, outf) != 1) {
		perror(outname);
		done(-1);
	}
}

/* the type of a symbol table entry */
symtype(flags, ip, n)
unsigned char flags;
struct sym *ip;
int n;
{
	unsigned char t;

	if ((flags & 6) == 0) {
		ip->s_2 = 0L;
		return (0x20);
	}
	if ((flags & 1) == 0)
		t = 1;
	else {
		switch (flags & 6) {
		case 2:
		case 4:
		case 6:
			switch ((char)ip->s_0) {
			case 0:
				t = 2;
				break;
			case 1:
				t = 3;
				break;
			case 2:
				t = 4;
				break;
			}
		}
	}
	if ((flags & 6) == 2)
		t |= 0x20;
	return (t);
}

/* the symbol file of pass 1 into the symbol table of the object file */
osymout()
{
	register int idx;
	register int n;
	register long v;
	register struct sym *sp;
	unsigned char tag;
	unsigned char flags;
	char t;
	char sect;
	struct sym *ip;

	if (fseek(sfile, 0L, 0) == -1) {
		perror(sfname);
		done(-1);
	}
	n = 0;
	while ((tag = sgetb()) != 0xF0) {
		switch (tag) {
		case 0x14:
		case 0x15:
			idx = sgetw();
			sp = stabp(idx);
			if (tag == 0x15 && uflag && sp->s_type == 0) {
				;
			} else if (tag != 0x14 || sp->s_type != 1)
				continue;
			if (tag == 0x15) {
				setsym(idx, (unsigned char)1, 0);
				sp = stabp(idx);
				ip = itabp(sp->s_8);
				ip->s_2 = 0L;
				ip->s_0 = cursub;
			}
			sp = stabp(idx);
			flags = sp->s_7;
			if ((flags & 6) == 6)
				continue;
			if (!dflag && (flags & 6) == 4)
				continue;
			ip = itabp(sp->s_8);
			if ((flags & 1) == 1)
				v = ip->s_2 | ((long)((unsigned)ip->s_0 >> 8) << 24);
			else
				v = ip->s_2;
			sect = (unsigned)ip->s_0 >> 8;
			t = symtype(flags, ip, n);
			sp = stabp(idx);
			n += nlwrite(v, t, sect, sp->s_2);
			break;
		case 0x2a:
		case 0x2b:
			n += stabout(tag);
			break;
		}
	}
	hdr.s_syms = n * 14;
}

/* a .stab record: the name, then the value, the section and the type */
stabout(tag)
unsigned char tag;
{
	register NODE *np;
	register char *p;
	register long v;
	char type;
	char sect;

	p = stabname;
	for (;;) {
		*p = sgetb();
		if (*p == 0)
			break;
		p++;
	}
	if (tag == 0x2a) {
		np = fn8358();
		if (exprtype(np) != 3)
			error(2, "Stab field must be constant\n");
		else {
			v = np->n_u.l << 8;
			nfreed++;
		}
		np = fn8358();
		if (exprtype(np) != 3)
			error(2, "Stab field must be constant\n");
		else {
			v |= np->n_u.l;
			nfreed++;
		}
	} else if (tag == 0x2b) {
		np = fn8358();
		switch (np->n_type) {
		case 3:
			v = np->n_u.l;
			break;
		case 7:
			v = symval(np->n_u.pw.w);
			break;
		case 0x19:
			v = exprval(np);
			break;
		}
		nfreed++;
	}
	np = fn8358();
	if (exprtype(np) != 3)
		error(2, "Stab field must be constant\n");
	else {
		sect = np->n_u.l;
		nfreed++;
	}
	np = fn8358();
	if (exprtype(np) != 3)
		error(2, "Stab field must be constant\n");
	else {
		type = np->n_u.l;
		nfreed++;
	}
	chknodes();
	if (dflag)
		return (nlwrite(v, type, sect, stabname));
	return (0);
}

/* one symbol: a s_nlist entry, and more entries for a long name */
nlwrite(v, t, sect, name)
long v;
char t;
char sect;
char *name;
{
	register int n;
	register unsigned len;
	register unsigned i;
	char *dest;

	nl.v = v;
	nl.t = t;
	nl.s = sect;
	for (i = 0; i < 128; i++)
		nl.name[i] = 0;
	len = strlen(name);
	if (len <= 8) {
		n = 1;
		dest = nl.name;
	} else {
		n = (len - 7) / 14 + 2;
		nl.name[0] = (len + 1) | 0x80;
		dest = nl.name + 1;
	}
	strncpy(dest, name, len);
	if (fwrite(&nl, 14, n, osymf) != n) {
		perror(ofname);
		done(-1);
	}
	return (n);
}

/* append the relocation information and the symbols to the object file */
ofinish()
{
	int w;

	if (fseek(relf, 0L, 0) == -1) {
		perror(rfname);
		done(-1);
	}
	if (fseek(outf, 0L, 2) == -1) {
		perror(outname);
		done(-1);
	}
	if (fseek(osymf, 0L, 0) == -1) {
		perror(ofname);
		done(-1);
	}
	while (fread(&w, 2, 1, relf)) {
		if (fwrite(&w, 2, 1, outf) != 1) {
			perror(outname);
			done(-1);
		}
	}
	while (fread(&w, 2, 1, osymf)) {
		if (fwrite(&w, 2, 1, outf) != 1) {
			perror(outname);
			done(-1);
		}
	}
}

/* the values of the symbols: data and bss follow the code of their section */
symadjust()
{
	register struct sym *sp;
	register struct sym *xp;
	register struct sym *ip;
	unsigned i;
	int sect, area;

	for (i = 2; i < stoff; ) {
		sp = (struct sym *)0x87000030L;
		sp = (struct sym *)((char *)sp + i);
		if (sp->s_type == 1 && (sp->s_7 & 1) == 1) {
			ip = itabp(sp->s_8);
			area = ip->s_0 & 0xFF;
			sect = (unsigned)ip->s_0 >> 8;
			switch (area) {
			case 1:
				if (segmode != 0)
					ip->s_2 += lctab[sect][0];
				break;
			case 2:
				ip->s_2 += lctab[sect][1];
				if (segmode != 0)
					ip->s_2 += lctab[sect][0];
				break;
			}
		}
		if ((sp->s_7 & 6) == 6 || (sp->s_7 & 8) == 8)
			i += 16;
		else
			i += 10;
	}
}
