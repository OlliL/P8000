#include "config.h"
#ifdef AX25
#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "ax25.h"
#include "timer.h"
#include "netuser.h"
#ifdef NETROM
#include "nr4.h"
#endif
#include "session.h"
#include "tcp.h"	/* K5JB */
#include "ftp.h"
#include "telnet.h"
#include "iface.h"
#include "finger.h"
#include "lapb.h"

/* miscellaneous function declarations - K5JB */

void free_q(),start_timer(),tx_enq(),disc_stale(),del_ax25(),frmr(),
	stop_timer();
int sendctl(),sendframe();

/* Called whenever timer T1 expires */
void
recover(n)
int *n;
{
	register struct ax25_cb *axp;
	void lapbstate();

	axp = (struct ax25_cb *)n;

	switch(axp->state){
	case SETUP:
		if(axp->n2 != 0 && axp->retries == axp->n2){
			free_q(&axp->txq);
			lapbstate(axp,DISCONNECTED);
		} else {
			axp->retries++;
			sendctl(axp,COMMAND,SABM);
			start_timer(&axp->t1);
		}
		break;
	case DISCPENDING:
		if(axp->n2 != 0 && axp->retries == axp->n2){
			lapbstate(axp,DISCONNECTED);
		} else {
			axp->retries++;
			sendctl(axp,COMMAND,DISC);
			start_timer(&axp->t1);
		}
		break;
	case CONNECTED:
		axp->retries = 0;
	case RECOVERY:	/* note fall-thru */
		if(axp->n2 != 0 && axp->retries == axp->n2){
			/* Give up */
			sendctl(axp,RESPONSE,DM);
			free_q(&axp->txq);
			lapbstate(axp,DISCONNECTED);
		} else {
			/* Transmit poll */
			tx_enq(axp);
			axp->retries++;
			lapbstate(axp,RECOVERY);
		}
		break;
	case FRAMEREJECT:
		if(axp->n2 != 0 && axp->retries == axp->n2){
			sendctl(axp,RESPONSE,DM);
			free_q(&axp->txq);
			lapbstate(axp,DISCONNECTED);
		} else {
			frmr(axp,0,0);	/* Retransmit last FRMR */
			start_timer(&axp->t1);
			axp->retries++;
		}
		break;
	}
	/* Empty the trash */
	if(axp->state == DISCONNECTED)
		del_ax25(axp);
}

/* Start data transmission on link, if possible
 * Return number of frames sent -- This is called on T2 expiration
 * Moved from lapb.c.  Replaces previous T2 scheme which only controlled
 * acks and was ineffective.
 */
void
dlapb_output(axp)
struct ax25_cb *axp;
{
	register struct mbuf *bp;
	struct mbuf *tbp;
	char control;
	int sent = 0;
	int i;

	/* wait until now for this in case something went bad before t2 timed
	 * out
	 */
	if(axp == NULLAX25
	 || (axp->state != RECOVERY && axp->state != CONNECTED)
#ifdef notdef	/* k37 */
	 || axp->remotebusy
#endif
	)
		return;

	if(axp->remotebusy){	/* start polling until RNRs stop, or we retry out k37 */
		if(!run_timer(&axp->t1))
			start_timer(&axp->t1);
		return;
	}
	/* Dig into the send queue for the first unsent frame */
	bp = axp->txq;
	for(i = 0; i < axp->unack; i++){
		if(bp == NULLBUF)
			break;	/* Nothing to do */
		bp = bp->anext;
	}

	/* Start at first unsent I-frame, stop when either the
	 * number of unacknowledged frames reaches the maxframe limit,
	 * or when there are no more frames to send
	 */
	while(bp != NULLBUF && axp->unack < axp->maxframe){
		control = I | (axp->vs++ << 1) | (axp->vr << 5);
		axp->vs &= MMASK;
		dup_p(&tbp,bp,0,len_mbuf(bp));
		if(tbp == NULLBUF)
			return;	/* Probably out of memory */
		/* this had COMMAND, gonna try something else */
		bp = bp->anext;
		axp->unack++;
#ifdef FINALPOLL	/* if you wanna play play "monkey see, monkey do" */
		/* Will put PF bit on last one in set.  NET prev. didn't do this */
		sendframe(axp,bp == NULLBUF || axp->unack == axp->maxframe ? COMMAND :
			C_NOPOLL,control,tbp);
#else
		sendframe(axp,C_NOPOLL,control,tbp);
#endif
		start_timer(&axp->t4);
		/* We're implicitly acking any data he's sent, so stop any
		 * delayed ack
		 */
		axp->response = 0;
		if(!run_timer(&axp->t1)){
			stop_timer(&axp->t3);
			start_timer(&axp->t1);
		}
		sent++;
	}
	if(axp->response != 0)
		sendctl(axp,RESPONSE,axp->response);
	axp->response = 0;

	return;
}

/* Send a poll (S-frame command with the poll bit set) - the t3 function */
void
pollthem(n)
int *n;
{
	register struct ax25_cb *axp;
	void lapbstate();

	axp = (struct ax25_cb *)n;
	if(axp->proto == V1)
		return;	/* Not supported in the old protocol */
	switch(axp->state){
	case CONNECTED:
		axp->retries = 0;
		tx_enq(axp);
		lapbstate(axp,RECOVERY);
		break;
	}
}

void
disc_stale(p)	/* same as NOS's redundant(), T4's function */
char *p;
{
	register struct ax25_cb *axp;
	void lapbstate();

	axp = (struct ax25_cb *)p;
	switch(axp->state){
	case CONNECTED:
	case RECOVERY:
		axp->retries = 0;
		sendctl(axp,COMMAND,DISC);
		start_timer(&axp->t1);
		free_q(&axp->txq);
		lapbstate(axp,DISCPENDING);
		break;
	}
}

/* Transmit query - called on T1 or T3 expiration */
void
tx_enq(axp)
register struct ax25_cb *axp;
{
	char ctl;
	struct mbuf *bp;

#ifdef notdef /* old method */
	/* this correction does what I think author intended - K5JB */
	if(axp->txq != NULLBUF && !axp->remotebusy &&
			(axp->proto == V1 || len_mbuf(axp->txq) < axp->pthresh) ){}
#endif
	/* Changed with k37 */
	if(axp->txq != NULLBUF && !axp->remotebusy &&
			(len_mbuf(axp->txq) < axp->pthresh || axp->proto == V1)){
		/* Retransmit oldest unacked I-frame */
		dup_p(&bp,axp->txq,0,len_mbuf(axp->txq));
		ctl = I | (((axp->vs - axp->unack) & MMASK) << 1) | (axp->vr << 5);
		sendframe(axp,COMMAND,ctl,bp);
	} else {
		ctl = len_mbuf(axp->rxq) >= axp->window ? RNR : RR;
		sendctl(axp,COMMAND,ctl);
	}
	axp->response = 0;
	stop_timer(&axp->t3);
	start_timer(&axp->t1);
}

#endif /* AX25 */
