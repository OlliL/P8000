# include	<stdio.h>
# include	"buf.h"
# include	"gen.h"

static char	*Buf_flat;

/*
**  BUFFER MANIPULATION ROUTINES
**
**
**
**  BUFPUT -- put character onto buffer
**
**	The character 'c' is put onto the buffer 'bp'.  If the buffer
**	need be extended it is.
*/
bufput(c, bp)
int			c;
register struct buf	**bp;
{
	register struct buf	*b;
	register struct buf	*a;
	extern struct buf	*bufalloc();

	if (!(b = *bp) || b->ptr >= &b->buffer[BUFSIZE])
	{
		/* allocate new buffer segment */
		a = bufalloc(sizeof (struct buf));
		a->nextb = b;
		a->ptr = a->buffer;
		*bp = b = a;
	}

	*b->ptr++ = c;
}


/*
**  BUFGET -- get character off of buffer
**
**	The buffer is popped and the character is returned.  If the
**	segment is then empty, it is returned to the free list.
*/
bufget(bp)
register struct buf	**bp;
{
	register struct buf	*b;
	register int		c;

	if (!(b = *bp) || b->ptr == b->buffer)
		return (0);	/* buffer is empty -- return end of file */

	c = *--(b->ptr);

	/* check to see if we have emptied the (non-initial) segment */
	if (b->ptr == b->buffer && b->nextb)
	{
		/* deallocate segment */
		*bp = b->nextb;
		buffree(b);
	}

	return (c);
}


/*
**  BUFPURGE -- return an entire buffer to the free list
**
**	The buffer is emptied and returned to the free list.  This
**	routine should be called when the buffer is to no longer
**	be used.
*/
bufpurge(bp)
register struct buf	**bp;
{
	register struct buf	*b;
	register struct buf	*a;

	b = *bp;
	*bp = (struct buf *) 0;

	/* return the segments to the free list */
	while (b)
	{
		a = b->nextb;
		buffree(b);
		b = a;
	}
}


/*
**  BUFFLUSH -- flush a buffer
**
**	The named buffer is truncated to zero length.  However, the
**	segments of the buffer are not returned to the system.
*/
bufflush(bp)
register struct buf	**bp;
{
	register struct buf	*b;

	if (!(b = *bp))
		return;

	/* return second and subsequent segments to the system */
	bufpurge(&b->nextb);

	/* truncate this buffer to zero length */
	b->ptr = b->buffer;
}


/*
**  BUFCRUNCH -- flatten a series of buffers to a string
**
**	The named buffer is flattenned to a conventional C string,
**	null terminated.  The buffer is deallocated.  The string is
**	allocated "somewhere" off in memory, and a pointer to it
**	is returned.
*/
char	*bufcrunch(buffer)
register struct buf	**buffer;
{
	register char	*p;
	extern char	*bufflatten();

	p = bufflatten(*buffer, 1);
	*p = 0;
	*buffer = (struct buf *) 0;
	return (Buf_flat);
}


char	*bufflatten(b, length)
register struct buf	*b;
int		length;
{
	register char		*p;
	register char		*q;
	extern char		*bufflatten();
	extern struct buf	*bufalloc();

	/* see if we have advanced to beginning of buffer */
	if (!b)	/* yes, allocate the string */
		return (Buf_flat = (char *) bufalloc(length));

	/* no, keep moving back */
	p = (char *) bufflatten(b->nextb, length + (b->ptr - b->buffer));

	/* copy buffer into string */
	for (q = b->buffer; q < b->ptr; )
		*p++ = *q++;

	/* deallocate the segment */
	buffree(b);

	/* process next segment */
	return (p);
}


/*
**  BUFALLOC -- allocate clear memory
**
**	This is similar to the system alloc routine except that
**	it has no error return, and memory is guaranteed to be clear
**	when you return.
**
**	It might be nice to rewrite this later to avoid the nasty
**	memory fragmentation that alloc() tends toward.
**
**	The error processing might have to be modified if used anywhere
**	other than DBS.
*/
struct buf	*bufalloc(size)
register int	size;
{
	register struct buf	*p;
	extern int		(*AAexitfn)();	/* defined in AAsyserr.c */
	extern char		*malloc();

	if (!(p = (struct buf *) malloc(size)))
	{
		printf("%sMACRO processor: Speicherueberlauf\n", ERROR);
		fflush(stdout);
		(*AAexitfn)(-1);
	}

	bufclear(p, size);

	return (p);
}


/*
**  BUFCLEAR -- clear a block of core memory
**
**	Parameters:
**		buf -- pointer to area
**		size -- size in bytes
*/
bufclear(buf, size)
register char	*buf;
register int	size;
{
	register char	*b;

	for (b = &buf[size]; buf < b; )
		*buf++ = 0;
}


/*
**  BUFFREE -- free memory
*/
buffree(ptr)
register char	*ptr;
{
	if (!ptr)
		AAsyserr(16000);

	free(ptr);
}
