#include "global.h"
#include "timer.h"
#include "mbuf.h"
#include "netuser.h"
#include "internet.h"
#include "tcp.h"

struct tcb *tcbs[NTCB];
int16 tcp_mss = DEF_MSS;	/* Maximum segment size to be sent with SYN */
int32 tcp_irtt = DEF_RTT;	/* Initial guess at round trip time */

/* Lookup connection, return TCB pointer or NULLTCB if nonexistant */
struct tcb *
lookup_tcb(conn)
struct connection *conn;
{
	register struct tcb *tcb;
	int16 hash_tcb();	

	tcb = tcbs[hash_tcb(conn)];
	while(tcb != NULLTCB){
		/* Yet another structure compatibility hack */
		if(conn->local.address == tcb->conn.local.address
		 && conn->remote.address == tcb->conn.remote.address
		 && conn->local.port == tcb->conn.local.port
		 && conn->remote.port == tcb->conn.remote.port)
			break;
		tcb = tcb->next;
	}
	return tcb;
}

/* Create a TCB, return pointer. Return pointer if TCB already exists. */
struct tcb *
create_tcb(conn)
struct connection *conn;
{
	register struct tcb *tcb;
	void tcp_timeout(),tcp_msl();
	void link_tcb();

	if((tcb = lookup_tcb(conn)) != NULLTCB)
		return tcb;
	if((tcb = (struct tcb *)calloc(1,sizeof (struct tcb))) == NULLTCB)
		return NULLTCB;
	ASSIGN(tcb->conn,*conn);

	tcb->cwind = tcb->mss = tcp_mss;
	tcb->ssthresh = 0xffff;
	tcb->srtt = tcp_irtt;
	/* Initialize timer intervals */
	tcb->timer.start = tcb->srtt / MSPTICK;
	tcb->timer.func = tcp_timeout;
	tcb->timer.arg = (char *)tcb;
	tcb->rtt_timer.start = MAX_TIME; /* Largest possible value */

	link_tcb(tcb);
	return tcb;
}

/* Close our TCB */
void
close_self(tcb,reason)
register struct tcb *tcb;
char reason;
{
	struct reseq *rp,*rp1;
	void free();

	stop_timer(&tcb->timer);
	stop_timer(&tcb->rtt_timer);
	tcb->reason = reason;

	/* Flush reassembly queue; nothing more can arrive */
	for(rp = tcb->reseq;rp != NULLRESEQ;rp = rp1){
		rp1 = rp->next;
		free_p(rp->bp);
		free((char *)rp);
	}
	tcb->reseq = NULLRESEQ;
	setstate(tcb,CLOSED);
}

/* Determine initial sequence number */
int32
iss()
{
	static int32 seq;

	seq += 250000l;
	return seq;
}

/* Sequence number comparisons
 * Return true if x is between low and high inclusive,
 * false otherwise
 */
int
seq_within(x,low,high)
register int32 x,low,high;
{
	if(low <= high){
		if(low <= x && x <= high)
			return 1;
	} else {
		if(low >= x && x >= high)
			return 1;
	}
	return 0;
}
int
seq_lt(x,y)
register int32 x,y;
{
	return (long)(x-y) < 0;
}
#ifdef DEAD
int
seq_le(x,y)
register int32 x,y;
{
	return (long)(x-y) <= 0;
}
#endif
int
seq_gt(x,y)
register int32 x,y;
{
	return (long)(x-y) > 0;
}
int
seq_ge(x,y)
register int32 x,y;
{
	return (long)(x-y) >= 0;
}

/* Hash a connect structure into the hash chain header array */
static int16
hash_tcb(conn)
struct connection *conn;
{
	register int16 hval;

	/* Compute hash function on connection structure */
	hval = hiword(conn->remote.address);
	hval ^= loword(conn->remote.address);
	hval ^= hiword(conn->local.address);
	hval ^= loword(conn->local.address);
	hval ^= conn->remote.port;
	hval ^= conn->local.port;
	hval %= NTCB;
	return hval;
}
/* Insert TCB at head of proper hash chain */
void
link_tcb(tcb)
register struct tcb *tcb;
{
	register struct tcb **tcbhead;
	int16 hash_tcb();
	char i_state;

	tcb->prev = NULLTCB;
	i_state = disable();
	tcbhead = &tcbs[hash_tcb(&tcb->conn)];
	tcb->next = *tcbhead;
	if(tcb->next != NULLTCB){
		tcb->next->prev = tcb;
	}
	*tcbhead = tcb;
	restore(i_state);
}
/* Remove TCB from whatever hash chain it may be on */
void
unlink_tcb(tcb)
register struct tcb *tcb;
{
	register struct tcb **tcbhead;
	int16 hash_tcb();
	char i_state;

	i_state = disable();
	tcbhead = &tcbs[hash_tcb(&tcb->conn)];
	if(*tcbhead == tcb)
		*tcbhead = tcb->next;	/* We're the first one on the chain */
	if(tcb->prev != NULLTCB)
		tcb->prev->next = tcb->next;
	if(tcb->next != NULLTCB)
		tcb->next->prev = tcb->prev;
	restore(i_state);
}
void
setstate(tcb,newstate)
register struct tcb *tcb;
register char newstate;
{
	register char oldstate;

	oldstate = tcb->state;
	tcb->state = newstate;
	if(tcb->s_upcall){
		(*tcb->s_upcall)(tcb,oldstate,newstate);
	}
	/* Notify the user that he can begin sending data */
	if(tcb->t_upcall && newstate == ESTABLISHED){
		(*tcb->t_upcall)(tcb,tcb->window - tcb->sndcnt);
	}
}
/* Convert TCP header in host format into mbuf ready for transmission,
 * link in data (if any), and compute checksum
 */
struct mbuf *
htontcp(tcph,data,ph)
struct tcp *tcph;
struct mbuf *data;
struct pseudo_header *ph;
{
	int16 hdrlen;
	struct mbuf *bp;
	register char *cp;
	int16 csum,cksum();

	hdrlen = (tcph->mss != 0) ? TCPLEN + MSS_LENGTH : TCPLEN;

	if((bp = pushdown(data,hdrlen)) == NULLBUF){
		free_p(data);
		return NULLBUF;
	}
	cp = bp->data;
	cp = put16(cp,tcph->source);
	cp = put16(cp,tcph->dest);
	cp = put32(cp,tcph->seq);
	cp = put32(cp,tcph->ack);
	*cp++ = hdrlen << 2;	/* Offset field */
	*cp++ = tcph->flags;
	cp = put16(cp,tcph->wnd);
	*cp++ = 0;	/* Zero out checksum field */
	*cp++ = 0;
	cp = put16(cp,tcph->up);

	if(tcph->mss != 0){
		*cp++ = MSS_KIND;
		*cp++ = MSS_LENGTH;
		cp = put16(cp,tcph->mss);
	}
	csum = cksum(ph,bp,ph->length);
	cp = &bp->data[16];	/* Checksum field */
	*cp++ = csum >> 8;	/* this is certainly non portable! - K5JB */
	*cp = csum;

	return bp;
}
/* Pull TCP header off mbuf */
int
ntohtcp(tcph,bpp)
struct tcp *tcph;
struct mbuf **bpp;
{
	int16 hdrlen;
	int16 i,optlen;

	tcph->source = pull16(bpp);
	tcph->dest = pull16(bpp);
	tcph->seq = pull32(bpp);
	tcph->ack = pull32(bpp);
	if(*bpp == NULLBUF)
		/* Buffer too short to pull off header length */
		return -1;
	hdrlen = (pullchar(bpp) & 0xf0) >> 2;
	tcph->flags = pullchar(bpp);
	tcph->wnd = pull16(bpp);
	(void)pull16(bpp);	/* Skip checksum */
	tcph->up = pull16(bpp);
	tcph->mss = 0;

	/* Check for option field. Only space for one is allowed, but
	 * since there's only one TCP option (MSS) this isn't a problem
	 */
	if(hdrlen < TCPLEN)
		return -1;	/* Header smaller than legal minimum */
	if(hdrlen == TCPLEN)
		return hdrlen;	/* No options, all done */

	if(hdrlen > len_mbuf(*bpp) + TCPLEN){
		/* Remainder too short for options length specified */
		return -1;
	}
	/* Process options */
	for(i=TCPLEN; i < hdrlen;){
		switch(pullchar(bpp)){
		case EOL_KIND:
			i++;
			goto eol;	/* End of options list */
		case NOOP_KIND:
			i++;
			break;
		case MSS_KIND:
			optlen = pullchar(bpp);
			if(optlen == MSS_LENGTH)
				tcph->mss = pull16(bpp);
			i += optlen;
			break;
		}
	}
eol:
	/* Get rid of any padding */
	if(i < hdrlen)
		pullup(bpp,NULLCHAR,hdrlen - i);
	return hdrlen;
}
