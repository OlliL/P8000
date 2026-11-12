/* @[$] symscan.c	2.1  09/12/83 11:34:06 - 87wega3.2 */

/*
 * symscan(p, stream): read the symbol table of an s.out file from stream (readsym) up to the symbol named
 * p->nl_name, set nl_value and nl_type from it. A name of more than 8 characters is stored as a long
 * name: its first byte is 0200 | (length + 1), the name follows over (length + 6) / 14 more entries.
 * Reconstructed from the member of /lib/slibc.a; the names are guesses. The member of libc.a still comes from
 * ../oldcc/symscan (one block: another register choice, see ../HANDOFF.md).
 */
#include <s.out.h>
#include <nlist.h>
#include <stdio.h>

symscan(p, stream)
struct nlist *p;
FILE *stream;
{
	register n;
	register char *s, *t;
	struct s_nlist sym;
	char rest[128];		/* the entries of a long name (readsym) */
	char c;
	int k;

	p->nl_type = 0;
	n = 0;
	for (s = p->nl_name; *s++; )
		n++;
	if (n > 8)
		goto lname;
	do {
		if ((n = readsym(&sym, stream)) == 0) {
			sym.sn_name[8] = 0;
			s = sym.sn_name;
			for (t = p->nl_name; *t; t++, s++)
				if (*t != *s)
					break;
			if (*s == *t)
				goto found;
		}
	} while (n >= 0);
	goto found;
lname:
	c = n + 1;
	k = (c + 5) / 14;
	c |= 0200;
	do {
		if ((n = readsym(&sym, stream)) == k && sym.sn_name[0] == c) {
			s = p->nl_name;
			t = &sym.sn_name[1];
			while (*s++ && *t++ && *t)
				;
			if (*t == 0)
				goto found;
		}
	} while (n >= 0);
found:
	if (n >= 0) {
		p->nl_value = sym.sn_value;
		p->nl_type = sym.sn_type;
	}
}
