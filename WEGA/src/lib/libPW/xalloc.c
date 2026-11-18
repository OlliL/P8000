# include	"stdio.h"

/*
	xalloc/xfree based on alloc/free in C library at one time.
	Also xfreeall() frees all memory allocated (calls brk(II)).
 
	Xfree always coalesces contiguous free blocks.
	Xalloc uses a first fit strategy.
	Xalloc always allocates words (rounds up).
	Xalloc actually allocates one more word than the
	amount requested.  The extra word (the first word of the
	allocated block) contains the size (in bytes) of the entire block.
	This size is used by xfree to identify contiguous blocks,
	and is used by xalloc to implement the first fit strategy.

	Bad things will happen if the size word is changed.
	Worse things happen if xfree is called with a
	garbage argument.

	Xalloc returns the address of the allocated area on success,
	fatal() on failure.
	Xfree and xfreeall don't return anything.
*/

struct fb {
	unsigned	f_size;
	char		*f_next;
};

struct fb Freelist={
	0,
	(char *)0x3FFFFFFF,
};

char *Freeend = (char *)0x3FFFFFFF;

# define SIZINCRO	10
# ifdef vax
# define MAX_PAGE	256000
# else
# define MAX_PAGE	65535 - 8000
# endif
# define SLOP	(sizeof(int *))

extern	end;
#ifndef SEG
unsigned Lastbrk = (unsigned)&end;
#else
long	Lastbrk = (long)&end;
static union { long l; struct { unsigned seg, off; } s; } u;
char	*ssgbrk();
#endif

#ifndef SEG
xalloc(asize)
#else
char *xalloc(asize)
#endif
unsigned asize;
{
	register unsigned usize;
	register struct fb *np, *cp;
	int	tot = 0;
	long	asizel = 0;
	long	lastbrkl = 0;
	char	*xcp;
#ifdef SEG
	int	seg = (u.l = (long)&end, (u.s.seg & 0x7FFF) >> 8);
#endif

	if((usize = asize) == 0)
		return(0);
	usize += 2*sizeof(int *) -1;
	usize &= ~(sizeof(int *) -1);
	for(;;) {
		cp = &Freelist;
		while((np = (struct fb *)cp->f_next) != (struct fb *)Freeend) {
			if(np->f_size>=usize) {
			/*
				Don't break the block up if it
				is not more than SLOP bigger than the
				amount needed.
			*/
				if(usize+SLOP >= np->f_size)
					cp->f_next = np->f_next;
			/*
				Break the block into 2 pieces.
			*/
				else {
#ifndef SEG
					cp = (struct fb *)(cp->f_next = (char *)(((int)np) + usize));
#else
					cp = (struct fb *)(cp->f_next = (char *)((long)np + usize));
#endif
					cp->f_size = np->f_size - usize;
					cp->f_next = np->f_next;
					np->f_size = usize;
				}
#ifndef SEG
				return((int)&np->f_next);
#else
				return((char *)&np->f_next);
#endif
			}
			cp = np;
		}
	/*
		Nothing on the free list is big enough;
		get more core from the operating system.
	*/
		asize = usize<1024? 1024: usize;
		asize = (asize+511) & (~511);
		asizel = asize;
		lastbrkl = Lastbrk;
#ifndef SEG
		if(((asizel + lastbrkl) > MAX_PAGE) ||
			(cp = (struct fb *)sbrk(asize)) == (struct fb *)-1) {
			xcp = (char *)sbrk(SIZINCRO);
			while(((cp = (struct fb *)sbrk(SIZINCRO)) != (struct fb *)-1) &&
				(xcp != (char *)-1)) {
				tot += SIZINCRO;
				lastbrkl = Lastbrk = ((int) cp) + SIZINCRO;
				if((SIZINCRO + lastbrkl) > MAX_PAGE) {
					break;
				}
			}
#else
		if(((u.l = asizel + lastbrkl, u.s.off) > MAX_PAGE) ||
			(cp = (struct fb *)ssgbrk(seg, asize)) == (struct fb *)-1) {
			xcp = (char *)ssgbrk(seg, SIZINCRO);
			while(((cp = (struct fb *)ssgbrk(seg, SIZINCRO)) != (struct fb *)-1) &&
				(xcp != (char *)-1)) {
				tot += SIZINCRO;
				lastbrkl = Lastbrk = (long)(cp + SIZINCRO);
				if((u.l = lastbrkl + SIZINCRO, u.s.off) > MAX_PAGE) {
					break;
				}
			}
#endif
			if((!tot) || (xcp == (char *)-1)) {
				printf("out of space \n");
				return((int)cp);
			}
			cp = (struct fb *)xcp;
			cp->f_size = tot + SIZINCRO;
		}
		else {
#ifndef SEG
			Lastbrk = ((int)cp) + asize;
#else
			Lastbrk = (long)(cp + asize);
#endif
			cp->f_size = asize;
		}
	/*
		Add new piece to free list.
	*/
		xfree(&cp->f_next);
	}
}


xfree(aptr)
char *aptr;
{
	register struct fb *ptr, *cp, *np;

#ifndef SEG
	if (aptr && aptr < (char *)Lastbrk) {
		ptr = (struct fb *) ((unsigned)aptr - sizeof(int *));
#else
	if (aptr && aptr < (char *)Lastbrk) {
		ptr = (struct fb *)(aptr - sizeof(int));
#endif
		cp = &Freelist;
		while((np = (struct fb *)cp->f_next) < ptr)
			cp = np;
	/*
		Try to coalesce with the following block.
	*/
#ifndef SEG
		if(((int)ptr) + ptr->f_size == ((int)np)) {
#else
		if(ptr + ptr->f_size == np) {
#endif
			ptr->f_size += np->f_size;
			ptr->f_next = np->f_next;
			np = ptr;
		} else
			ptr->f_next = (char *)np;
	/*
		Try to coalesce with the preceding block.
	*/
#ifndef SEG
		if(((int)ptr) + ptr->f_size == ((int)np)) {
#else
		if(cp + cp->f_size == ptr) {
#endif
			cp->f_size += ptr->f_size;
			cp->f_next = ptr->f_next;
		} else
			cp->f_next = (char *)ptr;
	}
}


xfreeall()
{
#ifndef SEG
	brk(&end);
	Lastbrk = (unsigned)&end;
#else
	sgbrk(&end);
	Lastbrk = (long)&end;
#endif
	Freelist.f_size = 0;
	Freelist.f_next = Freeend;
}
