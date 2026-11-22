/*
 * rdr3.c - the readers: numbers and trees of the intermediate file, the symbol table file and the data file
 */
#include "mfile3"

extern int firstsect;
extern long gasciibytes;
extern int gnumbytes;
extern int gnumcount;
extern long gsymbytes;
extern long nasciibytes;
extern int nglob;
extern int nmembytes;
extern int nmems;
extern int nnumbytes;
extern int nnums;
extern long nsymbytes;
extern int nsyms;
extern int numtab[];
extern int stflag;
extern long symbase;
extern struct tagent tagtab[];
static char sccsid[] __attribute__((unused)) = "@[$]rdr3.c\t2.1  11/15/82 12:49:01 Copr. 1981 87wega3.2.";
/*
 * The symbol table file is written by cparse of the Z8000: big endian, 16 bit words, 32 bit longs, records of 22 bytes. It is read
 * field by field (the host struct sym has another size and byte order).
 */
int
rd16(FILE *f)
{
	int h = getc(f);

	return (short)(h << 8 | getc(f));
}

long
rd32(FILE *f)
{
	long h = rd16(f);

	return (int32_t)((uint32_t)(h & 0xFFFF) << 16 | (rd16(f) & 0xFFFF));
}

/* getw of the original libc: two bytes, the first one the high one; -1 at the end of the file */
static int
getw16(FILE *f)
{
	int h = getc(f);

	if (feof(f))
		return -1;
	return (short)(h << 8 | getc(f));
}

/* a record of the symbol table file (22 bytes, big endian) into a symbol */
static void
symdecode(struct sym *q, const unsigned char *r)
{
#define W16(o)	((short)(r[o] << 8 | r[(o) + 1]))
#define W32(o)	((int32_t)((uint32_t)r[o] << 24 | (uint32_t)r[(o) + 1] << 16 | (uint32_t)r[(o) + 2] << 8 | r[(o) + 3]))
	q->nameoff = W16(0);
	q->stype = (unsigned)W16(2) & 0xFFFF;
	q->sclass = (char)r[4];
	q->sbyte5 = (char)r[5];
	q->sx6 = W16(6);
	q->soffset = W32(8);
	q->ssize = W32(12);
	q->sflags = W16(16);
	q->sx12 = W16(18);
	q->sx14 = W16(20);
#undef W16
#undef W32
}

/* host: n symbol records of 22 bytes; the number of the records read */
static int
rdsyms22(struct sym *tab, int n, FILE *f)
{
	int x;
	unsigned char rec[22];

	for (x = 0; x < n; x++) {
		if (fread(rec, 22, 1, f) != 1)
			break;
		symdecode(&tab[x], rec);
	}
	return x;
}

#include "../mip/rdr.2.5"
