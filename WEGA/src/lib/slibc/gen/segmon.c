/* @[$] segmon.c	1.1  08/17/83 10:55:34 - 87wega3.2 */

/*
 * segmon(sr, cntsiz): monitor for segmented programs. sr is a list of segment ranges, ended by a
 * segment number 0377; each range gets a profile buffer of its own (mkseg), the header segment holds
 * the counts. segmon of a list that starts with 0377 stops the profiling and writes mon.out.
 * Reconstructed from the member of /lib/slibc.a; the names are guesses.
 */

struct segrange {
	char	sr_seg;
	unsigned sr_high;
	unsigned sr_low;
};

struct seghdr {
	long	sh_low;
	long	sh_high;
	long	sh_scale;
	long	sh_seg;
};

extern	cntno;
extern	errno;
long	*ctbase;
long	mkseg();

segmon(sr, cntsiz)
struct segrange *sr;
{
	register o;
	static char *segbuf[10];
	static long *hdr;
	static unsigned segsiz[10];
	static long *nsegs;
	struct seghdr *sh;
	int i, hsize;
	long *sp;

	cntno = cntsiz;
	if (sr->sr_seg == 0377) {
		sprofil(0, (char *)0, 0, 0, 0);
		o = creat("mon.out", 0666);
		i = 0;
		if (write(o, hdr, hdr[1]) == -1) {
			printf("system error %d on write to mon.out\n", errno);
			return(-1);
		}
		(*nsegs)--;
		do {
			if (write(o, segbuf[i], segsiz[i]) == -1) {
				printf("system error %d on write to mon.out\n", errno);
				return(-1);
			}
		} while (++i <= *nsegs);
		close(o);
		return;
	}
	for (i = 0; i < 10; i++)
		segsiz[i] = sr[i].sr_high - sr[i].sr_low;
	i = (hsize = (cntsiz + 2) << 3) + 160;
	if ((hdr = (long *)mkseg(0, i)) == (long *)-1) {
		printf("system error %d while doing mkseg\n", errno);
		return(-1);
	}
	sh = (struct seghdr *)((long)hdr + hsize);
	hdr[0] = hsize;
	hdr[1] = i;
	hdr[2] = cntsiz;
	nsegs = &hdr[3];
	ctbase = hdr + 4;
	*nsegs = 0;
	for (i = 0; sr[i].sr_seg != 0377; i++) {
		if (i >= 11)
			return;
		(*nsegs)++;
		if ((segbuf[i] = (char *)mkseg(0, segsiz[i])) == (char *)-1) {
			printf("system error %d while doing mkseg\n", errno);
			return(-1);
		}
		sh->sh_low = sr[i].sr_low;
		sh->sh_high = sr[i].sr_high;
		sp = &sh->sh_scale;
		sh->sh_seg = sr[i].sr_seg;
		o = (segsiz[i] >> 1) & 077777;
		if (segsiz[i] < o)
			o = ((long)segsiz[i] << 15) / o;
		else
			o = 077777;
		*sp = o << 1;
		sprofil(sr[i].sr_seg, segbuf[i], segsiz[i], sr[i].sr_low, o << 1);
		sh++;
	}
}
