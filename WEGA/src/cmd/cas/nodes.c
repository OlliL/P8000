static char sccsid[] = "@[$]nodes.c\t1.1 03/16/83 19:03:25 - 87wega3.2";

/*
 * nodes - the node pool
 *
 * Nodes are taken from a pool of NNODES and handed back all at once when a
 * statement is finished: ifile.c counts the nodes it has written in nfreed.
 */
#include "cas.h"

unsigned nfreed = 0;
unsigned nalloc = 0;
NODE nodes[NNODES];

/* end of a statement: all nodes must have been written */
chknodes()
{
	if (nalloc == nfreed)
		nfreed = nalloc = 0;
	else
		error(2, "Nodes allocated at end of statement\n");
}

/* a node with one long value */
NODE *
mknode1(type, val)
char type;
long val;
{
	register NODE *np;

	if (nalloc >= NNODES)
		error(2, "Out of nodes\n");
	np = &nodes[nalloc++];
	np->n_type = type;
	np->n_next = 0;
	np->n_u.l = val;
	return(np);
}

/* type 6: a word and a node */
NODE *
mknode6(w, l)
int w;
long l;
{
	register NODE *np;

	if (nalloc >= NNODES)
		error(2, "Out of nodes\n");
	np = &nodes[nalloc++];
	np->n_type = 6;
	np->n_next = 0;
	np->n_u.wl.w = w;
	np->n_u.wl.l = l;
	return(np);
}

/* type 7: a word */
NODE *
mknode7(w)
int w;
{
	register NODE *np;

	if (nalloc >= NNODES)
		error(2, "Out of nodes\n");
	np = &nodes[nalloc++];
	np->n_type = 7;
	np->n_next = 0;
	np->n_u.pw.w = w;
	return(np);
}

/* a node with two long values */
NODE *
mknode2(type, l1, l2)
char type;
long l1, l2;
{
	register NODE *np;

	if (nalloc >= NNODES)
		error(2, "Out of nodes\n");
	np = &nodes[nalloc++];
	np->n_type = type;
	np->n_next = 0;
	np->n_u.ll.l1 = l1;
	np->n_u.ll.l2 = l2;
	return(np);
}

/* the character after a backslash in a string */
static char
escape(sp)
char **sp;
{
	char buf[3];

	switch (**sp) {
	case '%':
		(*sp)++;
		buf[0] = **sp;
		(*sp)++;
		buf[1] = **sp;
		buf[2] = '\0';
		return(xtol(buf, 16));
	case '0':
		return(0);
	case 'b':
		return(8);
	case 'f':
		return(12);
	case 'l':
	case 'n':
		return(10);
	case 'r':
		return(13);
	case 't':
		return(9);
	default:
		return(**sp);
	}
}

/* a string constant: a list of one character nodes, closed by the delimiter */
NODE *
mkstring(s)
char *s;
{
	register NODE *tail;
	register NODE *np;
	char c, delim;

	if (nalloc >= NNODES)
		error(2, "Out of nodes\n");
	np = &nodes[nalloc++];
	np->n_type = 0x16;
	delim = *s;
	s++;
	c = *s;
	if (c == '\\') {
		s++;
		c = escape(&s);
	}
	tail = mknode1(1, (long)c);
	locadv(1);
	np->n_next = tail;
	for (s++; *s != delim; s++) {
		c = *s;
		if (c == '\\') {
			s++;
			c = escape(&s);
		}
		tail->n_next = mknode1(1, (long)c);
		tail = tail->n_next;
		locadv(1);
	}
	np->n_u.l = (long)tail;
	return(np);
}

/* type 25: a binary operation on two expressions */
NODE *
mknode25(c1, c2, l1, l2)
char c1, c2;
long l1, l2;
{
	register NODE *np;

	if (nalloc >= NNODES)
		error(2, "Out of nodes\n");
	np = &nodes[nalloc++];
	np->n_type = 0x19;
	np->n_next = 0;
	np->n_u.cc.c1 = c1;
	np->n_u.cc.c2 = c2;
	np->n_u.cc.l2 = l2;
	np->n_u.cc.l1 = l1;
	return(np);
}
