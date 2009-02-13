/* Link Access Procedures Balanced (LAPB) - with changes for rational
 * behavior over packet radio
 * Added segmentation and deferred frame delivery to correct LAPB errors
 * from queued transmit frames already gone to the TNC. k33 - K5JB
 * Added IP only VC call sign k34 - K5JB
 */
#include "options.h"
#include "config.h"
#ifdef AX25
#include "global.h"
#include "mbuf.h"
#include "timer.h"
#include "ax25.h"
#include "lapb.h"
#include "iface.h"
#ifdef MSDOS
#include <mem.h>
#endif

void free_q(),frmr();
void procdata(),clr_ex(),est_link(),enq_resp(),inv_rex(),lapb_output();
int sendframe(),start_timer(),stop_timer();	/* latter 2 should have been void */
int sendctl(),ackours();

#ifdef VCIP_SSID
extern struct ax25_addr ip_call;
int addreq();
#endif

#ifdef SID2
extern int bbscall_set;
#endif

#if defined(SEGMENT) && defined(SEG_CMD)
int rx_segment = 1;	/* also used in ax25cmd.c */
#endif

/* Process incoming frames */
int
lapb_input(axp,cmdrsp,bp)
struct ax25_cb *axp;		/* Link control structure */
char cmdrsp;			/* Command/response flag */
struct mbuf *bp;		/* Rest of frame, starting with ctl */
{
	int16 ftype();
	void lapbstate(),del_ax25();
	char control;
	char class;		/* General class (I/S/U) of frame */
	int16 type;		/* Specific type (I/RR/RNR/etc) of frame */
	char poll = 0;
	char final = 0;
	int nr;			/* ACK number of incoming frame */
	int ns;			/* Seq number of incoming frame */
	char tmp;
	int recovery;

	if(bp == NULLBUF || axp == NULLAX25){
		free_p(bp);
		return -1;
	}

	/* Extract the various parts of the control field for easy use */
	control = pullchar(&bp);
	type = ftype(control);
	class = type & 0x3;
	/* Check for polls and finals */
	if(control & PF){
		switch(cmdrsp){
		case COMMAND:
			poll = YES;
			break;
		case RESPONSE:
			final = YES;
			break;
		}
	}
	/* Extract sequence numbers, if present */
	switch(class){
	case I:
	case I+2:
		ns = (control >> 1) & MMASK;
	case S:	/* Note fall-thru */
		nr = (control >> 5) & MMASK;
		break;
	}
	/* This section follows the SDL diagrams by K3NA fairly closely */
	recovery = 0;
	switch(axp->state){
	case DISCONNECTED:	/* need to revisit this... */
		switch(type){
		case SABM:	/* Initialize or reset link */
			sendctl(axp,RESPONSE,UA);	/* Always accept */
			clr_ex(axp);
			axp->unack = axp->vr = axp->vs = 0;
			lapbstate(axp,CONNECTED);/* Resets state counters */
			start_timer(&axp->t3);
			start_timer(&axp->t4);
			break;
/* note that NOS has a case DISC here but it has same effect as default */
		case DM:	/* Ignore to avoid infinite loops */
			break;
		default:	/* All others get DM */
/* This is not according to ax.25 v1, but looks OK to me
			if(poll)
*/
				sendctl(axp,RESPONSE,DM);
			break;
		}
		break;
/* note that NOS has a test for state == DISCONNECTED here but we avoid
 * spaghetti code by doing it later */
	case SETUP:
		switch(type){
		case SABM:	/* Simultaneous open */
			sendctl(axp,RESPONSE,UA);
			break;
		case DISC:
			sendctl(axp,RESPONSE,DM);
			break;
		case UA:	/* Connection accepted */
			/* Note: xmit queue not cleared */
			stop_timer(&axp->t1);
			start_timer(&axp->t3);
			axp->unack = axp->vr = axp->vs = 0;
			lapbstate(axp,CONNECTED);
			start_timer(&axp->t4);
			break;
		case DM:	/* Connection refused */
			free_q(&axp->txq);
			stop_timer(&axp->t1);
			lapbstate(axp,DISCONNECTED);
			break;
		default:	/* All other frames ignored */
			break;
		}
		break;
	case DISCPENDING:
		switch(type){
		case SABM:
			sendctl(axp,RESPONSE,DM);
			break;
		case DISC:
			sendctl(axp,RESPONSE,UA);
			break;
		case UA:
		case DM:
			stop_timer(&axp->t1);
			lapbstate(axp,DISCONNECTED);
			break;
		default:	/* Respond with DM only to command polls */
			if(poll)
				sendctl(axp,RESPONSE,DM);
			break;
		}
		break;

	case RECOVERY:	/* revised following saved 800 bytes - K5JB */
		recovery = 1;	/* fall through */
	case CONNECTED:
		switch(type){
		case SABM:
			sendctl(axp,RESPONSE,UA);
			clr_ex(axp);
			if(!recovery)
				free_q(&axp->txq); /* remarked out in GRI NOS */
			stop_timer(&axp->t1);
			start_timer(&axp->t3);
			axp->unack = axp->vr = axp->vs = 0;
			lapbstate(axp,CONNECTED); /* remarked out in NOS *//* Purge queues */
			if(recovery && !run_timer(&axp->t4))
					start_timer(&axp->t4);
			break;
		case DISC:
			free_q(&axp->txq);
			sendctl(axp,RESPONSE,UA);
			stop_timer(&axp->t1);
			stop_timer(&axp->t3);
			lapbstate(axp,DISCONNECTED);
			break;
		case DM:
			lapbstate(axp,DISCONNECTED);
			break;
		case UA:
		case FRMR:
			est_link(axp);
			lapbstate(axp,SETUP);	/* Re-establish link */
			break;
		case REJ:
		case RNR:
		case RR:
			axp->remotebusy = (type == RNR) ? YES : NO;
			if(recovery){
				if(axp->proto == V1 || final){
					stop_timer(&axp->t1);
					ackours(axp,nr);
					if(axp->unack != 0)
						inv_rex(axp);
					else{
						start_timer(&axp->t3);
						if(type != RNR)	/* k37 */
							lapbstate(axp,CONNECTED);
						if(!run_timer(&axp->t4))
							start_timer(&axp->t4);
					}
				}else{
					if(poll)
						enq_resp(axp);
					ackours(axp,nr);
					if(type == REJ && axp->unack != 0)
						inv_rex(axp);	/* This is certain to trigger output */
					if(!run_timer(&axp->t1))
						start_timer(&axp->t1);
				}
			}else{ /* not recovery */
				if(poll)
					enq_resp(axp);
				ackours(axp,nr);
				if(type == REJ){
					stop_timer(&axp->t1);
					start_timer(&axp->t3);
				/* This may or may not actually invoke transmission,
				 * depending on whether this REJ was caused by
				 * our losing his prior ACK.
				 */
					inv_rex(axp);
				 }
			}
			break;
		case I:
			ackours(axp,nr);
			if(recovery){
				if(!run_timer(&axp->t1))
				/* Make sure timer is running, since an I frame
				 * cannot satisfy a poll
				 */
					start_timer(&axp->t1);
			}else
				start_timer(&axp->t4);
			if(len_mbuf(axp->rxq) >= axp->window){
				/* Too bad he didn't listen to us; he'll
				 * have to resend the frame later. This
				 * drastic action is necessary to avoid
				 * deadlock.
				 */
				if(recovery || poll)
					sendctl(axp,RESPONSE,RNR);
				free_p(bp);
				bp = NULLBUF;
				break;
			}
			/* Reject or ignore I-frames with receive sequence number errors */
			if(ns != axp->vr){
				if(axp->proto == V1 || !axp->rejsent){
					axp->rejsent = YES;
					sendctl(axp,RESPONSE,REJ);
				}
				else
				axp->response = 0;
				break;
			}
			axp->rejsent = NO;
			axp->vr = (axp->vr+1) & MMASK;
			tmp = len_mbuf(axp->rxq) >= axp->window ? RNR : RR;
			/* in this area, NOS sends unnecessary RR(p) frames */
			axp->response = tmp;	/* will take care of after lapb_output */
			procdata(axp,bp);
			bp = NULLBUF;
			break;
		default:	/* All others ignored */
			break;
		}
		break;
	case FRAMEREJECT:
		switch(type){
		case SABM:
			sendctl(axp,RESPONSE,UA);
			clr_ex(axp);
			axp->unack = axp->vr = axp->vs = 0;
			stop_timer(&axp->t1);
			start_timer(&axp->t3);
			lapbstate(axp,CONNECTED);
			break;
		case DISC:
			free_q(&axp->txq);
			sendctl(axp,RESPONSE,UA);
			stop_timer(&axp->t1);
			lapbstate(axp,DISCONNECTED);
			break;
		case DM:
			stop_timer(&axp->t1);
			lapbstate(axp,DISCONNECTED);
			break;
		default:
			frmr(axp,0,0);
			break;
		}
		break;
	}
	free_p(bp);	/* In case anything's left */
	/* Empty the trash */
	if(axp->state == DISCONNECTED){
		del_ax25(axp);
		return 0;
	}
	/* See if we can send some data, perhaps piggybacking an ack.
	 * If successful, lapb_output will clear axp->response.
	 */
	lapb_output(axp);
	return 0;
}
/* Handle incoming acknowledgements for frames we've sent.
 * Free frames being acknowledged.
 * Return -1 to cause a frame reject if number is bad, 0 otherwise
 */
static int
ackours(axp,n)
struct ax25_cb *axp;
char n;
{
	struct mbuf *bp;
	int acked = 0;	/* Count of frames acked by this ACK */
	int oldest;	/* Seq number of oldest unacked I-frame */

	/* Free up acknowledged frames by purging frames from the I-frame
	 * transmit queue. Start at the remote end's last reported V(r)
	 * and keep going until we reach the new sequence number.
	 * If we try to free a null pointer, then we have a frame reject condition.
	 */
	oldest = (axp->vs - axp->unack) & MMASK;
	while(axp->unack != 0 && oldest != n){
		if((bp = dequeue(&axp->txq)) == NULLBUF){
			/* Acking unsent frame */
			return -1;
		}
		free_p(bp);
		axp->unack--;
		acked++;
#ifdef notdef /* don't think we need this */
		if(!axp->remotebusy)	/* k37 */
#endif
		axp->retries = 0;
		oldest = (oldest + 1) & MMASK;
	}
	if(axp->unack == 0){
		/* All frames acked, stop timeout */
		stop_timer(&axp->t1);
		start_timer(&axp->t3);
	} else if(acked != 0) { 
		/* Partial ACK; restart timer */
		start_timer(&axp->t1);
	}
	/* If user has set a transmit upcall, indicate how many frames
	 * may be queued
	 */
	if(acked != 0 && axp->t_upcall != NULLVFP)
		(*axp->t_upcall)(axp,axp->paclen * (axp->maxframe - axp->unack));

	return 0;
}

/* Establish data link */
void
est_link(axp)
struct ax25_cb *axp;
{
	clr_ex(axp);
	axp->retries = 0;
	sendctl(axp,COMMAND,SABM);
	stop_timer(&axp->t3);
	start_timer(&axp->t1);
}
/* Clear exception conditions */
void
clr_ex(axp)
struct ax25_cb *axp;
{
	axp->remotebusy = NO;
	axp->rejsent = NO;
	axp->response = 0;
	stop_timer(&axp->t3);
}
/* Enquiry response */
void
enq_resp(axp)
struct ax25_cb *axp;
{
	char ctl;

	ctl = len_mbuf(axp->rxq) >= axp->window ? RNR : RR;
	sendctl(axp,RESPONSE,ctl);
	axp->response = 0;
	stop_timer(&axp->t3);
}
/* Invoke retransmission */
void
inv_rex(axp)
struct ax25_cb *axp;
{
	axp->vs -= axp->unack;
	axp->vs &= MMASK;
	axp->unack = 0;
}
/* Generate Frame Reject (FRMR) response
 * If reason != 0, this is the initial error frame
 * If reason == 0, resend the last error frame
 */
void
frmr(axp,control,reason)
register struct ax25_cb *axp;
char control;
char reason;
{
	struct mbuf *frmrinfo;
	register char *cp;
	void lapbstate();

	if(reason != 0){
		cp = axp->frmrinfo;
		*cp++ = control;
		*cp++ =  axp->vr << 5 || axp->vs << 1;
		*cp = reason;
	}
	if((frmrinfo = alloc_mbuf(3)) == NULLBUF)
		return;	/* No memory */
	frmrinfo->cnt = 3;
	memcpy(frmrinfo->data,axp->frmrinfo,3);
	sendframe(axp,RESPONSE,FRMR,frmrinfo);
}

/* Send S or U frame to currently connected station */
int
sendctl(axp,cmdrsp,cmd)
struct ax25_cb *axp;
char cmdrsp,cmd;
{
	int16 ftype();

	if((ftype(cmd) & 0x3) == S)	/* Insert V(R) if S frame */
		cmd |= (axp->vr << 5);
	return sendframe(axp,cmdrsp,cmd,NULLBUF);
}

/* defer output with timer, give time for ack to abort retry */

void
lapb_output(axp)
register struct ax25_cb *axp;
{
	/* function installed in lapbtime.c */
	axp->t2.arg = (char *)axp;
	start_timer(&axp->t2);
}

/* Set new link state.
 * If the new state is disconnected, also free the link control block.
 */
void
lapbstate(axp,s)
struct ax25_cb *axp;
int s;
{
	int oldstate;

	oldstate = axp->state;
	axp->state = s;
	if(s == DISCONNECTED){
		stop_timer(&axp->t1);
		stop_timer(&axp->t2);
		stop_timer(&axp->t3);
		stop_timer(&axp->t4);
		free_q(&axp->txq);
	}
	/* Don't bother the client unless the state is really changing */
	if(oldstate != s && axp->s_upcall != NULLVFP)
		(*axp->s_upcall)(axp,oldstate,s);
}
/* Process a valid incoming I frame */
static void
procdata(axp,bp)
struct ax25_cb *axp;
struct mbuf *bp;
{
	unsigned char pid;
#ifdef SEGMENT
	int seq;
#endif
	int ip_route();
#ifdef NETROM
	void nr_route();
#endif
	/* Extract level 3 PID, and THEN see if there is any info - k35 */
	if(pullup(&bp,&pid,1) != 1 || bp == NULLBUF)
		return;	/* No PID, or was a zero length ROSE Booger */

#ifdef SEGMENT	/* NOS's "segmentation" scheme -- uses its own PID */
#ifdef SEG_CMD
	if(pid == PID_SEGMENT && rx_segment)	/* if turned off we will trash it */
#else
	if(pid == PID_SEGMENT)
#endif
	{
		seq = pullchar(&bp);
		if(axp->segremain == 0){	/* Probably first segment of a set */
			/* Start reassembly */
			if(!(seq & SEG_FIRST)){
				free_p(bp);     /* How in the hell did we get this from ax25? */
				return;
			}
			/* Put first segment on list */
			axp->segremain = seq & SEG_REM;
			axp->rxasm = bp;
			return; /* will handle it later */
		}else{
			/* Reassembly in progress; continue */
			if((seq & SEG_REM) != axp->segremain - 1){
				/* Error! How in hell did it get out of order? */
				free_p(axp->rxasm);
				axp->rxasm = NULLBUF;
				axp->segremain = 0;
				free_p(bp);
				return;
			}

			/* Correct, in-order segment */
			append(&axp->rxasm,bp);
			if((axp->segremain = (seq & SEG_REM)) != 0)
				return; /* will handle it later */

			/* Else, done; process it */
			bp = axp->rxasm;
			axp->rxasm = NULLBUF;
			pid = pullchar(&bp); /* tumble to switch below */
		}	/* else */
	}	/* if SEGMENT */
#endif /* SEGMENT */

	/* Finally (maybe), we do something with this frame */

	switch(pid){
	case PID_IP:		/* DoD Internet Protocol */
		ip_route(bp,0);
		break;
	case PID_NO_L3:
#ifdef VCIP_SSID
		/* don't think it is now necessary to test for bp->cnt - k35 */
#ifdef notdef
		if(addreq(&axp->addr.source,&ip_call) || bp->cnt == 0){
			free_p(bp);	/* don't send zero length packets upstairs */
			break;
		}
#else
		/* zero length I frames already trashed - k35 */
		if(addreq(&axp->addr.source,&ip_call)){
			free_p(bp);
			break;
		}
#endif
#endif
#ifdef SID2
		/* new style of eliminating spurious I frames k36 */
		if(bbscall_set && addreq(&axp->addr.source,&mycall) && !axp->user){
			free_p(bp);
			break;
		}
#ifdef REFERENCE
		/* See if a session already exists */
		for(s = sessions; s < &sessions[nsessions]; s++){
			if(s->type == AX25TNC
				&& addreq(&s->cb.ax25_cb->addr.dest,&dest)){
				printf("Session %u to %s already exists\n",s - sessions,argv[2]);
				return 1;
			}
		}
#endif
#endif
		append(&axp->rxq,bp); /* Enqueue for application - usually mailbox */
		if(axp->r_upcall != NULLVFP)
			(*axp->r_upcall)(axp,len_mbuf(axp->rxq));
		break;
#ifdef NETROM
	case PID_NETROM:
		nr_route(bp,axp);
		break;
#endif
	default:		/* Note: ARP is invalid here */
		free_p(bp);
		break;
	}
}
#endif /* AX25 */
