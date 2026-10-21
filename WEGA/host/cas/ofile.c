/*
 * ofile - the object file
 *
 * The header and the segment table are made from the location counters at the end of
 * pass 1; the symbol file of pass 1 is turned into the symbol table (s_nlist entries,
 * names longer than 8 characters take more than one), and at the end the relocation
 * information and the symbols are appended to the object file. The structures of
 * <s.out.h> are written field by field, high byte first.
 */
#include "cas.h"

#define S_EXECSZ	0x18		/* size of struct s_exec */
#define SEGTSZ		16		/* size of struct segt */
#define NLISTSZ		14		/* size of struct s_nlist */
#define SG_CODE		0x0001
#define SG_DATA		0x0002
#define SG_BSS		0x0004

FILE *osymf;

int imoff;				/* size of header and segment table: where the image starts */
struct seekent *seektab;

static struct {				/* struct s_exec */
	uint16_t s_magic;
	uint32_t s_imsize;
	uint32_t s_bss;
	uint16_t s_segt;
	uint16_t s_syms;
} hdr;

struct segent {				/* struct segt */
	uint8_t	sg_segno;
	uint16_t sg_code, sg_data, sg_bss, sg_atr;
};
static struct segent *segtab;

static void
put16(uint8_t *p, unsigned v)
{
	p[0] = v >> 8;
	p[1] = v;
}

static void
put32(uint8_t *p, uint32_t v)
{
	put16(p, v >> 16);
	put16(p + 2, v);
}

/* the header and the segment table from the sizes of the sections */
void
ohdrinit(void)
{
	int i;
	int32_t imsize, bss;
	struct segent *p;

	if (segmode)
		hdr.s_magic = 0xE607;
	else
		hdr.s_magic = 0xE711;
	imsize = 0;
	bss = 0;
	hdr.s_segt = nsect << 4;
	segtab = calloc(nsect, sizeof *segtab);
	seektab = calloc(nsect, sizeof *seektab);
	if (segtab == 0 || seektab == 0) {
		perror("segment table");
		done(-1);
	}
	p = segtab;
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
	imoff = hdr.s_segt + S_EXECSZ;
}

/* write the header and the segment table */
void
ohdrput(void)
{
	uint8_t b[S_EXECSZ], s[SEGTSZ];
	int i;

	if (fseek(outf, 0L, SEEK_SET) == -1) {
		perror(outname);
		done(-1);
	}
	memset(b, 0, sizeof b);
	put16(b, hdr.s_magic);
	put32(b + 2, hdr.s_imsize);
	put32(b + 6, hdr.s_bss);
	put16(b + 10, hdr.s_segt);
	put16(b + 12, hdr.s_syms);
	if (fwrite(b, S_EXECSZ, 1, outf) != 1) {
		perror(outname);
		done(-1);
	}
	for (i = 0; i < nsect; i++) {
		memset(s, 0, sizeof s);
		s[0] = segtab[i].sg_segno;
		put16(s + 4, segtab[i].sg_code);
		put16(s + 6, segtab[i].sg_data);
		put16(s + 8, segtab[i].sg_bss);
		put16(s + 10, segtab[i].sg_atr);
		if (fwrite(s, SEGTSZ, 1, outf) != 1) {
			perror(outname);
			done(-1);
		}
	}
}

/* the type of a symbol table entry */
static int
symtype(int flags, struct sym *ip, int n)
{
	int t;

	t = 0;
	if ((flags & 6) == 0) {
		ip->s_2 = (uint16_t)n;
		return (0x20);
	}
	if ((flags & 1) == 0)
		t = 1;
	else {
		switch (flags & 6) {
		case 2:
		case 4:
		case 6:
			switch ((uint8_t)ip->s_0) {
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

/* one symbol: a s_nlist entry, and more entries for a long name; returns their number */
static int
nlwrite(int32_t v, int t, int sect, const char *name)
{
	uint8_t nl[NLISTSZ * 12];
	unsigned len;
	int n;

	memset(nl, 0, sizeof nl);
	put32(nl, v);
	nl[4] = t;
	nl[5] = sect;
	len = strlen(name);
	if (len > 127)
		len = 127;
	if (len <= 8) {
		n = 1;
		memcpy(nl + 6, name, len);
	} else {
		n = (len - 7) / 14 + 2;
		nl[6] = (len + 1) | 0x80;
		memcpy(nl + 7, name, len);
	}
	if (fwrite(nl, NLISTSZ, n, osymf) != (size_t)n) {
		perror(ofname);
		done(-1);
	}
	return (n);
}

/* a .stab record: the name, then the value, the section and the type */
static int
stabout(int tag)
{
	NODE *np;
	char *p;
	char stabname[128];
	int32_t v;
	int type, sect;

	v = type = sect = 0;
	p = stabname;
	for (;;) {
		*p = sgetb();
		if (*p == 0)
			break;
		if (p < stabname + sizeof stabname - 1)
			p++;
	}
	if (tag == 0x2a) {
		np = sgetnode();
		if (exprtype(np) != 3)
			error(2, "Stab field must be constant\n");
		else {
			v = (uint32_t)np->n_val << 8;
			nfreed++;
		}
		np = sgetnode();
		if (exprtype(np) != 3)
			error(2, "Stab field must be constant\n");
		else {
			v |= np->n_val;
			nfreed++;
		}
	} else if (tag == 0x2b) {
		np = sgetnode();
		switch (np->n_type) {
		case 3:
			v = np->n_val;
			break;
		case 7:
			v = symval(np->n_w);
			break;
		case 0x19:
			v = exprval(np);
			break;
		}
		nfreed++;
	}
	np = sgetnode();
	if (exprtype(np) != 3)
		error(2, "Stab field must be constant\n");
	else {
		sect = (uint8_t)np->n_val;
		nfreed++;
	}
	np = sgetnode();
	if (exprtype(np) != 3)
		error(2, "Stab field must be constant\n");
	else {
		type = (uint8_t)np->n_val;
		nfreed++;
	}
	chknodes();
	if (dflag)
		return (nlwrite(v, type, sect, stabname));
	return (0);
}

/* the symbol file of pass 1 into the symbol table of the object file */
void
osymout(void)
{
	int idx, n, tag, flags, t, sect;
	int32_t v;
	struct sym *sp, *ip;

	if (fseek(sfile, 0L, SEEK_SET) == -1) {
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
				setsym(idx, 1, 0);
				sp = stabp(idx);
				ip = itabp(sp->s_8);
				ip->s_2 = 0;
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
				v = (uint32_t)ip->s_2 | ((uint32_t)((uint16_t)ip->s_0 >> 8) << 24);
			else
				v = ip->s_2;
			sect = (uint16_t)ip->s_0 >> 8;
			t = symtype(flags, ip, n);
			sp = stabp(idx);
			n += nlwrite(v, t, sect, symname(sp));
			break;
		case 0x2a:
		case 0x2b:
			n += stabout(tag);
			break;
		}
	}
	hdr.s_syms = n * NLISTSZ;
}

/* append a file to the object file */
static void
append(FILE *f, const char *name)
{
	int c;

	if (fseek(f, 0L, SEEK_SET) == -1) {
		perror(name);
		done(-1);
	}
	while ((c = getc(f)) != EOF) {
		if (putc(c, outf) == EOF) {
			perror(outname);
			done(-1);
		}
	}
}

/* append the relocation information and the symbols to the object file */
void
ofinish(void)
{
	if (fseek(outf, 0L, SEEK_END) == -1) {
		perror(outname);
		done(-1);
	}
	append(relf, rfname);
	append(osymf, ofname);
}

/* the values of the symbols: data and bss follow the code of their section */
void
symadjust(void)
{
	struct sym *sp, *ip;
	unsigned i;
	int sect, area;

	for (i = 2; i < stoff; ) {
		sp = stabp(i);
		if (sp->s_type == 1 && (sp->s_7 & 1) == 1) {
			ip = itabp(sp->s_8);
			area = ip->s_0 & 0xFF;
			sect = (uint16_t)ip->s_0 >> 8;
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
