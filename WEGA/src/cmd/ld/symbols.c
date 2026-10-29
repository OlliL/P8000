#include "ld.h"

struct symbol cursym;
int v067c;
f2bf6()
{
	v067c++;
}

f2bfc()
{
	v067c = 0;
}

slookup(x)
int x;
{
	int spare;

	pack_name(x, cursym.sname);
	cursym.stype = ' ';
	cursym.svalue = 0;
	lookup_name();
}

pslookup(x)
int x;
{
	int spare;

	unpack_run(x, cursym.sname);
	cursym.stype = ' ';
	cursym.svalue = 0;
	lookup_name();
}

/* put_all: write all symbols of the table to the output (0x282e..0x2862 of /bin/ld) */
put_all()
{
	int spare1, spare2;

	register struct symbol *p;
	register struct symbol *end;

	p = table16_entry(0);
	end = table16_entry(symindex);
	while (p < end) {
		put_sym(&toutb, p);
		p = next_entry(p);
	}
}

/* lookup_name: look up a name in the hash table of symbols (0x25dc..0x26ac of /bin/ld, finding: register
   variables start and end are in the frame here) */
struct symbol **lookup_name()
{
	register struct symbol **slot;
	register char *cur;
	register char *rec;
	int h;
	int flag;
	char *start;
	char *end;

	h = 0;
	cur = cursym.sname;
	start = cur;
	end = lnbuf;
	if (*cur & 0x80) {
		start++;
		if (nlflag == 0)
			end++;
		else
			end = cursym.sname + (*cur & 0x7f);
	}
	for (cur = start; cur < end; cur++)
		h = h * 2 + *cur;
	slot = &hshtab[(h & 0x7fff) % 0x0923 + 2];
	while (*slot) {
		rec = (*slot)->sname;
		flag = 0;
		if (nlflag == 0)
			goto l1;
		cur = cursym.sname;
		goto l2;
l1:
		cur = start;
		if (*rec & 0x80)
			rec++;
l2:
		while (cur < end) {
			if (*cur++ != *rec++) {
				flag = 1;
				break;
			}
		}
		if (!flag)
			break;
		slot++;
		if (slot >= (struct symbol **)local)
			slot = hshtab;
	}
	return slot;
}
