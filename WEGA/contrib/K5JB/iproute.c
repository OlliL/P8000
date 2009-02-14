/* Lower half of IP, consisting of gateway routines
 * Includes routing and options processing code
 */
#include "config.h"
#include "global.h"
#include "mbuf.h"
#include "internet.h"
#include "timer.h"
#include "netuser.h"
#include "ip.h"
#include "icmp.h"
#include "iface.h"
#include "trace.h"

struct route *routes[32][NROUTE];	/* Routing table */
struct route r_default;			/* Default route entry */

int32 ip_addr;
struct ip_stats ip_stats;

#ifndef	GWONLY
struct mbuf *loopq;	/* Queue for loopback packets */
#endif

/* Route an IP datagram. This is the "hopper" through which all IP datagrams,
 * coming or going, must pass.
 *
 * "rxbroadcast" is set to indicate that the packet came in on a subnet
 * broadcast. The router will kick the packet upstairs regardless of the
 * IP destination address.
 */
int
ip_route(bp,rxbroadcast)
struct mbuf *bp;
char rxbroadcast;	/* True if packet had link broadcast address */
{
	struct mbuf *htonip();
	void ip_recv(),trim_mbuf();
	struct ip ip;			/* IP header being processed */
	int16 ip_len;			/* IP header length */
	int16 length;			/* Length of data portion */
	int32 gateway;			/* Gateway IP address */
	register struct route *rp;	/* Route table entry */
	struct interface *iface;	/* Output interface, possibly forwarded */
	struct route *rt_lookup();
	int16 offset;			/* Offset into current fragment */
	int16 mf_flag;			/* Original datagram MF flag */
	int strict = 0;			/* Strict source routing flag */
	char precedence;		/* Extracted from tos field */
	char delay;
	char throughput;
	char reliability;
	int16 opt_len;		/* Length of current option */
	char *opt;		/* -> beginning of current option */
	char *ptr;		/* -> pointer field in source route fields */
	struct mbuf *tbp;
	int ntohip(),icmp_output();
	int16 cksum();
	int16 mtu;
	ip_stats.total++;
	if(len_mbuf(bp) < IPLEN){
		/* The packet is shorter than a legal IP header */
		ip_stats.runt++;
		free_p(bp);
		return -1;
	}
	/* Sneak a peek at the IP header's IHL field to find its length */
	ip_len = (bp->data[0] & 0xf) << 2;
	if(ip_len < IPLEN){
		/* The IP header length field is too small */
		ip_stats.length++;
		free_p(bp);
		return -1;
	}
	if(cksum(NULLHEADER,bp,ip_len) != 0){
		/* Bad IP header checksum; discard */
		ip_stats.checksum++;
		free_p(bp);
		return -1;
	}
	/* Extract IP header */
	ntohip(&ip,&bp);

	if(ip.version != IPVERSION){
		/* We can't handle this version of IP */
		ip_stats.version++;
		free_p(bp);
		return -1;
	}
	/* Trim data segment if necessary. */
	length = ip.length - ip_len;	/* Length of data portion */
	trim_mbuf(&bp,length);
				
	/* Process options, if any. Also compute length of secondary IP
	 * header in case fragmentation is needed later
	 */
	strict = 0;
	for(opt = ip.options; opt < &ip.options[ip.optlen];opt += opt_len){
		int32 get32();

		/* Most options have a length field. If this is a EOL or NOOP,
		 * this (garbage) value won't be used
		 */
		opt_len = uchar(opt[1]);

		switch(opt[0] & OPT_NUMBER){
		case IP_EOL:
			goto no_opt;	/* End of options list, we're done */
		case IP_NOOP:
			opt_len = 1;
			break;		/* No operation, skip to next option */
		case IP_SSROUTE:	/* Strict source route & record route */
			strict = 1;	/* note fall-thru */
		case IP_LSROUTE:	/* Loose source route & record route */
			/* Source routes are ignored unless we're in the
			 * destination field
			 */
			if(ip.dest != ip_addr)
				break;	/* Skip to next option */
			if(uchar(opt[2]) >= opt_len){
				break;	/* Route exhausted; it's for us */
			}
			/* Put address for next hop into destination field,
			 * put our address into the route field, and bump
			 * the pointer
			 */
			ptr = opt + uchar(opt[2]) - 1;
			ip.dest = get32(ptr);
			put32(ptr,ip_addr);
			opt[2] += 4;
			break;
		case IP_RROUTE:	/* Record route */
			if(uchar(opt[2]) >= opt_len){
				/* Route area exhausted; kick back an error */
				union icmp_args icmp_args;

				icmp_args.pointer = IPLEN + opt - ip.options;
				icmp_output(&ip,bp,PARAM_PROB,0,&icmp_args);
				free_p(bp);
				return -1;
			}
			/* Add our address to the route */
			ptr = opt + uchar(opt[2]) - 1;
			ptr = put32(ptr,ip_addr);
			opt[2] += 4;
			break;
		}
	}
no_opt:

	/* See if it's a broadcast or addressed to us, and kick it upstairs */
	if(ip.dest == ip_addr || rxbroadcast){
#ifdef	GWONLY
	/* We're only a gateway, we have no host level protocols */
		if(!rxbroadcast)
			icmp_output(&ip,bp,DEST_UNREACH,PROT_UNREACH,(union icmp_args *)0);
		free_p(bp);
#else

		/* If this is a local loopback packet, place on the loopback
		 * queue for processing in the main loop. This prevents the
		 * infinite stack recursion and other problems that would
		 * otherwise occur when we talk to ourselves, e.g., with ftp
		 */
		if(ip.source == ip_addr){
			/* Put IP header back on */
			if((tbp = htonip(&ip,bp)) == NULLBUF){
				free_p(bp);
				return -1;
			}
			/* Copy loopback packet into new buffer.
			 * This avoids an obscure problem with TCP which
			 * dups its outgoing data before transmission and
			 * then frees it when an ack comes, even though the
			 * receiver might not have actually read it yet
			 */
			bp = copy_p(tbp,len_mbuf(tbp));
			free_p(tbp);
			if(bp == NULLBUF)
				return -1;
			enqueue(&loopq,bp);
		} else {
			ip_recv(&ip,bp,rxbroadcast);
		}
#endif
		return 0;
	}

	/* Decrement TTL and discard if zero */
	if(--ip.ttl == 0){
		/* Send ICMP "Time Exceeded" message */
		icmp_output(&ip,bp,TIME_EXCEED,0,NULLICMP);
		free_p(bp);
		return -1;
	}
	/* Look up target address in routing table */
	/* Added special null gateway 0.0.0.1 to bounce as unreachable */
	if((rp = rt_lookup(ip.dest)) == NULLROUTE || rp->gateway == (int32)1){
		/* No route exists, (or we interfered) return unreachable message */
		icmp_output(&ip,bp,DEST_UNREACH,HOST_UNREACH,NULLICMP);
		free_p(bp);
		return -1;
	}
	/* Check for output forwarding and divert if necessary */
	iface = rp->interface;
#ifdef OLD_FORWARD
	if(iface->forw != NULLIF)
		iface = iface->forw;
#endif
	/* Find gateway; zero gateway in routing table means "send direct" */
	if(rp->gateway == (int32)0)
		gateway = ip.dest;
	else
		gateway = rp->gateway;

	if(strict && gateway != ip.dest){
		/* Strict source routing requires a direct entry */
		icmp_output(&ip,bp,DEST_UNREACH,ROUTE_FAIL,NULLICMP);
		free_p(bp);
		return -1;
	}
	precedence = PREC(ip.tos);
	delay = ip.tos & DELAY;
	throughput = ip.tos & THRUPUT;
	reliability = ip.tos & RELIABILITY;

	/* let's use metric for something at last */
	mtu = rp->metric ? rp->metric : iface->mtu;
	if(ip.length <= mtu){
		/* Datagram smaller than interface MTU; put header
		 * back on and send normally
		 */
		if((tbp = htonip(&ip,bp)) == NULLBUF){
			free_p(bp);
			return -1;
		}
		return (*iface->send)(tbp,iface,gateway,
			precedence,delay,throughput,reliability);
	}
	/* Fragmentation needed */
	if(ip.fl_offs & DF){
		/* Don't Fragment set; return ICMP message and drop */
		icmp_output(&ip,bp,DEST_UNREACH,FRAG_NEEDED,NULLICMP);
		free_p(bp);
		return -1;
	}
	/* Create fragments */
	offset = (ip.fl_offs & F_OFFSET) << 3;
	mf_flag = ip.fl_offs & MF;	/* Save original MF flag */
	while(length != 0){		/* As long as there's data left */
		int16 fragsize;		/* Size of this fragment's data */
		struct mbuf *f_data;	/* Data portion of fragment */

		/* After the first fragment, should remove those
		 * options that aren't supposed to be copied on fragmentation
		 */
		ip.fl_offs = offset >> 3;
		if(length + ip_len <= mtu){
			/* Last fragment; send all that remains */
			fragsize = length;
			ip.fl_offs |= mf_flag;	/* Pass original MF flag */
		} else {
			/* More to come, so send multiple of 8 bytes */
			fragsize = (mtu - ip_len) & 0xfff8;
			ip.fl_offs |= MF;
		}
		ip.length = fragsize + ip_len;

		/* Move the data fragment into a new, separate mbuf */
		if((f_data = alloc_mbuf(fragsize)) == NULLBUF){
			free_p(bp);
			return -1;
		}
		f_data->cnt = pullup(&bp,f_data->data,fragsize);

		/* Put IP header back on */
		if((tbp = htonip(&ip,f_data)) == NULLBUF){
			free_p(f_data);
			free_p(bp);
			return -1;
		}
		/* and ship it out */
		if((*iface->send)(tbp,iface,gateway,
			precedence,delay,throughput,reliability) == -1)
			return -1;

		offset += fragsize;
		length -= fragsize;
	}
	return 0;
}

struct rt_cache rt_cache;

/* Add an entry to the IP routing table. Returns 0 on success, -1 on failure */
/* don't care much if it fails.  All tests are done by only function that
 * calls this
 */
void
rt_add(target,bits,gateway,metric,interface)
int32 target;		/* Target IP address prefix */
unsigned int bits;	/* Size of target address prefix in bits (0-32) */
int32 gateway;
int metric;
struct interface *interface;
{
	struct route *rp,**hp,*rt_lookup();
	int16 hash_ip(),i;
	int32 mask;

	rt_cache.target = 0;	/* Flush cache */

	/* Zero bits refers to the default route */
	if(bits == 0){
		rp = &r_default;
	} else {
		if(bits > 32)
			bits = 32;

		/* Mask off don't-care bits */
		mask = 0xffffffffl;
		for(i=31;i >= bits;i--)
			mask <<= 1;

		target &= mask;
		/* Search appropriate chain for existing entry */
		for(rp = routes[bits-1][hash_ip(target)];rp != NULLROUTE;rp = rp->next){
			if(rp->target == target)
				break;
		}
	}
	if(rp == NULLROUTE){
		/* The target is not already in the table, so create a new
		 * entry and put it in.
		 */
		if((rp = (struct route *)malloc(sizeof(struct route))) == NULLROUTE)
			return; /* keep 'em guessing */
		/* Insert at head of table */
		rp->prev = NULLROUTE;
		hp = &routes[bits-1][hash_ip(target)];
		rp->next = *hp;
		if(rp->next != NULLROUTE)
			rp->next->prev = rp;
		*hp = rp;
	}
	rp->target = target;
	rp->gateway = gateway;
	rp->metric = metric;
	rp->interface = interface;
}

/* Remove an entry from the IP routing table. Returns 0 on success, -1
 * if entry was not in table.
 */
int
rt_drop(target,bits)
int32 target;
unsigned int bits;
{
	register struct route *rp;
	struct route *rt_lookup();
	unsigned int i;
	int16 hash_ip();
	int32 mask;
	void free();

	rt_cache.target = 0;	/* Flush the cache */

	if(bits == 0){
		/* Nail the default entry */
		r_default.interface = NULLIF;
		return 0;
	}
	if(bits > 32)
		bits = 32;

	/* Mask off don't-care bits */
	mask = 0xffffffffl;
	for(i=31;i >= bits;i--)
		mask <<= 1;

	target &= mask;

	/* Search appropriate chain for existing entry */
	for(rp = routes[bits-1][hash_ip(target)];rp != NULLROUTE;rp = rp->next){
		if(rp->target == target)
			break;
	}
	if(rp == NULLROUTE)
		return -1;	/* Not in table */

	if(rp->next != NULLROUTE)
		rp->next->prev = rp->prev;
	if(rp->prev != NULLROUTE)
		rp->prev->next = rp->next;
	else
		routes[bits-1][hash_ip(target)] = rp->next;

	free((char *)rp);
	return 0;
}

/* Compute hash function on IP address */
static int16
hash_ip(addr)
register int32 addr;
{
	register int16 ret;

	ret = hiword(addr);
	ret ^= loword(addr);
	ret %= NROUTE;
	return ret;
}
#ifndef	GWONLY
/* Given an IP address, return the MTU of the local interface used to
 * reach that destination. This is used by TCP to avoid local fragmentation
 * It is only called by proc_syn() in tcpin.c - K5JB
 */
int16
ip_mtu(addr)
int32 addr;
{
	register struct route *rp;
	struct route *rt_lookup();
	struct interface *iface;
#ifdef AX25
	extern int16 paclen;
#endif
	rp = rt_lookup(addr);
	if(rp == NULLROUTE || rp->interface == NULLIF)
		return 0;

	iface = rp->interface;
#ifdef OLD_FORWARD	/* probably never use this */
	if(iface->forw != NULLIF)
		return iface->forw->mtu;
	else
#endif
		/* let's use metric for something at last */
		return rp->metric ? rp->metric : iface->mtu;
#ifdef notdef
		if(iface->flags == CONNECT_MODE)	/* revised to deal with ax25 segmentation */
			return iface->mtu;		/* in datagram mode we don't want mtu - K5JB */
		else
			return iface->mtu < paclen ? iface->mtu : paclen;
#endif
}
#endif
/* Look up target in hash table, matching the entry having the largest number
 * of leading bits in common. Return default route if not found;
 * if default route not set, return NULLROUTE
 */
static struct route *
rt_lookup(target)
int32 target;
{
	register struct route *rp;
	int16 hash_ip();
	int bits;
	int32 tsave;
	int32 mask;

	if(target == rt_cache.target)
		return rt_cache.route;

	tsave = target;

	mask = ~0;	/* All ones */
	for(bits = 31;bits >= 0; bits--){
		target &= mask;
		for(rp = routes[bits][hash_ip(target)];rp != NULLROUTE;rp = rp->next){
			if(rp->target == target){
				/* Stash in cache and return */
				rt_cache.target = tsave;
				rt_cache.route = rp;
				return rp;
			}
		}
		mask <<= 1;
	}
	if(r_default.interface != NULLIF){
		rt_cache.target = tsave;
		rt_cache.route = &r_default;
		return &r_default;
	} else
		return NULLROUTE;
}
/* Convert IP header in host format to network mbuf */
struct mbuf *
htonip(ip,data)
struct ip *ip;
struct mbuf *data;
{
	int16 hdr_len;
	struct mbuf *bp;
	register char *cp;
	int16 checksum,cksum();
	char *memcpy();

	hdr_len = IPLEN + ip->optlen;
	if((bp = pushdown(data,hdr_len)) == NULLBUF){
		free_p(data);
		return NULLBUF;
	}
	cp = bp->data;

	*cp++ = (IPVERSION << 4) | (hdr_len >> 2);
	*cp++ = ip->tos;
	cp = put16(cp,ip->length);
	cp = put16(cp,ip->id);
	cp = put16(cp,ip->fl_offs);
	*cp++ = ip->ttl;
	*cp++ = ip->protocol;
	cp = put16(cp,0);	/* Clear checksum */
	cp = put32(cp,ip->source);
	cp = put32(cp,ip->dest);
	if(ip->optlen != 0)
		(void)memcpy(cp,ip->options,ip->optlen);

	/* Compute checksum and insert into header */
	checksum = cksum(NULLHEADER,bp,hdr_len);
	put16(&bp->data[10],checksum);

	return bp;
}
/* Extract an IP header from mbuf */
int
ntohip(ip,bpp)
struct ip *ip;
struct mbuf **bpp;
{
	char v_ihl;
	int16 ihl;

	v_ihl = pullchar(bpp);
	ip->version = (v_ihl >> 4) & 0xf;
	ip->tos = pullchar(bpp);
	ip->length = pull16(bpp);
	ip->id = pull16(bpp);
	ip->fl_offs = pull16(bpp);
	ip->ttl = pullchar(bpp);
	ip->protocol = pullchar(bpp);
	(void)pull16(bpp);	/* Toss checksum */
	ip->source = pull32(bpp);
	ip->dest = pull32(bpp);

	ihl = (v_ihl & 0xf) << 2;
	if(ihl < IPLEN){
		/* Bogus packet; header is too short */
		return -1;
	}
	ip->optlen = ihl - IPLEN;
	if(ip->optlen != 0)
		pullup(bpp,ip->options,ip->optlen);

	return ip->optlen + IPLEN;
}
/* Perform end-around-carry adjustment */
int16
eac(sum)
register int32 sum;	/* Carries in high order 16 bits */
{
	register int16 csum;

	while((csum = sum >> 16) != 0)
		sum = csum + (sum & 0xffffL);
	return (int16) (sum & 0xffffl);	/* Chops to 16 bits */
}
/* Checksum a mbuf chain, with optional pseudo-header */
int16
cksum(ph,m,len)
struct pseudo_header *ph;
register struct mbuf *m;
int16 len;
{
	register unsigned int cnt, total;
	register int32 sum, csum;
	register char *up;
	int16 csum1;
	int swap = 0;
	int16 lcsum();

	sum = 0l;

	/* Sum pseudo-header, if present */
	if(ph != NULLHEADER){
		sum = hiword(ph->source);
		sum += loword(ph->source);
		sum += hiword(ph->dest);
		sum += loword(ph->dest);
#ifdef	WEGA
		sum += (int)ph->protocol;
#else
		sum += uchar(ph->protocol);
#endif
		sum += ph->length;
	}
	/* Now do each mbuf on the chain */
	for(total = 0; m != NULLBUF && total < len; m = m->next) {
		cnt = min(m->cnt, len - total);
		up = (char *)m->data;
		csum = 0;

		if(((long)up) & 1){
			/* Handle odd leading byte */
			if(swap)
#ifdef	WEGA
				csum = (int)(*up++);
#else
				csum = uchar(*up++);
#endif
			else
#ifdef	WEGA
				csum = (int16)((int)(*up++) << 8);
#else
				csum = (int16)(uchar(*up++) << 8);
#endif
			cnt--;
			swap = !swap;
		}
		if(cnt > 1){
			/* Have the primitive checksumming routine do most of
			 * the work. At this point, up is guaranteed to be on
			 * a short boundary
			 */
			csum1 = lcsum((unsigned short *)up, cnt >> 1);
			if(swap)
				csum1 = (csum1 << 8) | (csum1 >> 8);
			csum += csum1;
		}
		/* Handle odd trailing byte */
		if(cnt & 1){
			if(swap)
#ifdef	WEGA
			{
				unsigned short tmp;

				tmp = (unsigned short)up[--cnt];
				tmp <<= 8;
				tmp >>= 8;
				csum += tmp;
			}
#else
				csum += uchar(up[--cnt]);
#endif
			else
#ifdef	WEGA
				csum += (int16)((int)(up[--cnt]) << 8);
#else
				csum += (int16)(uchar(up[--cnt]) << 8);
#endif
			swap = !swap;
		}
		sum += csum;
		total += m->cnt;
	}
	/* Do final end-around carry, complement and return */
	return ~eac(sum) & 0xffff;
}
/* Machine-independent, alignment insensitive network-to-host long conversion */
static int32
get32(cp)
register char *cp;
{
	int32 rval;

	rval = uchar(*cp++);
	rval <<= 8;
	rval |= uchar(*cp++);
	rval <<= 8;
	rval |= uchar(*cp++);
	rval <<= 8;
	rval |= uchar(*cp);

	return rval;
}
