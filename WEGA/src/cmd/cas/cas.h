/*
 * cas.h - declarations shared by the modules of the cas assembler
 *
 * Reconstructed from the WEGA binary /bin/cas; the names are ours (the binary
 * is stripped), the layout of the structures follows the binary.
 */

/* expression and statement nodes, allocated by nodes.c (16 bytes) */
typedef struct node NODE;
struct node {
	unsigned char n_type;		/* 0: node type */
	char	n_pad;			/* 1 */
	NODE	*n_next;		/* 2 */
	union {				/* 6 */
		long	l;
		struct { int w; long l; } wl;
		struct { int pad; int w; } pw;
		struct { long l1; long l2; } ll;
		struct { unsigned char c1; unsigned char c2; long l1; long l2; } cc;
	} n_u;
};

#define NNODES	100			/* size of the node pool */

extern NODE	nodes[NNODES];
extern unsigned	nalloc;			/* nodes handed out */
extern unsigned	nfreed;			/* nodes written to the intermediate file */
