/* Upper half of IP, consisting of send/receive primitives, including
 * fragment reassembly, for higher level protocols.
 * Not needed when running as a standalone gateway.
 */
#include "global.h"
#include "mbuf.h"
#include "timer.h"
#include "internet.h"
#include "iface.h"
#include "ip.h"
#include "icmp.h"

void ip_recv();

#ifdef RTIMER
int32 rtime = TLB;
#endif
char ip_ttl = MAXTTL;	/* Default time-to-live for IP datagrams */

struct reasm *reasmq;

#define	INSERT	0
#define	APPEND	1
#define	PREPEND	2

/* Send an IP datagram. Modeled after the example interface on p 32 of
 * RFC 791
 */
int
ip_send(source,dest,protocol,tos,ttl,bp,length,id,df)
int32 source;			/* source address */
int32 dest;			/* Destination address */
char protocol;			/* Protocol */
char tos;			/* Type of service */
char ttl;			/* Time-to-live */
struct mbuf *bp;		/* Data portion of datagram */
int16 length;			/* Optional length of data portion */
int16 id;			/* Optional identification */
char df;			/* Don't-fragment flag */
{
	struct mbuf *htonip(),*tbp;
	struct ip ip;		/* Pointer to IP header */
	static int16 id_cntr;	/* Datagram serial number */
	int ip_route();		/* Datagram router */

	if(length == 0 && bp != NULLBUF)
		length = len_mbuf(bp);
	if(id == 0)
		id = id_cntr++;		
	if(ttl == 0)
		ttl = ip_ttl;

	/* Fill in IP header */
	ip.tos = tos;
	ip.length = IPLEN + length;
	ip.id = id;
	if(df)
		ip.fl_offs = DF;
	else
		ip.fl_offs = 0;
	ip.ttl = ttl;
	ip.protocol = protocol;
	ip.source = source;
	ip.dest = dest;
	ip.optlen = 0;
	if((tbp = htonip(&ip,bp)) == NULLBUF){
		free_p(bp);
		return -1;
	}
	return ip_route(tbp,0);		/* Toss it to the router */
}

/* Reassemble incoming IP fragments and dispatch completed datagrams
 * to the proper transport module
 */
void
ip_recv(ip,bp,rxbroadcast)
struct ip *ip;		/* Extracted IP header */
struct mbuf *bp;	/* Data portion */
char rxbroadcast;	/* True if received on subnet broadcast address */
{
	struct mbuf *fraghandle();
	void (*recv)();	/* Function to call with completed datagram */
	void tcp_input(),udp_input(),icmp_input();

	/* Initial check for protocols we can't handle */
	switch(uchar(ip->protocol)){
	case TCP_PTCL:
		recv = tcp_input;
		break;
	case UDP_PTCL:
		recv = udp_input;
		break;
	case ICMP_PTCL:
		recv = icmp_input;
		break;
	default:
		/* Send an ICMP Protocol Unknown response... */
		ip_stats.badproto++;
		/* ...unless it's a broadcast */
		if(!rxbroadcast){
			icmp_output(ip,bp,DEST_UNREACH,PROT_UNREACH,(union icmp_args *)0);
		}
		free_p(bp);
		return;
	}
	/* If we have a complete packet, call the next layer
	 * to handle the result. Note that fraghandle passes back
	 * a length field that does NOT include the IP header
	 */
	if((bp = fraghandle(ip,bp)) != NULLBUF)
		(*recv)(bp,ip->protocol,ip->source,ip->dest,ip->tos,
			ip->length - (IPLEN + ip->optlen),rxbroadcast);
}
/* Process IP datagram fragments
 * If datagram is complete, return it with ip->length containing the data
 * length (MINUS header); otherwise return NULLBUF
 */
static
struct mbuf *
fraghandle(ip,bp)
struct ip *ip;		/* IP header, host byte order */
struct mbuf *bp;	/* The fragment itself */
{
	void ip_timeout(),freefrag(),free_reasm();
	struct reasm *lookup_reasm(),*creat_reasm();
	register struct reasm *rp; /* Pointer to reassembly descriptor */
	struct frag *lastfrag,*nextfrag,*tfp,*newfrag();
	struct mbuf *tbp;
	int16 i;
	int16 offset;		/* Index of first byte in fragment */
	int16 last;		/* Index of first byte beyond fragment */
	char mf;		/* 1 if not last fragment, 0 otherwise */

	offset = (ip->fl_offs & F_OFFSET) << 3;	/* Convert to bytes */
	last = offset + ip->length - (IPLEN + ip->optlen);
	mf = (ip->fl_offs & MF) ? 1 : 0;

	rp = lookup_reasm(ip);
	if(offset == 0 && !mf){
		/* Complete datagram received. Discard any earlier fragments */
		if(rp != NULLREASM)
			free_reasm(rp);

		return bp;
	}
	if(rp == NULLREASM){
		/* First fragment; create new reassembly descriptor */
		if((rp = creat_reasm(ip)) == NULLREASM){
			/* No space for descriptor, drop fragment */
			free_p(bp);
			return NULLBUF;
		}
	}
	/* Keep restarting timer as long as we keep getting fragments */
	stop_timer(&rp->timer);
	start_timer(&rp->timer);

	/* If this is the last fragment, we now know how long the
	 * entire datagram is; record it
	 */
	if(!mf)
		rp->length = last;

	/* Set nextfrag to the first fragment which begins after us,
	 * and lastfrag to the last fragment which begins before us
	 */
	lastfrag = NULLFRAG;
	for(nextfrag = rp->fraglist;nextfrag != NULLFRAG;nextfrag = nextfrag->next){
		if(nextfrag->offset > offset)
			break;
		lastfrag = nextfrag;
	}
	/* Check for overlap with preceeding fragment */
	if(lastfrag != NULLFRAG  && offset < lastfrag->last){
		/* Strip overlap from new fragment */
		i = lastfrag->last - offset;
		pullup(&bp,NULLCHAR,i);
		if(bp == NULLBUF)
			return NULLBUF;	/* Nothing left */
		offset += i;
	}
	/* Look for overlap with succeeding segments */
	for(; nextfrag != NULLFRAG; nextfrag = tfp){
		tfp = nextfrag->next;	/* save in case we delete fp */

		if(nextfrag->offset >= last)
			break;	/* Past our end */
		/* Trim the front of this entry; if nothing is
		 * left, remove it.
		 */
		i = last - nextfrag->offset;
		pullup(&nextfrag->buf,NULLCHAR,i);
		if(nextfrag->buf == NULLBUF){
			/* superseded; delete from list */
			if(nextfrag->prev != NULLFRAG)
				nextfrag->prev->next = nextfrag->next;
			else
				rp->fraglist = nextfrag->next;
			if(tfp->next != NULLFRAG)
				nextfrag->next->prev = nextfrag->prev;
			freefrag(nextfrag);
		} else
			nextfrag->offset = last;
	}
	/* Lastfrag now points, as before, to the fragment before us;
	 * nextfrag points at the next fragment. Check to see if we can
	 * join to either or both fragments.
	 */
	i = INSERT;
	if(lastfrag != NULLFRAG && lastfrag->last == offset)
		i |= APPEND;
	if(nextfrag != NULLFRAG && nextfrag->offset == last)
		i |= PREPEND;
	switch(i){
	case INSERT:	/* Insert new desc between lastfrag and nextfrag */
		tfp = newfrag(offset,last,bp);
		tfp->prev = lastfrag;
		tfp->next = nextfrag;
		if(lastfrag != NULLFRAG)
			lastfrag->next = tfp;	/* Middle of list */
		else
			rp->fraglist = tfp;	/* First on list */
		if(nextfrag != NULLFRAG)
			nextfrag->prev = tfp;
		break;
	case APPEND:	/* Append to lastfrag */
		append(&lastfrag->buf,bp);
		lastfrag->last = last;	/* Extend forward */
		break;
	case PREPEND:	/* Prepend to nextfrag */
		tbp = nextfrag->buf;
		nextfrag->buf = bp;
		append(&nextfrag->buf,tbp);
		nextfrag->offset = offset;	/* Extend backward */
		break;
	case (APPEND|PREPEND):
		/* Consolidate by appending this fragment and nextfrag
		 * to lastfrag and removing the nextfrag descriptor
		 */
		append(&lastfrag->buf,bp);
		append(&lastfrag->buf,nextfrag->buf);
		nextfrag->buf = NULLBUF;
		lastfrag->last = nextfrag->last;

		/* Finally unlink and delete the now unneeded nextfrag */
		lastfrag->next = nextfrag->next;
		if(nextfrag->next != NULLFRAG)
			nextfrag->next->prev = lastfrag;
		freefrag(nextfrag);
		break;
	}
	if(rp->fraglist->offset == 0 && rp->fraglist->next == NULLFRAG
		&& rp->length != 0){
		/* We've gotten a complete datagram, so extract it from the
		 * reassembly buffer and pass it on.
		 */
		bp = rp->fraglist->buf;
		rp->fraglist->buf = NULLBUF;
		/* Tell IP the entire length */
		ip->length = rp->length + (IPLEN + ip->optlen);
		free_reasm(rp);
		return bp;
	} else
		return NULLBUF;
}
static struct reasm *
lookup_reasm(ip)
struct ip *ip;
{
	register struct reasm *rp;

	for(rp = reasmq;rp != NULLREASM;rp = rp->next){
		if(ip->source == rp->source && ip->dest == rp->dest
		 && ip->protocol == rp->protocol && ip->id == rp->id)
			return rp;
	}
	return NULLREASM;
}

/* Create a reassembly descriptor,
 * put at head of reassembly list
 */
static struct reasm *
creat_reasm(ip)
register struct ip *ip;
{
	register struct reasm *rp;
	void ip_timeout();

	if((rp = (struct reasm *)calloc(1,sizeof(struct reasm))) == NULLREASM)
		return rp;	/* No space for descriptor */
	rp->source = ip->source;
	rp->dest = ip->dest;
	rp->id = ip->id;
	rp->protocol = ip->protocol;
#ifdef RTIMER
	rp->timer.start = rtime;
#else
	rp->timer.start = TLB;
#endif
	rp->timer.func = ip_timeout;
	rp->timer.arg = (char *)rp;

	rp->next = reasmq;
	if(rp->next != NULLREASM)
		rp->next->prev = rp;
	reasmq = rp;
	return rp;
}

/* Free all resources associated with a reassembly descriptor */
static void
free_reasm(rp)
register struct reasm *rp;
{
	register struct frag *fp;
	void free();

	stop_timer(&rp->timer);
	/* Remove from list of reassembly descriptors */
	if(rp->prev != NULLREASM)
		rp->prev->next = rp->next;
	else
		reasmq = rp->next;
	if(rp->next != NULLREASM)
		rp->next->prev = rp->prev;
	/* Free any fragments on list, starting at beginning */
	while((fp = rp->fraglist) != NULLFRAG){
		rp->fraglist = fp->next;
		free_p(fp->buf);
		free((char *)fp);
	}
	free((char *)rp);
}

/* Handle reassembly timeouts by deleting all reassembly resources */
static void
ip_timeout(arg)
int *arg;
{
	register struct reasm *rp;

	rp = (struct reasm *)arg;
	free_reasm(rp);
}
/* Create a fragment */
static
struct frag *
newfrag(offset,last,bp)
int16 offset,last;
struct mbuf *bp;
{
	struct frag *fp;

	if((fp = (struct frag *)calloc(1,sizeof(struct frag))) == NULLFRAG){
		/* Drop fragment */
		free_p(bp);
		return NULLFRAG;
	}
	fp->buf = bp;
	fp->offset = offset;
	fp->last = last;
	return fp;
}
/* Delete a fragment, return next one on queue */
static
void
freefrag(fp)
struct frag *fp;
{
	void free();

	free_p(fp->buf);
	free((char *)fp);
}
