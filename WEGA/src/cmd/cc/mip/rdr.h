/*
 * rdr.h - the symbol table file of cparse as the passes that read it with the unit rdr see it (gopt: rdr 2.4, codgen:
 * rdr 2.5); included by their headers mfile2.h and mfile3. A guess at a shared header (the name too): the two have the
 * same symbol and the same ascii table size; neither binary has an SCCS line of it. See host/KNOWLEDGE.md.
 */
#define MAXASCII	0x1194	/* the ascii table (the names of the symbols) */

#define SYMSZ	22	/* the size of a symbol in the symbol table file */

/* a symbol of the symbol table file (SYMSZ bytes there) */
struct sym {
	int	nameoff;	/* +0: offset of the name in the ascii table */
	unsigned stype;		/* +2 */
	char	sclass;		/* +4: AUTO ... USTATIC */
	char	sbyte5;		/* +5: the register number of a register variable, the size of a field in bits (+4) */
	int	sx6;		/* +6 */
	long	soffset;	/* +8: bit offset in the frame, or the label number of a static */
	long	ssize;		/* +0xC: size in bits (a structure argument) */
	int	sflags;		/* +0x10: SFSYMNO, SFPMEM, SFGLOBAL, SFLPOPT, SFOPT */
	int	sx12;		/* +0x12 */
	int	sx14;		/* +0x14: 0xFFFF */
};

/* sflags: cparse (putsym) writes the symbol number in bits 4..15 and the four bits below as 0; the passes use them
   (the names are guessed; "global", "lpopt" and "optimize" are the words of the debug output of rdr and gopt) */
#define SFSYMNO(f)	(((f) >> 4) & 0xFFF)	/* the symbol number */
#define SFPMEM	010			/* codgen: a parameter that stays in memory (wrt3: not in a register at the entry) */
#define SFISPMEM(f)	((f) >> 3 & 1)
#define SFGLOBAL	04			/* rdr: a global symbol */
#define SFISGLOBAL(f)	((f) >> 2 & 1)
#define SFLPOPT	02			/* gopt: defined in the loop (lpinv2: "lpopt") */
#define SFISLPOPT(f)	((f) >> 1 & 1)
#define SFOPT	01			/* gopt: "optimize" (live2 setsymtable, reg2) */

/* the storage classes of sclass, as cparse writes them (cparse/mfile1) */
#define AUTO	1
#define EXTERN	2
#define STATIC	3
#define REGISTER	4
#define EXTDEF	5
#define LABEL	6
#define ULABEL	7
#define MOS	8
#define PARAM	9
#define STNAME	10
#define MOU	11
#define UNAME	12
#define TYPEDEF	13
#define FORTRAN	14
#define ENAME	15
#define MOE	16
#define UFORTRAN	17
#define USTATIC	18
