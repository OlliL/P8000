/*
 * cg2.h - the structures and sizes that the modules of codgen / scodgen share beyond cg.h (not for the tables).
 */
#define MAXASCII	0x1194
#define NDELAY	20
#define NTREE	500
#define NPOOL	87

struct rv { struct sym *sym; int rtag; int rreg; int rtype; int rcls; };
struct hoptab { int op; char *str; int tword; };
struct tmpent {
	int	off;		/* +0: frame offset of the temporary, negative: end of the table */
	char	done;		/* +2: not zero: the temporary is still needed */
	char	cls;		/* +3: class of the register */
	int flags;		/* +4: bit 15: the register still holds the value */
	int	reg;		/* +6: the register */
};
struct arg { int reg; NODE *node; };
#include "cgproto.h"
int inarena(const void *p);
int rd16(FILE *f);
long rd32(FILE *f);
