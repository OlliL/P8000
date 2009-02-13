#include "global.h"
#include "timer.h"
#include "mbuf.h"
#include "netuser.h"
#include "internet.h"
#include "tcp.h"

/* Send a segment on the specified connection. One gets sent only
 * if there is data to be sent or if "force" is non zero
 */
void
tcp_output(tcb)
register struct tcb *tcb;
{
	struct pseudo_header ph;/* Pseudo-header for checksum calcs */
	struct mbuf *hbp,*dbp;	/* Header and data buffer pointers */
	int16 hsize;		/* Size of header */
	struct tcp seg;		/* Local working copy of header */
	int16 ssize;		/* Size of current segment being sent,
				 * including SYN and FIN flags */
	int16 dsize;		/* Size of segment less SYN and FIN */
	int16 usable;		/* Usable window */
	int16 sent;		/* Sequence count (incl SYN/FIN) already in the pipe */
	int16 backoff();	/* K5JB */
	/* note that unsigned int32 doesn't work here */
	unsigned long backoffval;
	extern int32 backoffcap;	/* set in tcpcmd.c */

	if(tcb == NULLTCB)
		return;

	switch(tcb->state){
	case LISTEN:
	case CLOSED:
		return;	/* Don't send anything */
	}
	for(;;){
		sent = tcb->snd.ptr - tcb->snd.una;

#ifdef	notdef
		/* If this is a retransmission, send only the oldest segment
		 * (first-only retransmission policy) -- OBSOLETED by cwind
		 */
		if((tcb->flags & RETRAN) && sent != 0)
			break;
#endif
		/* Don't send anything else until our SYN has been acked */
		if(sent != 0 && !(tcb->flags & SYNACK))
			break;

		if(tcb->snd.wnd == 0){
			/* Allow only one closed-window probe at a time */
			if(sent != 0)
				break;
			/* Force a closed-window probe */
			usable = 1;
		} else {
			/* usable window = offered window - unacked bytes in transit
			 * limited by the congestion window
			 */
			usable = min(tcb->snd.wnd,tcb->cwind) - sent;

			/* John Nagle's "single outstanding segment" rule.
			 * Allow only one segment in the pipeline unless there is enough
			 * unsent data to form at least one maximum-sized segment.
			 */
			if(sent != 0 && tcb->sndcnt - sent < tcb->mss){
				usable = 0;
			}
#ifdef	notdef
			/* Silly window avoidance. Don't send anything if the usable window
			 * is less than a quarter of the offered window.
			 * This test comes into play only when the offered window is at
			 * least 4 times the MSS; otherwise Nagle's test is sufficient
			 * to prevent SWS.
		 	 */
			else if(usable < tcb->snd.wnd/4){
				usable = 0;
			}
#endif
		}
		/* Compute size of segment to send. This is either the usable
		 * window, the mss, or the amount we have on hand, whichever is less.
		 * (I don't like optimistic windows)
		 */
		ssize = min(tcb->sndcnt - sent,usable);
		ssize = min(ssize,tcb->mss);
		dsize = ssize;

		if(ssize == 0 && !(tcb->flags & FORCE))
			break;		/* No need to send anything */

		tcb->flags &= ~FORCE;	/* Only one forced segment! */

		seg.source = tcb->conn.local.port;
		seg.dest = tcb->conn.remote.port;

		/* Set the SYN and ACK flags according to the state we're in. It is
		 * assumed that if this segment is associated with a state transition,
		 * then the state change will already have been made. This allows
		 * this routine to be called from a retransmission timeout with
		 * force=1.
		 * If SYN is being sent, adjust the dsize counter so we'll
		 * try to get the right amount of data off the send queue.
		 */
		seg.flags = ACK; /* Every state except SYN_SENT */
		hsize = TCPLEN;	/* Except when SYN being sent */
		seg.mss = 0;

		switch(tcb->state){
		case SYN_SENT:
			seg.flags = 0;	/* Note fall-thru */
		case SYN_RECEIVED:
			if(tcb->snd.ptr == tcb->iss){
				seg.flags |= SYN;
				dsize--;
				/* Also send MSS */
				seg.mss = tcp_mss;
				hsize = TCPLEN + MSS_LENGTH;
			}
			break;
		}
		seg.seq = tcb->snd.ptr;
		seg.ack = tcb->rcv.nxt;
		seg.wnd = tcb->rcv.wnd;
		seg.up = 0;

		/* Now try to extract some data from the send queue.
		 * Since SYN and FIN occupy sequence space and are reflected
		 * in sndcnt but don't actually sit in the send queue,
		 * dup_p will return one less than dsize if a FIN needs to be sent.
		 */
		if(dsize != 0){
			if(dup_p(&dbp,tcb->sndq,sent,dsize) != dsize){
				/* We ran past the end of the send queue; send a FIN */
				seg.flags |= FIN;
				dsize--;
			}
		} else
			dbp = NULLBUF;

		/* If the entire send queue will now be in the pipe, set the
		 * push flag
		 */
		if(dsize != 0 && sent + ssize == tcb->sndcnt)
			seg.flags |= PSH;

		/* If this transmission includes previously transmitted data,
		 * snd.nxt will already be past snd.ptr. In this case,
		 * compute the amount of retransmitted data and keep score
		 */
		if(tcb->snd.ptr < tcb->snd.nxt)
			tcb->resent += min(tcb->snd.nxt - tcb->snd.ptr,ssize);

		tcb->snd.ptr += ssize;
		/* If this is the first transmission of a range of sequence
		 * numbers, record it so we'll accept acknowledgments
		 * for it later
		 */
		if(seq_gt(tcb->snd.ptr,tcb->snd.nxt))
			tcb->snd.nxt = tcb->snd.ptr;

		/* Fill in fields of pseudo IP header */
		ph.source = tcb->conn.local.address;
		ph.dest = tcb->conn.remote.address;
		ph.protocol = TCP_PTCL;
		ph.length = hsize + dsize;

		/* Generate TCP header, compute checksum, and link in data */
		if((hbp = htontcp(&seg,dbp,&ph)) == NULLBUF){
			free_p(dbp);
			return;
		}
		/* If we're sending some data or flags, start retransmission
		 * and round trip timers if they aren't already running.
		 */
		if(ssize != 0){
			backoffval = backoff(tcb->backoff) *
			 (2 * tcb->mdev + tcb->srtt + (int32)MSPTICK) / (int32)MSPTICK;

			/* this must be unsigned comparison, because, well, shit happens! */
			tcb->timer.start = backoffval < backoffcap ? backoffval : backoffcap;
			start_timer(&tcb->timer);

			/* If round trip timer isn't running, start it */
			if(!run_timer(&tcb->rtt_timer)){
				start_timer(&tcb->rtt_timer);
				tcb->rttseq = tcb->snd.ptr;
			}
		}
		ip_send(tcb->conn.local.address,tcb->conn.remote.address,
		 TCP_PTCL,tcb->tos,0,hbp,ph.length,0,0);
	}
}
