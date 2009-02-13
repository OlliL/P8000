/* Primitive mbuf allocate/free routines */
/* With version k29, adopted the pointer and count of mbufs duped with dup_p
 * to correct a problem where packets in ax.25 queue were garbaged when tcpin
 * freed acked data.  This was preferable to using copy_p in tcpout.c 0 K5JB
 */

#include "config.h"
#include "global.h"
#include "mbuf.h"

#undef MBUFDEBUG	/* See also alloc.c */

#if defined(MBUFDEBUG) && !defined (UNIX) /* alloc.c not used in unix */
#define	NALLOC	150
/* Map of allocated memory chunks */
struct alloc {
	char *addr;
	unsigned size;
	unsigned long pc;
	unsigned long mbufpc;
};

extern struct alloc alloc[];
#endif /* MBUFDEBUG */

#ifdef UNIX
	char *memcpy();
#else
	void *memcpy();
#endif

/* Allocate mbuf with associated buffer of 'size' bytes */
struct mbuf *
alloc_mbuf(size)
#if defined(MBUFDEBUG) && !defined(UNIX)
int16 size;
#else
register int16 size;
#endif
{
	register struct mbuf *bp;
#if defined(MBUFDEBUG) && !defined(UNIX)
	struct alloc *ap;
	unsigned *ptr;
	int i,i_state,disable();
	void restore();
	extern int maxalloc;
#endif

	if((bp = (struct mbuf *)malloc((unsigned)(size + sizeof(struct mbuf)))) == NULLBUF)
		return NULLBUF;
#if defined(MBUFDEBUG) && !defined(UNIX) /* in conjunction with ALLOCDEBUG */
	ptr = (unsigned *)&size;
	i_state = disable();
	for(ap = alloc,i=0; ap < &alloc[NALLOC] && i <= maxalloc;ap++,i++)
		if(ap->addr == (char *)bp){
#ifdef LARGECODE
			ap->mbufpc = ((unsigned long)ptr[-1] << 16);
			ap->mbufpc |= (unsigned)ptr[-2];
#else
			ap->mbufpc = (unsigned long)ptr[-1];
#endif
			break;
		}
	restore(i_state);
#endif /* MBUFDEBUG */

	bp->next = bp->anext = NULLBUF;
	if(size != 0){
		bp->data = (char *)(bp + 1);
	} else {
		bp->data = NULLCHAR;
	}
	bp->size = size;
	bp->cnt = 0;
	bp->refcnt = 1;
	bp->dup = NULLBUF;
	return bp;
}

/* Free all resources associated with mbuf
 * Return pointer to next mbuf in packet chain
 */
struct mbuf *
free_mbuf(bp)
register struct mbuf *bp;
{
	void free();
	struct mbuf *bpnext;

	if(bp == NULLBUF)
		return NULLBUF;

	bpnext = bp->next;

	if(bp->dup != NULLBUF){	/* taken from NOS */
		free_mbuf(bp->dup);
		bp->dup = NULLBUF;
	}
	if(--bp->refcnt <= 0){
		bp->next = NULLBUF;	/* detect attempts to use */
		bp->data = NULLCHAR;	/* a freed mbuf */
		free((char *)bp);
	}
	return bpnext;
}

/* Free packet (a chain of mbufs). Return pointer to next packet on queue,
 * if any
 */
struct mbuf *
free_p(bp)
register struct mbuf *bp;
{
	struct mbuf *abp;

	if(bp == NULLBUF)
		return NULLBUF;
	abp = bp->anext;
	while(bp != NULLBUF)
		bp = free_mbuf(bp);
	return abp;
}		
/* Free entire queue of packets (of mbufs) */
void
free_q(q)
struct mbuf **q;
{
	register struct mbuf *bp;

	while((bp = dequeue(q)) != NULLBUF)
		free_p(bp);
}

/* Count up the total number of bytes in an mbuf */
int16
len_mbuf(bp)
register struct mbuf *bp;
{
	int cnt;

	cnt = 0;
	while(bp != NULLBUF){
		cnt += bp->cnt;
		bp = bp->next;
	}
	return cnt;
}
/* Count up the number of packets in a queue */
int16
len_q(bp)
register struct mbuf *bp;
{
	register int cnt;

	for(cnt=0;bp != NULLBUF;cnt++,bp = bp->anext)
		;
	return cnt;
}
/* Trim mbuf to specified length by lopping off end */
void
trim_mbuf(bpp,length)
struct mbuf **bpp;
int16 length;
{
	register int16 tot = 0;
	register struct mbuf *bp;

	if(bpp == NULLBUFP || *bpp == NULLBUF)
		return;	/* Nothing to trim */

	if(length == 0){
		/* Toss the whole thing */
		free_p(*bpp);
		*bpp = NULLBUF;
		return;
	}
	/* Find the point at which to trim. If length is greater than
	 * the packet, we'll just fall through without doing anything
	 */
	for( bp = *bpp; bp != NULLBUF; bp = bp->next){
		if(tot + bp->cnt < length){
			tot += bp->cnt;
		} else {
			/* Cut here */
			bp->cnt = length - tot;
			free_p(bp->next);
			bp->next = NULLBUF;
			break;
		}
	}
}
/* Duplicate/enqueue/dequeue operations based on mbufs */

/* Duplicate first 'cnt' bytes of packet starting at 'offset'.
 * This is done without copying data; only the headers are duplicated,
 * but without data segments of their own. The pointers are set up to
 * share the data segments of the original copy. The return pointer is
 * passed back through the first argument, and the return value is the
 * number of bytes actually duplicated.
 */
int16
dup_p(hp,bp,offset,cnt)
struct mbuf **hp;
register struct mbuf *bp;
register int16 offset;
register int16 cnt;
{
	register struct mbuf *cp;
	int16 tot;

	if(cnt == 0 || bp == NULLBUF || hp == NULLBUFP){
		if(hp != NULLBUFP)
			*hp = NULLBUF;
		return 0;
	}
	if((*hp = cp = alloc_mbuf(0)) == NULLBUF){
		return 0;
	}
	/* Skip over leading mbufs that are smaller than the offset */
	while(bp != NULLBUF && bp->cnt <= offset){
		offset -= bp->cnt;
		bp = bp->next;
	}
	if(bp == NULLBUF){
		free_mbuf(cp);
		*hp = NULLBUF;
		return 0;	/* Offset was too big */
	}
	tot = 0;
	for(;;){
		/* Make sure we get the original, "real" buffer (i.e. handle the
		 * case of duping a dupe)
		 */
		if(bp->dup != NULLBUF)
			cp->dup = bp->dup;
		else
			cp->dup = bp;

		/* Increment the duplicated buffer's reference count */
		cp->dup->refcnt++;

		cp->data = bp->data + offset;
		cp->cnt = min(cnt,bp->cnt - offset);
		offset = 0;
		cnt -= cp->cnt;
		tot += cp->cnt;
		bp = bp->next;
		if(cnt == 0 || bp == NULLBUF || (cp->next = alloc_mbuf(0)) == NULLBUF)
			break;
		cp = cp->next;
	}
	return tot;
}
/* Copy first 'cnt' bytes of packet into a new, single mbuf */
struct mbuf *
copy_p(bp,cnt)
register struct mbuf *bp;
register int16 cnt;
{
	register struct mbuf *cp;
	register char *wp;
	register int16 n;

	if(bp == NULLBUF || cnt == 0 || (cp = alloc_mbuf(cnt)) == NULLBUF)
		return NULLBUF;
	wp = cp->data;
	while(cnt != 0 && bp != NULLBUF){
		n = min(cnt,bp->cnt);
		memcpy(wp,bp->data,n);
		wp += n;
		cp->cnt += n;
		cnt -= n;
		bp = bp->next;
	}
	return cp;
}
/* Copy and delete "cnt" bytes from beginning of packet. Return number of
 * bytes actually pulled off - dinked with this while looking for cause
 * of packets being garbaged by tcpin when they are in the ax25 transmit
 * queue.  Think I fixed it with a copy_p() in tcpout so restored this
 * to original - K5JB (k29)
 */
int16
pullup(bph,buf,cnt)
struct mbuf **bph;
char *buf;
int16 cnt;
{
	register struct mbuf *bp;
	int16 n,tot;

	tot = 0;
	if(bph == NULLBUFP)
		return 0;
	while(cnt != 0 && ((bp = *bph) != NULLBUF)){
		n = min(cnt,bp->cnt);
#ifdef NOSOPT	/* I can't tell that this is worth extra bytes it costs */
		if(buf != NULLCHAR){
			if(n == 1)      /* Common case optimization, from NOS - K5JB */
				*buf = *bp->data;
			else if(n > 1)
				memcpy(buf,bp->data,n);
			buf += n;
		}
#else
		if(buf != NULLCHAR && n != 0){
			memcpy(buf,bp->data,n);
			buf += n;
		}
#endif
		tot += n;
		cnt -= n;
		bp->data += n;
		bp->cnt -= n;
#ifndef SEGMENT	/* From NOS. the original code is ok if no segmentation */
		if(bp->cnt == 0)
			*bph = free_mbuf(bp);

#else
		/* following copied from NOS which uses the bp->anext after a couple
		 * of calls to pullup.  Doesn't have any benefit in NET - K5JB */

		if(bp->cnt == 0){
		/* (Note from NOS) If this is the last mbuf of a packet but there
		 * are others on the queue, return a pointer to the next on the queue.
		 * This allows pullups to to work on a packet queue
		 */
			if(bp->next == NULLBUF && bp->anext != NULLBUF){
				*bph = bp->anext;
				free_mbuf(bp);
			} else
				*bph = free_mbuf(bp);
		}
#endif
	} /* while */
	return tot;
}


/* Append mbuf to end of mbuf chain */
void
append(bph,bp)
struct mbuf **bph;
struct mbuf *bp;
{
	register struct mbuf *p;

	if(bph == NULLBUFP || bp == NULLBUF)
		return;
	if(*bph == NULLBUF){
		/* First one on chain */
		*bph = bp;
	} else {
		for(p = *bph ; p->next != NULLBUF ; p = p->next)
			;
		p->next = bp;
	}
}
/* Insert specified amount of contiguous new space at the beginning of an
 * mbuf chain. If enough space is available in the first mbuf, no new space
 * is allocated. Otherwise a mbuf of the appropriate size is allocated and
 * tacked on the front of the chain.
 *
 * This operation is the logical inverse of pullup(), hence the name.
 */
struct mbuf *
pushdown(bp,size)
register struct mbuf *bp;
int16 size;
{
	register struct mbuf *nbp;

	/* Check that bp is real and that there's data space associated with
	 * this buffer (i.e., this is not a buffer from dup_p) before
	 * checking to see if there's enough space at its front
	 */
	if(bp != NULLBUF && bp->refcnt == 1 && bp->dup == NULLBUF
		&& bp->data - (char *)(bp+1) >= size){
		/* No need to alloc new mbuf, just adjust this one */
		bp->data -= size;
		bp->cnt += size;
	} else {
		if((nbp = alloc_mbuf(size)) != NULLBUF){
			nbp->next = bp;
			nbp->cnt = size;
			bp = nbp;
		} else {
			bp = NULLBUF;
		}
	}
	return bp;
}
/* Append packet to end of packet queue */
void
enqueue(q,bp)
struct mbuf **q;
struct mbuf *bp;
{
	register struct mbuf *p;
	char i_state;

	if(q == NULLBUFP || bp == NULLBUF)
		return;
	i_state = disable();
	if(*q == NULLBUF){
		/* List is empty, stick at front */
		*q = bp;
	} else {
		for(p = *q ; p->anext != NULLBUF ; p = p->anext)
			;
		p->anext = bp;
	}
	restore(i_state);
}
/* Unlink a packet from the head of the queue */
struct mbuf *
dequeue(q)
register struct mbuf **q;
{
	register struct mbuf *bp;
	char i_state;

	if(q == NULLBUFP)
		return NULLBUF;
	i_state = disable();
	if((bp = *q) != NULLBUF){
		*q = bp->anext;
		bp->anext = NULLBUF;
	}
	restore(i_state);
	return bp;
}	

/* Copy user data into an mbuf */
struct mbuf *
qdata(data,cnt)
char *data;
int16 cnt;
{
	register struct mbuf *bp;

	if((bp = alloc_mbuf(cnt)) == NULLBUF)
		return NULLBUF;
	memcpy(bp->data,data,cnt);
	bp->cnt = cnt;
	return bp;
}
#ifdef DEAD
/* Copy mbuf data into user buffer */
int16
dqdata(bp,buf,cnt)
struct mbuf *bp;
char *buf;
unsigned cnt;
{
	unsigned n,tot;
	struct mbuf *bp1;

	if(buf == NULLCHAR)
		return 0;

	tot = 0;
	for(bp1 = bp;bp1 != NULLBUF; bp1 = bp1->next){
		n = min(bp1->cnt,cnt);
		memcpy(buf,bp1->data,n);
		cnt -= n;
		buf += n;
		tot += n;
	}
	free_p(bp);
	return tot;
}
#endif /* DEAD */

/* Pull a 32-bit integer in host order from buffer in network byte order */
int32
pull32(bpp)
struct mbuf **bpp;
{
	int32 rval;
	char buf[4];
	register char *cp;

	if(pullup(bpp,buf,4) != 4){
		/* Return zero if insufficient buffer */
		return 0;
	}
	cp = buf;

	/* Unwound for speed */
	rval = uchar(*cp++);
	rval <<= 8;
	rval |= uchar(*cp++);
	rval <<= 8;
	rval |= uchar(*cp++);
	rval <<= 8;
	rval |= uchar(*cp);

	return rval;
}
/* Pull a 16-bit integer in host order from buffer in network byte order */
int16
pull16(bpp)
struct mbuf **bpp;
{
	int16 rval;
	char buf[2];
	register char *cp;

	if(pullup(bpp,buf,2) != 2){
		/* Return zero if insufficient buffer */
		return 0;
	}
	cp = buf;

	rval = uchar(*cp++);
	rval <<= 8;
	rval |= uchar(*cp);
	return rval;
}
/* Pull single character from mbuf */
char
pullchar(bpp)
struct mbuf **bpp;
{
	char c;

	if(pullup(bpp,&c,1) != 1)
		/* Return zero if nothing left */
		c = 0;
	return c;
}
/* Put a long in host order into a char array in network order */
char *
put32(cp,x)
register char *cp;
int32 x;
{
	*cp++ = x >> 24;
	*cp++ = x >> 16;
	*cp++ = x >> 8;
	*cp++ = x;
	return cp;
}
/* Put a short in host order into a char array in network order */
char *
put16(cp,x)
register char *cp;
int16 x;
{
	*cp++ = x >> 8;
	*cp++ = x;

	return cp;
}
