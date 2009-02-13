/* Send and receive IP datagrams on serial lines. Compatible with SLIP
 * under Berkeley Unix.
 */
#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "iface.h"
#include "ax25.h"
#include "slip.h"
#include "asy.h"
#include "options.h"	/* In case SERIALTEST has changed */
#include "config.h"
#ifdef TRACE
#include "trace.h"
#endif
#if defined(MSDOS) && defined(SERIALTEST)
#include "8250.h"
#endif

int16 maxslipq = 8;	/* ms-dos version can tune this with mem s<n> */
int16 slipbalks;	/* reported in memstat */
int16 sliphiwater;
int asy_ioctl();
int kiss_ioctl();
int slip_send();
void doslip();
int asy_output();

/* Slip level control structure */
struct slip slip[ASY_MAX];

/* Send routine for point-to-point slip
 * This is a trivial function since there is no slip link-level header
 */
int
slip_send(data,interface,gateway,precedence,delay,throughput,reliability)
struct mbuf *data;		/* Buffer to send */
struct interface *interface;	/* Pointer to interface control block */
int32 gateway;			/* Ignored (SLIP is point-to-point) */
char precedence;
char delay;
char throughput;
char reliability;
{
	if(interface == NULLIF){
		free_p(data);
		return -1;
	}
#ifdef TRACE
	dump(interface,IF_TRACE_OUT,TRACE_IP,data);
#endif
	return (*interface->raw)(interface,data);
}
/* Send a raw slip frame -- also trivial */
int
slip_raw(interface,data)
struct interface *interface;
struct mbuf *data;
{
	/* Queue a frame on the slip output queue and start transmitter */
	return slipq(interface->dev,data);
}
/* Encode a raw packet in slip framing, put on link output queue, and kick
 * transmitter
 */
static int
slipq(dev,data)
int16 dev;		/* Serial line number */
struct mbuf *data;	/* Buffer to be sent */
{
	register struct slip *sp;
	struct mbuf *slip_encode(),*bp;
	void asy_start();

	sp = &slip[dev];
	if(sp->sndcnt > sliphiwater)
		sliphiwater = sp->sndcnt;
	/* this was added as defense from ax.25 station with diarrhea - K5JB */
	if(sp->sndcnt >= maxslipq){
		free_p(data);
		slipbalks++;	/* this is reported with ip stat */
		sliphiwater = 0;	/* reset this, we know it went over high water mark */
		return -1;
	}

	if((bp = slip_encode(data)) == NULLBUF)
		return -1;

	enqueue(&sp->sndq,bp);
	sp->sndcnt++;
	if(sp->tbp == NULLBUF)
		asy_start(dev);
	return 0;
}
/* Start output, if possible, on asynch device dev */
static void
asy_start(dev)
int16 dev;
{
	register struct slip *sp;

	if(!stxrdy(dev))
		return;		/* Transmitter not ready */

	sp = &slip[dev];
	if(sp->tbp != NULLBUF){
		/* transmission just completed */
		free_p(sp->tbp);
		sp->tbp = NULLBUF;
	}
	if(sp->sndq == NULLBUF)
		return;	/* No work */

	sp->tbp = dequeue(&sp->sndq);
	sp->sndcnt--;
	asy_output(dev,sp->tbp->data,sp->tbp->cnt);
}
/* Encode a packet in SLIP format */
struct mbuf *	/* was static but need for COMBIOS - K5JB */
slip_encode(bp)
struct mbuf *bp;
{
	struct mbuf *lbp;	/* Mbuf containing line-ready packet */
	register char *cp;
	char c;

	/* Allocate output mbuf that's twice as long as the packet.
	 * This is a worst-case guess (consider a packet full of FR_ENDs!)
	 */
	lbp = alloc_mbuf(2*len_mbuf(bp) + 2);
	if(lbp == NULLBUF){
		/* No space; drop */
		free_p(bp);
		return NULLBUF;
	}
	cp = lbp->data;

	/* Flush out any line garbage */
	*cp++ = FR_END;

	/* Copy input to output, escaping special characters */
	while(pullup(&bp,&c,1) == 1){
		switch(uchar(c)){
		case FR_ESC:
			*cp++ = FR_ESC;
			*cp++ = T_FR_ESC;
			break;
		case FR_END:
			*cp++ = FR_ESC;
			*cp++ = T_FR_END;
			break;
		default:
			*cp++ = c;
		}
	}
	*cp++ = FR_END;
	lbp->cnt = cp - lbp->data;
	return lbp;
}
/* Process incoming bytes in SLIP format
 * When a buffer is complete, return it; otherwise NULLBUF
 */
struct mbuf *	/* was static but need for COMBIOS - K5JB */
slip_decode(dev,c)
int16 dev;	/* Slip unit number */
char c;		/* Incoming character */
{
	struct mbuf *bp;
	register struct slip *sp;

	sp = &slip[dev];
	switch(uchar(c)){
	case FR_END:
		bp = sp->rbp;
		sp->rbp = NULLBUF;
		sp->rcnt = 0;
		return bp;	/* Will be NULLBUF if empty frame */
	case FR_ESC:
		sp->escaped = 1;
		return NULLBUF;
	}
	if(sp->escaped){
		/* Translate 2-char escape sequence back to original char */
		sp->escaped = 0;
		switch(uchar(c)){
		case T_FR_ESC:
			c = FR_ESC;
			break;
		case T_FR_END:
			c = FR_END;
			break;
		default:
			sp->errors++;	/* I don't think this is used anywhere */
			break;
		}
	}
	/* We reach here with a character for the buffer;
	 * make sure there's space for it
	 */
	if(sp->rbp == NULLBUF){
		/* Allocate first mbuf for new packet */
		if((sp->rbp1 = sp->rbp = alloc_mbuf(SLIP_ALLOC)) == NULLBUF)
			return NULLBUF; /* No memory, drop */
		sp->rcp = sp->rbp->data;
	} else if(sp->rbp1->cnt == SLIP_ALLOC){
		/* Current mbuf is full; link in another */
		if((sp->rbp1->next = alloc_mbuf(SLIP_ALLOC)) == NULLBUF){
			/* No memory, drop whole thing */
			free_p(sp->rbp);
			sp->rbp = NULLBUF;
			sp->rcnt = 0;
			return NULLBUF;
		}
		sp->rbp1 = sp->rbp1->next;
		sp->rcp = sp->rbp1->data;
	}
	/* Store the character, increment fragment and total
	 * byte counts
	 */
	*sp->rcp++ = c;
	sp->rbp1->cnt++;
	sp->rcnt++;
	return NULLBUF;
}
/* Process SLIP line I/O */
void
doslip(interface)
struct interface *interface;
{
	char c;
	struct mbuf *bp;
	int16 dev;
	int16 asy_recv();

	dev = interface->dev;
	/* Process any pending input */
	while(asy_recv(dev,&c,1) != 0)
		if((bp = slip_decode(dev,c)) != NULLBUF)
			(*slip[dev].recv)(interface,bp);

	/* Kick the transmitter if it's idle */
	if(stxrdy(dev))
		asy_start(dev);
}

/* Unwrap incoming SLIP packets -- trivial operation since there's no
 * link level header
 */
void
slip_recv(interface,bp)
struct interface *interface;
struct mbuf *bp;
{
	int ip_route();

#if defined(MSDOS) && defined(SERIALTEST)
/* Also done in kiss.c, kpc.c and nrs.c to clear error flag possibly
 * set in the async handler
 */
	extern int16 serial_err;        /* defined in ipcmd.c */
	extern int32 tot_rframes;
	struct fifo *fp;

	tot_rframes++;
	fp = &asy[interface->dev].fifo;
	if(fp->error){  /* contains 8250 line status register */
		serial_err++;
		fp->error = 0;
		free_p(bp);
		return;
	}
#endif


	/* By definition, all incoming packets are "addressed" to us */
#ifdef TRACE
	dump(interface,IF_TRACE_IN,TRACE_IP,bp);
#endif
	ip_route(bp,0);
}
