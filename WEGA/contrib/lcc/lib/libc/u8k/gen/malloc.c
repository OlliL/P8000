/*	C storage allocator
 *	circular first-fit strategy
 *	works with noncontiguous, but monotonically linked, arena
 *	each block is preceded by a ptr to the (pointer of) 
 *	the next following block
 *	blocks are exact number of words long 
 *	aligned to the data type requirements of ALIGN
 *	pointers to blocks must have BUSY bit 0
 *	bit in ptr is 1 for busy, 0 for idle
 *	gaps in arena are merely noted as busy blocks
 *	last block of arena (pointed to by alloct) is empty and
 *	has a pointer to first
 *	idle blocks are coalesced during space search
 *
 *	a different implementation may need to redefine
 *	ALIGN, NALIGN, BLOCK, BUSY, INT
 *	where INT is integer type to which a pointer can be cast
*/
#define INT long
#define ALIGN long
#define NALIGN 1
#define WORD sizeof(union store)
#define BLOCK 2048	/* a multiple of WORD*/
#define BUSY 1
#define testbusy(p) ((unsigned)(p)&BUSY)
#define setbusy(p) (union store *)(((INT)(p))|BUSY)
#define clearbusy(p) (union store *)((INT)(p)&~BUSY)

union store {
	union store *ptr;
	ALIGN dummy[NALIGN];
	int calloc;		/*calloc clears an array of integers*/
	char c[4];
};

extern union store *ssgbrk();
extern union store *mkseg();

static union store newseg;	/*pointer into new segment*/
static union store allocs[2];	/*initial arena*/
static union store *allocp;	/*search ptr*/
static union store *alloct;	/*arena top*/
static union store *allocx;	/* for saving overwritten INT */
static int Lw48;	/* != 0 if alloct points to real last block */

char *
malloc(nbytes)
unsigned nbytes;
{
	register union store *p, *q;
	unsigned nw;
	int i = 0;	/* 1, wenn noch ein INT fuer letzten */
			/* (Null Block)-Pointer benoetigt wird */
	unsigned j;
	union store *P;		/* hier neuen Speicher anhaengen */
	union store *T;

	if (nbytes > 0xfff8)
		return(0);	/* 8 Bytes aus 64KB Segment fuer *
				/* ersten und letzten Zeiger */
	if (!allocp || !Lw48)
		/* noch kein echter Null-Block fuer alloct da */
		i++;
	if (!allocp)
	{
		allocp = &allocs[0];
		alloct = &allocs[1];
		allocs[0].ptr = setbusy(&allocs[1]);
		allocs[1].ptr = setbusy(&allocs[0]);
		newseg.ptr = mkseg(0, 0);
	}
	nw = (nbytes + WORD + (WORD -1)) / WORD;	/* no of items */
							/* to allocate */
	p = allocp;
	q = clearbusy(p->ptr);
	/* if p points to free block, then (q-p)-1 == no of free items */
	P = alloct;
	/* if not enough free, append new block to block pointed to by P */

	/* search first free block with at least nw elements */
	/* idle blocks are coalesced during search space */
	do
	{
		if (testbusy(p->ptr))
			continue;
		for (; !testbusy(q->ptr); )
		{
			/* Nachfolgeblock ist auch frei, */
			/* deshalb an p-Block anhaengen */
			/* da jetzt q-Block vernichtet werden wird, */
			/* muss evtl allocp neu eingerichtet werden */
			T = q;
			q = clearbusy(q->ptr);
			p->ptr = q;	/* hier erfolgt die Vereinigung */
			if (T == allocp)
				allocp = q;
		}
		/* q zeigt nun auf ersten belegten Block  */
		/* p zeigt nach wie vor auf freien Block, der auf */
		/* den q-Block weist */
		if (q >= p + nw)
			/* ohne wrap around waere p gross genug */
			if ( p <= p + nw)
				goto found; /* tatsaechlich gefunden */
		if (q == alloct)
			/* Ende allen verwalteten Speichers erreicht */
			/* An Block auf den P weist, evtl. zusaetzlichen */
			/* anfuegen */
			P = p;
	}
	while (p = q, q = clearbusy(p->ptr), p!= allocp) ;
			/* dieses p ist abgehandelt, naechstes nehmen */
	/* nicht genuegend Speicher vorhanden, einen grossen Hieb holen */
	j = ((((long)(nw+i-(alloct-P)))*WORD)+(BLOCK-1))/BLOCK;
	/* no of requested (+i) - no of already available items rounded */
	/* to BLOCKs (only 32 BLOCKs in one Segment !) */
	if (j > 32)
		return(0);
	nbytes = (j * BLOCK) - 1;
	p = ssgbrk(newseg.c[0], nbytes);
	if ((long)p == -1L)
	{
		j = (((long)(nw+1))*WORD + (BLOCK-1))/BLOCK;
			/* 1 item fuer alloct block schon beruecksichtigt */
		nbytes = j * BLOCK;
		p = mkseg(0, nbytes - 1);
		if ((unsigned)p == -1)
			return(0);
		newseg.ptr = p;
		i = 1;		/* alloct block erforderlich */
	}
	if (i)
	{
		/* bisherigen alloct block (dummy) dicht machen */
		/* und vor neu erhaltenen block ketten */
		alloct->ptr = setbusy(p);
		q = p;
		Lw48++;
	}
	else
	{
		p = P;
		q = alloct;
	}
	q = (union store *)((char *)q + j*BLOCK);
	if (i)
		q--;
	q->ptr = setbusy(&allocs[0]);
	alloct = q;
found:
	allocp = p + nw;
	if (q > allocp)
	{
		allocx = allocp->ptr;
		allocp->ptr = q;
	}
	p->ptr = setbusy(allocp);
	return((char *)(p+1));
}

free(ap)
register union store *ap;
{

	allocp = --ap;		/* strategy tuned for LIFO allocation */
	ap->ptr = clearbusy(ap->ptr);
}

static char *
find_nc_block(nc)
unsigned nc;
{
	register union store *p, *q;
	unsigned wc;

	wc = (nc + WORD + (WORD-1)) /  WORD;
	p = allocp;
	q = clearbusy(p->ptr);
	do
	{
		if (testbusy(p->ptr))
			continue;
		while (!testbusy(q->ptr))
		{
			if (q == allocp)
			{
				q = q->ptr;
				allocp = q;
			} else
				q = q->ptr;
			p->ptr = q;
		}
		if (q < p+wc)
			continue;
		if (p <= p+wc)
			return((char *)p);
	} while (p=q, q=clearbusy(p->ptr), p!=allocp) ;
	return(0);
}

char *
realloc(p, nbytes)
register union store *p;
unsigned nbytes;
{
	register union store *q;
	union store *X, *Y;
	unsigned own;
	int i;

	X = (p-1)->ptr;
	if (!testbusy(X))
		return((char *)0);
	X = clearbusy(X);
	Y = X->ptr;
	free(p);
	own = (char *)X - (char *)p;
				/* size of old block */
	q = (union store *)malloc(nbytes);
	if (q == p)
		return((char *)q);
	if ((long)q == 0)
	{
		X->ptr = Y;
		Y = p - 1;
		Y->ptr = setbusy(X);
		for (q=allocs; q != alloct; q=p)
		{
			p = clearbusy(q->ptr);
			if (p == Y)
				return((char *)0);
			if ( p > Y)
			{
				q->ptr = setbusy(Y);
				break;
			}
		}
		return((char *)0);
	}
	nbytes = ((nbytes + WORD - 1) / WORD) * WORD;
				/* size of new block */
	i = (nbytes < own) ? nbytes : own;
	q = (union store *)((char *)q + i);
	p = (union store *)((char *)p + i);
	i /= WORD;
	while (i--)
	{
		p--;
		q--;
		*((INT *)q) = *(INT *)p;
	}
	if((char *)q+nbytes > (char *)p && (char *)q+nbytes < (char *)p+own)
		((union store *)((char *)q+((char *)q+nbytes-(char *)p)))->ptr
			= allocx;
	return((char *)q);
}

/*
 *	calloc - allocate and clear memory block
 */

char *
calloc(num, size)
register unsigned num;
register unsigned size;
{
	register INT *q;

	num *= size;
	q = (INT *)(malloc(num));
	if((INT)q == 0)
		return((char *)0);
	num = ((num+WORD-1)/WORD)*WORD;
	q = (INT *)((char *)q + num);
	num /= WORD;
	while((int)(--num) >= 0)
		*--q = 0;
	return((char *)q);
}
