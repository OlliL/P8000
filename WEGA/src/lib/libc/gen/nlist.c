/* @[$] nlist.c	2.1  09/12/83 11:28:01 - 87wega3.2 */

#include <s.out.h>
#include <nlist.h>
#define SPACE 100		/* number of symbols read at a time */

int a_magic[] = { S_MAGIC1, S_MAGIC3, N_MAGIC1, N_MAGIC3, 0 };

nlist(name, list)
char *name;
struct nlist *list;
{
	register struct s_nlist *q;
	register struct nlist *p;
	int f, i, k;
	unsigned n;
	int m;
	long sa;
	struct s_exec buf;
	struct s_nlist space[SPACE+10];
	int nleft;
	char *end, *s1, *s2;
	int diff;

	nleft = 0;
	for(p = list; *p->nl_name; p++) {
		p->nl_type = 0;
		p->nl_value = 0;
		nleft++;
	}

	if((f = open(name, 0)) < 0)
		return(-1);
	read(f, (char *) &buf, sizeof(buf));
	for(i = 0; a_magic[i]; i++)
		if(a_magic[i] == buf.s_magic)
			break;
	if(a_magic[i] == 0) {
		close(f);
		return(-1);
	}
	sa = buf.s_imsize;
	if((buf.s_flag & SF_STRIP) != 1)
		sa *= 2;
	sa += (long)(unsigned)(buf.s_segt + sizeof(buf));
	lseek(f, sa, 0);
	n = buf.s_syms;

	while(n) {
		m = SPACE * sizeof(struct s_nlist);
		if(n < SPACE * sizeof(struct s_nlist))
			m = n;
		read(f, (char *) space, m);
		n -= m;
		q = space;
		while(m) {
			for(p = list; *p->nl_name; p++) {
				s1 = p->nl_name;
				s2 = q->sn_name;
				if(*s2 & 128) {
					end = s2 + (*s2 & 127);
					s2++;
				} else
					end = s2 + 8;
				diff = 0;
				while(*s1 && s2 < end) {
					if(*s1++ != *s2++) {
						diff++;
						break;
					}
				}
				if(s2 < end && *s2)
					diff++;
				if(diff == 0) {
					p->nl_value = q->sn_value;
					if(p->nl_type = q->sn_type)
						nleft--;
					break;
				}
			}
			if(nleft == 0) {
				close(f);
				return(0);
			}
			m -= (i = readby(q)) * sizeof(struct s_nlist);
			if(m == 0)
				continue;
			q += i;
			if((i = readby(q)) > (k = (unsigned)m / sizeof(struct s_nlist))) {
				read(f, (char *) &space[SPACE], i = (i - k) * sizeof(struct s_nlist));
				n -= i;
				m += i;
			}
		}
	}
	close(f);
	return(0);
}

readby(sym)
struct s_nlist *sym;
{
	int n;

	if(sym->sn_name[0] & 128) {
		n = (sym->sn_name[0] & 127) - 7;
		n = (n - 7) / 14 + 1;
		n++;
		return(n);
	}
	return(1);
}
