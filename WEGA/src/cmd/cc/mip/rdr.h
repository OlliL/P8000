/*
 * rdr.h - the symbol table file of cparse as the passes that read it with the unit rdr see it (gopt: rdr 2.4, codgen:
 * rdr 2.5); included by their headers mfile2.h and mfile3. A guess at a shared header (the name too): the two have the
 * same symbol and the same ascii table size; neither binary has an SCCS line of it. See host/KNOWLEDGE.md.
 */
#define MAXASCII	0x1194	/* the ascii table (the names of the symbols) */

/* a symbol of the symbol table file (22 bytes there) */
struct sym {
	int	nameoff;	/* +0: offset of the name in the ascii table */
	unsigned stype;		/* +2 */
	char	sclass;		/* +4: 1 auto, 3 static, 4 register, 9 parameter ... */
	char	sbyte5;		/* +5: the register number of a register variable, the size of a field in bits (+4) */
	int	sx6;		/* +6 */
	long	soffset;	/* +8: bit offset in the frame, or the label number of a static */
	long	ssize;		/* +0xC: size in bits (a structure argument) */
	int	sflags;		/* +0x10 */
	int	sx12;		/* +0x12 */
	int	sx14;		/* +0x14: 0xFFFF */
};
