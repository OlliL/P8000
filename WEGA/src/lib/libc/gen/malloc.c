/*	C storage allocator
 *	circular first-fit strategy
 *	works with noncontiguous, but monotonically linked, arena
 *	each block is preceded by a ptr to the (pointer of)
 *	the next following block
 *	blocks are exact number of words long
 *	pointers to blocks must have BUSY bit 0
 *	bit in ptr is 1 for busy, 0 for idle
 *	gaps in arena are merely noted as busy blocks
 *	last block of arena (pointed to by alloct) is empty and
 *	has a pointer to first
 *	idle blocks are coalesced during space search
 *
 *	Reconstructed from the object member of the WEGA libc (87wega3.2):
 *	the top free block is extended when the break is contiguous
 *	(sbrk in units of BLOCK bytes, at most 64 blocks per call).
 */
static char sccsid[] = "@[$]malloc.c\t3.3  09/12/83 10:32:30 - 87wega3.2";

#define INT int
#define WORD sizeof(union store)
#define BLOCK 1024	/* a multiple of WORD */
#define BUSY 1
#define NULL 0
#define testbusy(p) ((INT)(p)&BUSY)
#define setbusy(p) (union store *)((INT)(p)|BUSY)
#define clearbusy(p) (union store *)((INT)(p)&~BUSY)

union store { union store *ptr;
	      int dummy[1];
	      int calloc;	/* calloc clears an array of integers */
};

static	unsigned maxbytes = 0xFFF8;
static	unsigned maxblocks = 64;
static	union store allocs[2];	/* initial arena */
static	union store *allocp;	/* search ptr */
static	union store *alloct;	/* arena top */
static	int grown;
char	*sbrk();

char *
malloc(nbytes)
register unsigned nbytes;
{
	register union store *p, *q, *last;
	register unsigned nw;
	register temp;
	register first;

	first = 0;
	if(nbytes > maxbytes)
		return(NULL);
	if(allocp == 0 || grown == 0)
		first = 1;
	if(allocp == 0) {	/* first time */
		allocp = &allocs[0];
		alloct = &allocs[1];
		allocp->ptr = setbusy(&allocs[1]);
		alloct->ptr = setbusy(&allocs[0]);
	}
	nw = (nbytes+WORD+WORD-1)/WORD;
	p = allocp;
	q = clearbusy(p->ptr);
	last = alloct;
	do {
		if(!testbusy(p->ptr)) {
			while(!testbusy(q->ptr)) {
				if(q == allocp)
					allocp = p->ptr = q = clearbusy(q->ptr);
				else
					p->ptr = q = clearbusy(q->ptr);
			}
			if(q>=p+nw && p+nw>=p)
				goto found;
			if(q == alloct)
				last = p;
		}
		p = q;
		q = clearbusy(p->ptr);
	} while(p != allocp);
	temp = ((long)(nw - (alloct - last) + first)*WORD + BLOCK-1)/BLOCK;
	if(temp > maxblocks)
		return(NULL);
	nbytes = temp*BLOCK;
	p = (union store *)sbrk(0);
	if((char *)p + nbytes < (char *)p)
		return(NULL);
	p = (union store *)sbrk(nbytes);
	if((INT)p == -1)
		return(NULL);
	if(first) {
		alloct->ptr = setbusy(p);
		q = p + temp*(BLOCK/WORD) - 1;
		grown = 1;
	} else {
		p = last;
		q = alloct + temp*(BLOCK/WORD) - 1;
	}
	q->ptr = setbusy(allocs);
	q->ptr = q->ptr;
	alloct = q;
found:
	allocp = p + nw;
	if(q>allocp)
		allocp->ptr = q;
	p->ptr = setbusy(allocp);
	return((char *)(p+1));
}

/*	search an idle block of nbytes without growing the arena
 */
static union store *
findfree(nbytes)
register unsigned nbytes;
{
	register unsigned nw;
	register union store *p, *q;

	nw = (nbytes+WORD+WORD-1)/WORD;
	p = allocp;
	q = clearbusy(p->ptr);
	do {
		if(!testbusy(p->ptr)) {
			while(!testbusy(q->ptr)) {
				if(q == allocp)
					allocp = p->ptr = q = clearbusy(q->ptr);
				else
					p->ptr = q = clearbusy(q->ptr);
			}
			if(q>=p+nw && p+nw>=p)
				return(p);
		}
		p = q;
		q = clearbusy(p->ptr);
	} while(p != allocp);
	return(NULL);
}

/*	freeing strategy tuned for LIFO allocation
*/
free(ap)
char *ap;
{
	register union store *p = (union store *)ap;

	allocp = p = p - 1;
	p->ptr = clearbusy(p->ptr);
}

/*	realloc(p, nbytes) reallocates a block obtained from malloc()
 *	and freed since last call of malloc()
 *	to have new size nbytes, and old content
 *	returns new location, or 0 on failure
*/

char *
realloc(p, nbytes)
register union store *p;
unsigned nbytes;
{
	register union store *s, *t;
	register unsigned nw;
	register union store *b;
	register union store *q;
	register unsigned onw;
	union store *r;

	b = p - 1;
	onw = clearbusy(b->ptr) - p;
	nw = (nbytes+WORD-1)/WORD;
	if(testbusy(b->ptr))
		free((char *)p);
	q = findfree(nbytes);
	if(b<q+nw && q+nw<b+onw) {
		q->ptr = setbusy(q->ptr);
		r = (union store *)malloc(nbytes);
		q->ptr = clearbusy(q->ptr);
	} else
		r = (union store *)malloc(nbytes);
	if(r==NULL || r==p)
		return((char *)r);
	s = p;
	t = r;
	nw = nw > onw ? onw : nw;
	while(nw--!=0)
		*t++ = *s++;
	return((char *)r);
}

/*	calloc - allocate and clear memory block
*/
#define CHARPERINT (sizeof(int)/sizeof(char))

char *
calloc(num, size)
unsigned num, size;
{
	register int *q;
	register m;
	register char *mp;
	char *malloc();

	num *= size;
	mp = malloc(num);
	if(mp == NULL)
		return(NULL);
	q = (int *) mp;
	m = (num+CHARPERINT-1)/CHARPERINT;
	while(--m>=0)
		*q++ = 0;
	return(mp);
}

cfree(p, num, size)
char *p;
unsigned num, size;
{
	free(p);
}
