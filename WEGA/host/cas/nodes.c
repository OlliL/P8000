/*
 * nodes - the node pool
 *
 * Nodes are taken from a pool of NNODES and handed back all at once when a
 * statement is finished: ifile.c counts the nodes it has written in nfreed.
 * When the pool is full the original writes behind it; here the statement gets
 * a node from the heap after the message "Out of nodes" (and nalloc goes on
 * counting, as there), which is given back when the pool is emptied.
 */
#include "cas.h"

unsigned nfreed = 0;
unsigned nalloc = 0;
NODE nodes[NNODES];
static NODE *overflow;			/* the nodes beyond the pool */

/* a node outside the pool; it is given back when the pool is emptied */
static NODE *
extranode(void)
{
	NODE *np;

	np = calloc(1, sizeof *np);
	if (np == 0) {
		perror("nodes");
		done(-1);
	}
	np->n_link = overflow;
	overflow = np;
	return (np);
}

/* a new node, cleared */
NODE *
newnode(void)
{
	NODE *np;

	if (nalloc >= NNODES) {
		error(2, "Out of nodes\n");
		nalloc++;			/* the original goes on counting (and writes behind the pool) */
		return (extranode());
	}
	np = &nodes[nalloc++];
	memset(np, 0, sizeof *np);
	return (np);
}

/*
 * The node that a function returns after it has reported an error: the constant 0. It does not
 * count as a node of the statement (the original returns what the registers hold).
 */
NODE *
errnode(void)
{
	NODE *np;

	np = extranode();
	np->n_type = 3;
	return (np);
}

/* end of a statement: all nodes must have been written */
void
chknodes(void)
{
	if (nalloc == nfreed) {
		nfreed = nalloc = 0;
		while (overflow) {
			NODE *np = overflow;

			overflow = np->n_link;
			free(np);
		}
	} else
		error(2, "Nodes allocated at end of statement\n");
}

/* a node with one long value */
NODE *
mknode1(int type, int32_t val)
{
	NODE *np;

	np = newnode();
	np->n_type = type;
	np->n_val = val;
	return (np);
}

/* type 6: a word and a node */
NODE *
mknode6(int w, NODE *a)
{
	NODE *np;

	np = newnode();
	np->n_type = 6;
	np->n_w = w;
	np->n_a = a;
	return (np);
}

/* type 7: a word */
NODE *
mknode7(int w)
{
	NODE *np;

	np = newnode();
	np->n_type = 7;
	np->n_w = w;
	return (np);
}

/* a node with two operands */
NODE *
mknode2(int type, NODE *a, NODE *b)
{
	NODE *np;

	np = newnode();
	np->n_type = type;
	np->n_a = a;
	np->n_b = b;
	return (np);
}

/* the character after a backslash in a string; *sp is left on the last character used */
static int
escape(const char **sp)
{
	char buf[3];

	switch (**sp) {
	case '%':
		buf[0] = buf[1] = buf[2] = '\0';
		if (*++*sp) {
			buf[0] = **sp;
			if (*++*sp)
				buf[1] = **sp;
		}
		return ((unsigned char)xtol(buf, 16));
	case '0':
		return (0);
	case 'b':
		return (8);
	case 'f':
		return (12);
	case 'l':
	case 'n':
		return (10);
	case 'r':
		return (13);
	case 't':
		return (9);
	default:
		return ((unsigned char)**sp);
	}
}

/*
 * The scan of a string went on behind its end: the original takes characters from the memory that
 * follows until it finds a quote there, and makes a node of each one, so the pool is full
 * and the statement ends with "Out of nodes". The pool is not emptied by the end of that
 * statement (see newnode), the next one fails too.
 */
static void
runout(void)
{
	while (nalloc <= NNODES) {
		mknode1(1, 0);
		locadv(1);
	}
}

/* a string constant: a list of one character nodes, closed by the delimiter */
NODE *
mkstring(const char *s)
{
	NODE *tail, *np;
	int c, delim;

	np = newnode();
	np->n_type = 0x16;
	delim = *s;
	s++;
	c = (unsigned char)*s;
	if (c == '\\') {
		s++;
		c = escape(&s);
	}
	tail = mknode1(1, c);
	locadv(1);
	np->n_next = tail;
	if (*s == 0) {				/* the escape of the first character reached the end of the text */
		runout();
		np->n_a = tail;
		return (np);
	}
	/*
	 * The closing delimiter is not looked for in the first character: "" is a string of one
	 * character, and the scan goes on behind the end of the text. The original finds a quote in
	 * whatever memory follows and ends with "Out of nodes"; so does this one.
	 */
	for (s += (*s != 0); *s != delim; s++) {
		if (*s == 0) {
			runout();
			break;
		}
		c = (unsigned char)*s;
		if (c == '\\') {
			s++;
			c = escape(&s);
		}
		tail->n_next = mknode1(1, c);
		tail = tail->n_next;
		locadv(1);
		if (*s == 0) {			/* the escape reached the end of the text */
			runout();
			break;
		}
	}
	np->n_a = tail;
	return (np);
}

/* type 25: a binary operation on two expressions */
NODE *
mknode25(int c1, int c2, NODE *a, NODE *b)
{
	NODE *np;

	np = newnode();
	np->n_type = 0x19;
	np->n_c1 = c1;
	np->n_c2 = c2;
	np->n_a = a;
	np->n_b = b;
	return (np);
}
