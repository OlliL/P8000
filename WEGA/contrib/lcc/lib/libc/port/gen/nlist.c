#include <s.out.h>

#define	BADMAGIC(X) (X.s_magic != S_MAGIC1 &&\
		X.s_magic != S_MAGIC3 &&\
		X.s_magic != S_MAGIC4 &&\
		X.s_magic != N_MAGIC1 &&\
		X.s_magic != N_MAGIC3 &&\
		X.s_magic != N_MAGIC4)
#define S_NLEN	(sizeof(struct s_nlist))

struct nlist
{
	char	*nl_name;
	long	nl_value;
	char	nl_type;
} ;

extern long lseek();
extern int open(), read(), close(), strncmp();

int
nlist(name, list)
char *name;
struct nlist *list;
{
	struct s_nlist space[10];
	struct s_exec buf;
	int	namlen;
	unsigned n, m; 
	register struct nlist *p;
	register struct s_nlist *q;
	char * nameptr;
	long	sa;
	int	fd;

	for(p = list; p->nl_name != (char *)0; p++) {
		p->nl_type = 0;
		p->nl_value = 0;
	}
	
	if((fd = open(name, 0)) < 0)
		return(-1);
	(void) read(fd, (char *)&buf, sizeof(buf));
	if(BADMAGIC(buf)) {
		(void) close(fd);
		return(-1);
	}
	sa = buf.s_imsize;
	if((buf.s_flag & SF_STRIP) == 0)
		sa += sa;
	sa += (long)sizeof(buf) + buf.s_segt;
	(void) lseek(fd, sa, 0);
	n = buf.s_syms;
	while(n) {
		(void) read(fd, (char*)space, S_NLEN);
		n -= S_NLEN;
		q = space;
		nameptr = q->sn_name;
		namlen = SHORTNAME;
		if ( *nameptr & 0x80 )
		{
			namlen = (*nameptr & 0x7f);
			if (!namlen)
				namlen = 128;
			namlen--;
			nameptr++;
			m = namlen -(SHORTNAME-1);
			m += (S_NLEN - 1);
			m = (m/S_NLEN)*S_NLEN;
			(void) read(fd, (char*)(&space[1]), m);
			n -= m;
		}
		for(p=list; p->nl_name; ++p) {
			if(strncmp(p->nl_name, nameptr, namlen))
				continue;
			p->nl_value = q->sn_value;
			p->nl_type = q->sn_type;
			break;
		}
	}
	(void) close(fd);
	return(0);
}
