/* Low level AX.25 frame processing - address header */
/* added declaration to axarp and reformatted 9/7/91 */
/* Completed the SID2 ax25 mbxcall function 3/12/92 */
/* Added an IP only ax25 call sign as an alternative to vckt stuff 11/13/93 */

#include "options.h"
#include "config.h"	/*K5JB*/
#ifdef AX25
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "mbuf.h"
#include "iface.h"
#include "timer.h"
#include "arp.h"
#include "slip.h"
#include "ax25.h"
#include "lapb.h"
#include <ctype.h>

#ifdef AX25_HEARD
#include <time.h>
#include "heard.h"
#endif

#ifdef MULPORT
extern int mport;
#endif

/* AX.25 broadcast address: "QST-0" in shifted ascii */
struct ax25_addr ax25_bdcst = {
	'Q'<<1, 'S'<<1, 'T'<<1, ' '<<1, ' '<<1, ' '<<1,
	('0'<<1) | E
};

char axbdcst[AXALEN];    /* Same thing, network format */
struct ax25_addr mycall;
int digipeat = 1;   /* Controls digipeating */

#ifdef SID2
struct ax25_addr bbscall;
#endif
#ifdef VCIP_SSID
struct ax25_addr ip_call;
#endif

#ifdef SEGMENT
/* AX.25 transmit frame segmenter used by NOS. Returns queue of segmented
 * fragments, or original packet if small enough.  Creates paclen - 1 frames.
 */
struct mbuf *
segmenter(bp,ssize)
struct mbuf *bp;	/* Complete packet */
int16 ssize;		/* Max size of frame segments */
{
	void free_q();
	struct mbuf *result = NULLBUF;
	struct mbuf *bptmp, *bp1;
	int16 len,offset;
	int segments;

	/* See if packet is too small to segment. Note 1-byte grace factor
	 * so the PID will not cause segmentation of a 256-byte IP datagram.
	 */
	len = len_mbuf(bp);
	if(len <= ssize+1)
		return bp;	/* Too small to segment */

	ssize -= 2;		/* ssize now equal to data portion size */
	segments = 1 + (len - 1) / ssize;	/* # segments  */
	offset = 0;

	while(segments != 0){
		offset += dup_p(&bptmp,bp,offset,ssize);
		if(bptmp == NULLBUF){
			free_q(&result);
			break;
		}
		/* Make room for segmentation header */
		if((bp1 = pushdown(bptmp,2)) == NULLBUF){
			free_p(bptmp);
			free_q(&result);
		}
		bp1->data[0] = PID_SEGMENT;
		bp1->data[1] = --segments;
		if(offset == ssize)
			bp1->data[1] |= SEG_FIRST;
		enqueue(&result,bp1);
	}
	free_p(bp);
	return result;
}
#endif /* SEGMENT */

/* Send IP datagrams across an AX.25 link */
int
ax_send(bp,interface,gateway,precedence,delay,throughput,reliability)
struct mbuf *bp;
struct interface *interface;
int32 gateway;
char precedence;
char delay;
char throughput;
char reliability;
{
	char *hw_addr,*res_arp();
	struct ax25_cb *axp,*find_ax25(),*open_ax25();
	struct ax25 addr;
	struct ax25_addr destaddr;
	struct mbuf *tbp;
	extern int16 axwindow;
	void ax_incom();
#ifndef SEGMENT
	int16 size;
#endif
	int atohax25();
	void send_ax25();
	/* For ARP_VAX25, we have boogered res_arp to not enqueue UI frames and */
	int use_vc = 0;		/* call arp_output */
	if((hw_addr = res_arp(interface,ARP_VAX25,gateway,bp)) != NULLCHAR)
		use_vc = 1;
	else if((hw_addr = res_arp(interface,ARP_AX25,gateway,bp)) == NULLCHAR)
		return 0; /* Wait for address resolution */

	if(!use_vc)
		if(delay || (!reliability && (interface->flags == DATAGRAM_MODE)))
			/* Use UI frame */
			return (*interface->output)(interface,hw_addr,
				interface->hwaddr,PID_IP,bp);

	/* Reliability is needed; use I-frames in AX.25 connection */
	memcpy(destaddr.call,hw_addr,ALEN);
	destaddr.ssid = hw_addr[ALEN];

	if((axp = find_ax25(&destaddr)) == NULLAX25 ||
			(axp->state != CONNECTED && axp->state!=RECOVERY)){
		/*NOTE: ADDED W9NK's SABM FIX OF && AXP->STATE!=RECOVERY GRC*/
		/* Open a new connection or reinitialize the old one */
#ifdef VCIP_SSID
		if(use_vc)
			atohax25(&addr,hw_addr,&ip_call);
		else
#endif
			atohax25(&addr,hw_addr,(struct ax25_addr *)interface->hwaddr);
		axp = open_ax25(&addr,axwindow,ax_incom,NULLVFP,NULLVFP,interface,(char *)0);
		if(axp == NULLAX25){
			free_p(bp);
			return -1;
		}
	}
#ifdef SEGMENT	/* NOS style segmenter for ax.25 */
	if((tbp = pushdown(bp,1)) == NULLBUF){
		free_p(bp);
		return -1;
	}
	bp = tbp;
	bp->data[0] = PID_IP;
	if((tbp = segmenter(bp,axp->paclen)) == NULLBUF){
		free_p(bp);
		return -1;
	}
	send_ax25(axp,tbp);
	return 0;	/* nobody uses this */
#else
	/* Allocate buffer, allowing space for PID - assume mbuf smaller
	 * than paclen, enforced elsewhere
	 */
	if((tbp = alloc_mbuf((size = len_mbuf(bp)) + 1)) == NULLBUF)
		return 0;         /* out of memory! What should we return? */
	*tbp->data = PID_IP;

	tbp->cnt = 1;
	tbp->cnt += pullup(&bp,tbp->data + 1,size);
	send_ax25(axp,tbp);
	return 0;
#endif /* SEGMENT */
}

/* Add AX.25 link header and send packet.
 * Note that the calling order here must match ec_output
 * since ARP also uses it.
 */
int
ax_output(interface,dest,source,pid,data)
struct interface *interface;
char *dest;				/* Destination AX.25 address (7 bytes, shifted) */
							/* Also includes digipeater string */
char *source;			/* Source AX.25 address (7 bytes, shifted) */
unsigned char pid;				/* Protocol ID */
struct mbuf *data;	/* Data field (follows PID) */
{
	struct mbuf *abp,*cbp,*htonax25();
	struct ax25 addr;

	/* Allocate mbuf for control and PID fields, and fill in */
	if((cbp = pushdown(data,2)) == NULLBUF){
		free_p(data);
		return -1;
	}
	cbp->data[0] = UI;
	cbp->data[1] = pid;

	atohax25(&addr,dest,(struct ax25_addr *)source);
	if((abp = htonax25(&addr,cbp)) == NULLBUF){
		free_p(cbp);   /* Also frees data */
		return -1;
	}
#ifdef OLD_FORWARD
	/* This shouldn't be necessary because redirection has already been
	 * done at the IP router layer, but just to be safe...
	 */
	if(interface->forw != NULLIF)
		return (*interface->forw->raw)(interface->forw,abp);
	else
#endif
		return (*interface->raw)(interface,abp);
}

/* Process incoming AX.25 packets.
 * After optional tracing, the address field is examined. If it is
 * directed to us as a digipeater, repeat it.  If it is addressed to
 * us or to QST-0, kick it upstairs depending on the protocol ID.
 */
int
ax_recv(interface,bp)
struct interface *interface;
struct mbuf *bp;
{
#ifdef MULPORT
	struct interface *repeater();
#endif
	void arp_input();
	int ip_route(),ntohax25(),addreq(),lapb_input();
	struct ax25_addr *ap;
	struct mbuf *htonax25(),*hbp;
	char multicast = 0;
#ifdef SID2
	extern int ax25mbox;
	int isbbscall = 0;
	void mbx_state();
#endif
#ifdef AX25_HEARD
/* heard stuff */
	struct heard_stuff *hp;
	extern struct ax25_heard heard;
	int16 type, ftype();
	int prev, curr;
/* heard stuff */
#endif
#ifdef NETROM
	int nrnodes = 0;
	int nr_nodercv();
#endif
	unsigned char control;
	struct ax25 hdr;
	struct ax25_cb *axp,*find_ax25(),*cr_ax25();
	struct ax25_addr ifcall;
	extern struct ax25_addr nr_nodebc ;

	/* Use the address associated with this interface */
	memcpy(ifcall.call,interface->hwaddr,ALEN);
	ifcall.ssid = interface->hwaddr[ALEN];

#ifdef FORWARD
	if(interface->forw != NULLIF){
		struct mbuf *tbp;

		dup_p(&tbp,bp,0,len_mbuf(bp));
		if(tbp == NULLBUF)
			return;
		(*interface->forw->raw)(interface->forw,tbp);
	}
#endif
	/* Pull header off packet and convert to host structure */
	if(ntohax25(&hdr,&bp) < 0){
		/* Something wrong with the header */
		free_p(bp);
		return;
	}
#ifdef AX25_HEARD
	/* heard stuff - note that the heard structure is 1602 bytes */
	if (heard.enabled) {
	/* scan heard list.  if not there, add it */
		prev = -1;
		curr = heard.first;
		while (1) {
			if (curr != -1) {
				hp = &heard.list[curr];
				if(addreq(&hp->info.source, &hdr.source) &&
						addreq(&hp->info.dest, &hdr.dest) &&	/* done for W6SWE */
						hp->ifacename == interface->name){
					time(&hp->htime);
					/* if not the first, make it so */
					if (prev != -1) {
						heard.list[prev].next = hp->next;
						hp->next = heard.first;
						heard.first = curr;
					}
					break;
				}
			}

	/* if the last one, add another if room, or overwrite it */
			if (curr == -1 || hp->next == -1) {
   /* not in heard list, add it.  if no room, bump oldest */
				if (heard.cnt < MAX_HEARD) {
					heard.list[heard.cnt].next = heard.first;
					heard.first = heard.cnt;
					hp = &heard.list[heard.cnt++];
				} else {
					heard.list[prev].next = -1;
					heard.list[curr].next = heard.first;
					heard.first = curr;
					hp = &heard.list[curr];
				}
				hp->flags = 0;
				memcpy(&hp->info, &hdr, sizeof(struct ax25));
				time(&hp->htime);
				hp->ifacename = interface->name;
				break;
			}
			prev = curr;
			curr = hp->next;
		} /* while */

/* figure out what the other station is running */
		if (bp->cnt >= 2) {
			type = ftype(*bp->data);
			if(type == I || type == UI){
				switch(*(bp->data + 1)){	/* note that these accumulate */
					case PID_ARP:						/* for a given "from" call */
						hp->flags |= HEARD_ARP;
						break;
					case PID_NETROM:
						hp->flags |= HEARD_NETROM;
						break;
					case PID_IP:
						hp->flags |= HEARD_IP;
						break;
					case PID_ROSE1:
						hp->flags |= HEARD_ROSE;
						break;
				}
			}
		}
	} else {	/* heard not enabled */
		hp = NULL;
	}
#endif /* heard stuff */

	/* Scan, looking for our call in the repeater fields, if any.
	 * Repeat appropriate packets.
	 */
	for(ap = &hdr.digis[0]; ap < &hdr.digis[hdr.ndigis]; ap++){
		if(ap->ssid & REPEATED)
			continue; /* Already repeated */
		/* Check if packet is directed to us as a digipeater */
		if(digipeat && addreq(ap,&ifcall)){
			/* Yes, kick it back out */
#ifdef MULPORT
/****************************************************************************
*                 Multiport repeater hack by KE4ZV                          *
****************************************************************************/
			if (mport){
				interface=repeater(ap,interface,&hdr);
			}
/****************************************************************************
*                  Multiport repeater hack ends                             *
****************************************************************************/
#endif
			ap->ssid |= REPEATED;
			if((hbp = htonax25(&hdr,bp)) != NULLBUF){
#ifdef OLD_FORWARD
				if(interface->forw != NULLIF)
					(*interface->forw->raw)(interface->forw,hbp);
				else
#endif
					(*interface->raw)(interface,hbp);
				bp = NULLBUF;
			}
		}
		free_p(bp);    /* Dispose if not forwarded */
		return;
	} /* for */
	/* Packet has passed all repeaters, now look at destination */
	if(addreq(&hdr.dest,&ax25_bdcst))
		multicast = 1; /* Broadcast packet */
	else
#ifdef SID2
		if(ax25mbox && addreq(&hdr.dest,&bbscall))
			isbbscall = 1;
		else
#endif
		if(addreq(&hdr.dest,&ifcall))
			; /* Packet directed at us */
		else
#ifdef NETROM
		if(addreq(&hdr.dest,&nr_nodebc))
			nrnodes = 1 ;
		else
#endif
#ifdef VCIP_SSID
		if(addreq(&hdr.dest,&ip_call))  /* our IP only call */
			;       /* keep it -- probably coming from ROSE. lapb will discard
						* it if it has F0 PID
						*/
		else
#endif
		{
		/* Flunked all the if-else tests, No interest to us */
			free_p(bp);
			return;
		}

	if(bp == NULLBUF)
		/* Nothing left */
		return;

	  /* Sneak a peek at the control field. This kludge is necessary because
	 * AX.25 lacks a proper protocol ID field between the address and LAPB
	 * sublayers; a control value of UI indicates that LAPB is to be
	 * bypassed.
	 */
	control = *bp->data & ~PF;
	if(control == UI){
		unsigned char pid;

		(void)pullup(&bp,&pid,1);	/* toss the first character */
		if(pullup(&bp,&pid,1) != 1)
			return;        /* No PID */
#ifdef NETROM
		/* NET/ROM is very poorly layered. The meaning of the stuff
		 * following the PID of CF depends on what's in the AX.25 dest
		 * field.
		 */
		if(nrnodes){
			if(pid == PID_NETROM)
				nr_nodercv(interface,&hdr.source,bp) ;
			else      /* regular UI packets to "nodes" aren't for us */
				free_p(bp) ;
			return ;
		}
#endif
			 /* Handle packets. Multi-frame messages are not allowed */
		switch(pid){
		case PID_IP:
			ip_route(bp,multicast);
			break;
		case PID_ARP:
			arp_input(interface,bp);
			break;
		default:
			free_p(bp);
			break;
		}
		return;
	}
	/* Everything from here down is LAPB stuff, so drop anything
	 * not directed to us:
	 */

#ifdef NETROM
	if (multicast || nrnodes) {
		free_p(bp) ;
		return ;
	}
#else
	if (multicast) {
		free_p(bp) ;
		return ;
	}
#endif

	/* Find the source address in hash table */
	if((axp = find_ax25(&hdr.source)) == NULLAX25){
		/* Create a new ax25 entry for this guy,
		 * insert into hash table keyed on his address,
		 * and initialize table entries
		 */
		if((axp = cr_ax25(&hdr.source)) == NULLAX25){
			free_p(bp);
			return;
		}
#ifdef SID2	/* replace what was put in s_upcall by cr_ax25() */
		if(isbbscall)
			axp->s_upcall = mbx_state;
#endif
		axp->interface = interface;
		/* Swap source and destination, reverse digi string */
		ASSIGN(axp->addr.dest,hdr.source);
		ASSIGN(axp->addr.source,hdr.dest);
		if(hdr.ndigis > 0){
			int i,j;

			/* Construct reverse digipeater path */
			for(i=hdr.ndigis-1,j=0;i >= 0;i--,j++){
				ASSIGN(axp->addr.digis[j],hdr.digis[i]);
				axp->addr.digis[j].ssid &= ~(E|REPEATED);
			}
			/* Scale timers to account for extra delay */
			/* Note, this is not as agressive as typical TNCs */
			axp->t1.start *= hdr.ndigis+1;
/* we don't scale these
			axp->t2.start *= hdr.ndigis+1;
			axp->t3.start *= hdr.ndigis+1;
*/
		}
		axp->addr.ndigis = hdr.ndigis;
	}
	if(hdr.cmdrsp == UNKNOWN)
		axp->proto = V1;    /* Old protocol in use */
	else
		axp->proto = V2;
	lapb_input(axp,hdr.cmdrsp,bp);
}

/* General purpose AX.25 frame output */
/* Everything comes here where decision on whether to use AX.25 V1 or V2
 * is made.  Previously, V2 was hard coded into this thing - K5JB
 */
int
sendframe(axp,cmdrsp,ctl,data)
struct ax25_cb *axp;
char cmdrsp;
char ctl;
struct mbuf *data;
{
	struct mbuf *hbp,*cbp,*htonax25();
	int i;

	if(axp == NULLAX25 || axp->interface == NULLIF)
		return -1;

	/* Add control field */
	if((cbp = pushdown(data,1)) == NULLBUF){
		free_p(data);
		return -1;
	}
	cbp->data[0] = ctl;
	if(axp->proto == V1)
		axp->addr.cmdrsp = UNKNOWN;
	else
		if(cmdrsp == C_NOPOLL)  /* an I frame we don't want PF set on */
			axp->addr.cmdrsp = COMMAND;
		else{	/* do what caller wants with all other frames */
			cbp->data[0] |= PF;
			axp->addr.cmdrsp = cmdrsp;
		}
	/* Create address header */
	if((hbp = htonax25(&axp->addr,cbp)) == NULLBUF){
		free_p(cbp);
		return -1;
	}
	/* The packet is all ready, now send it */
#ifdef OLD_FORWARD	/* nobody would want to use this */
	if(axp->interface->forw != NULLIF)
		i = (*axp->interface->forw->raw)(axp->interface->forw,hbp);
	else
#endif
		i = (*axp->interface->raw)(axp->interface,hbp);

	return i;
}

/* Initialize AX.25 entry in arp device table */
void
axarp()
{
	int psax25(),setpath(),arp_init();

	memcpy(axbdcst,ax25_bdcst.call,ALEN);
	axbdcst[ALEN] = ax25_bdcst.ssid;

	arp_init(ARP_AX25,AXALEN,PID_IP,PID_ARP,axbdcst,psax25,setpath);
}
#endif /* AX25 */
