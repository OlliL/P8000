/* telserv.c Actually a telnet client that communicates with a server.
 * This is a stub that permits adding a server as a telnet socket, based
 * on telunix that I use on my Unix machine.  Joe, K5JB
 * Initially use ts->fp for the output message queue ID and leave input
 * queue ID a global
 * Before starting telserv, the server must be running with input key of
 * 1234 and output key of 5678.  This client will use the reverse of those
 * keys to establish communication.
 * (See notes in src/msgs for message queue management utilities.)

 */
#include "unixopt.h"
#ifdef TELSERV	/* rest of module */
#include <stdio.h>
#include "global.h"
#include "mbuf.h"
#include "timer.h"
#include "icmp.h"
#include "netuser.h"
#include "tcp.h"
#include "telnet.h"
#include "session.h"
#include <errno.h>
#include <fcntl.h>	/* one uploaded to ucsd.edu 2/8/92 used sys/fcntl.h */
#include <signal.h>
#include <sys/types.h>
#include <termio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MSGLEN 256	/* can tune this later */
#define INKEY 5678	/* these compliment keys in the server */
#define OUTKEY 1234

extern int exitval;

#define TSMAXSCAN 1	/* initially we will do only process */
#define TSQSIZ	512	/* max data we will request from pty at once */

struct tcb *tserv_tcb = NULLTCB;
struct tcb *tservtcb[TSMAXSCAN];	/* savebuf for tcb ptrs for scan routines */
int inqid = -1;	/* global until I add another structure member fd */

struct inmsgbuf {
	long mtype;
	char linebuf[MSGLEN];
	} inmsgbuf;

struct outmsgbuf {
	long mtype;
	char linebuf[MSGLEN];
	} outmsgbuf;

static void
printerr()
{
	printf("Errno was: ");
	switch(errno){
		case EINVAL:
			printf("invalid\n");
			break;
		case EACCES:
			printf("access\n");
			break;
		case EPERM:
			printf("permission\n");
			break;
		default:
			printf("unknown errno\n");
	}
}

static int
openmsgs()
{

	int outqid;
	int i,rtn;
	struct msgbuf *msgp;

	if(inqid >= 0)	/* already opened */
		return -1;

	inqid = msgget(INKEY,0600);	/* rw by owner only */
	if(inqid == -1)
		printf("Oops, msgget 1 failed\n");
#ifdef DEBUG
	printf("Client Input Queue ID is %d\n",inqid);
#endif

	outqid = msgget(OUTKEY,0600);	/* rw by owner only */
	if(outqid == -1)
		printf("Oops, msgget 2 failed\n");
#ifdef DEBUG
	printf("Server Output Queue ID is %d\n",outqid);
#endif

	return (inqid >= 0) ? outqid : -1;
}

static void
closemsgs(ts)	/* won't actually close, just ignore */
struct telnet *ts;
{
	ts->fd = 0;
	inqid = -1;
}

static int
writemsg(outqid,buf,qty)
int outqid,qty;
struct outmsgbuf *buf;
{
	int rtn = qty;

	outmsgbuf.mtype = 1;	/* if 0, send fails with EINVAL */
	if(qty > MSGLEN)
		qty = MSGLEN;	/* come back later and make a loop here */
	memcpy(outmsgbuf.linebuf,buf,qty);
	if(msgsnd(outqid,&outmsgbuf,qty,IPC_NOWAIT) != 0){
		printf("Oops, msgsnd failed\n");
		return -1;
	}else
		return rtn == qty ? qty : rtn - MSGLEN;
}

/* Start telnet server */
int
tserv1(argc,argv)
char *argv[];
{
	struct socket lsocket;
	extern int32 ip_addr;
	void tserv_state();
	int fd;

	lsocket.address = ip_addr;

	if(argc < 2)
		lsocket.port = TTYLINK_PORT;	/* needs defining */
	else
		lsocket.port = atoi(argv[1]);

	tserv_tcb = open_tcp(&lsocket,NULLSOCK,TCP_SERVER,0,
			NULLVFP,NULLVFP,tserv_state,0,NULLCHAR);

	if(tserv_tcb == NULLTCB){
		fprintf(stderr,"start telserver fails rsn %d.\n",net_error);
		return(-1);
	}else
		log(tserv_tcb,"STARTED Telserver - (%d %x)", lsocket.port,tserv_tcb);
	return 0;
}

/* Shut down Telnet server */
int
tserv0()
{
	if(tserv_tcb != NULLTCB) {
		log(tserv_tcb,"STOPPED Telserver - (%x)",tserv_tcb);
		close_tcp(tserv_tcb);
		return(0);
	} else
		return -1;
}

/* Handle incoming Telnet-Unix connect requests */
static void
tserv_state(tcb,old,new)
struct tcb *tcb;
char old,new;
{
	register struct telnet *ts;
	int tserv_addscan();
	extern void tserv_rmvscan(),sndmsg();
	extern int send_tcp();
	extern struct mbuf *free_p();
	extern int del_tcp();
	extern int close_tcp();
	extern void free();
	char junk;
	char *wakeup = "hello\n";
	char *goodnight = "goodbye\n";
	struct msqid_ds statbuf;

	ts = (struct telnet *)tcb->user;

	switch(new){
	case ESTABLISHED:
		/* Create and initialize a Telnet protocol descriptor */
		if((ts = (struct telnet *)calloc(1,sizeof(struct telnet))) == NULLTN){
			log(tcb,"reject Telserver - no space");
			sndmsg(tcb,"Rejected; no space on remote\015\012");
			close_tcp(tcb);
			return;
		}
		ts->session = NULLSESSION;
		ts->state = TS_DATA;
		tcb->user = (char *)ts;	/* Upward pointer */
		ts->tcb = tcb;		/* Downward pointer */
		ts->inbuf = NULLBUF;
		ts->outbuf = NULLBUF;
		if(tserv_addscan(tcb) < 0 ||
			(ts->fd = openmsgs()) == -1) {	/* only one process at a time */
			tserv_rmvscan(tcb);
			log(tcb,"reject Telserver - no Unix ports");
			sndmsg(tcb,
				 "Rejected; no ports available on remote\015\012");
			close_tcp(tcb);
			return;
		}
			/* flush old server data */
		if(msgctl(inqid,IPC_STAT,&statbuf) == -1){
			log(tcb,"error Telserver - state: msgctl (%d %d %d)",
				errno, ts->fd, TSQSIZ - ts->outbuf->cnt);
			close_tcp(tcb);
			closemsgs(ts);
			break;
		}
		while(statbuf.msg_qnum--)
			msgrcv(inqid,&inmsgbuf,MSGLEN,0,IPC_NOWAIT);
		writemsg(ts->fd,wakeup,7);	/* replace with suitable wake up call */
		log(tcb,"open Telserver - (%d %x %d %d)",ts->fd,tcb,old,new);
		break;

	case FINWAIT1:
	case FINWAIT2:
	case CLOSING:
	case LAST_ACK:
	case TIME_WAIT:
		if(ts != NULLTN && ts->fd > 0) {
			log(tcb,"close Telserver - (%d %x %d %d)",
				ts->fd,tcb,old,new);
			writemsg(ts->fd,goodnight,9);	/* suitable message, note length */
			closemsgs(ts);
		}
		tserv_rmvscan(tcb);
		break;

	case CLOSE_WAIT:
		/* flush that last buffer */
		if(ts != NULLTN &&
		   ts->outbuf != NULLBUF &&
		   ts->outbuf->cnt != 0) {
			send_tcp(tcb,ts->outbuf);
			ts->outbuf = NULLBUF;
		}
		close_tcp(tcb);
		break;

	case CLOSED:
		if(ts != NULLTN){
			if(ts->fd > 0){
				log(tcb,"close Telserver - (%d %x %d %d)",ts->fd,tcb,old,new);
				closemsgs(ts);
			}
			if(ts->inbuf != NULLBUF)
				free_p(ts->inbuf);
			if(ts->outbuf != NULLBUF)
				free_p(ts->outbuf);
			free((char *)ts);
		}
		tserv_rmvscan(tcb);
		del_tcp(tcb);
		if(tcb == tserv_tcb)
			tserv_tcb = NULLTCB;
		break;
	}
}

/* Telserver io interface.  Called periodically to process any waiting io.  */
static void
tserv_try(tcb)
register struct tcb *tcb;
{
	extern void tserv_rmvscan();
	extern int recv_tcp();
	extern int send_tcp();
	extern void tserv_input();

	register struct telnet *ts;
	register int i;
	struct msqid_ds statbuf;

	if((ts = (struct telnet *)tcb->user) == NULLTN || ts->fd < 3) {
		/* Unknown connection - remove it from queue */
		log(tcb,"error Telnet - tserv_try (%d)", ts);
		tserv_rmvscan(tcb);
		return;
	}
	/*
	 * First, check if there is any pending input messages
	 */
	if(ts->inbuf != NULLBUF)
		tserv_input(ts);

	if(ts->inbuf == NULLBUF)
		if(tcb->rcvcnt > 0 && recv_tcp(tcb,&(ts->inbuf),0) > 0)
			tserv_input(ts);

	/*
	 * Next, check if there is any io for tcp:
	 */
	i = 0;
	do{
		if(ts->outbuf == NULLBUF &&
		   (ts->outbuf = alloc_mbuf(TSQSIZ)) == NULLBUF)
			break;		/* can't do much without a buffer */

		if(ts->outbuf->cnt < TSQSIZ) {
			if(msgctl(inqid,IPC_STAT,&statbuf) == -1){
				log(tcb,"error Telserver - msgctl (%d %d %d)",
					errno, ts->fd, TSQSIZ - ts->outbuf->cnt);
				close_tcp(tcb);
				break;
			}
			if(statbuf.msg_qnum){
				i = msgrcv(inqid,&inmsgbuf,MSGLEN,0,IPC_NOWAIT);
				memcpy(ts->outbuf->data + ts->outbuf->cnt,inmsgbuf.linebuf,
					i <= (TSQSIZ - (int)ts->outbuf->cnt) ? i :
					(TSQSIZ - (int)ts->outbuf->cnt));

			}
			if((ts->outbuf->cnt += i) < TSQSIZ)
				i = 0;	/* didn't fill buffer so don't retry */
		}
		if(ts->outbuf->cnt == 0 ||	/* nothing to send */
		   tcb->sndcnt > tcb->window)	/* too congested to send */
			break;
		if(send_tcp(tcb,ts->outbuf) < 0) {
			log(tcb,"error Telserver - send_tcp (%d %d %d)",
				net_error, ts->fd, ts->outbuf->cnt);
			close_tcp(tcb);
			ts->outbuf = NULLBUF;
			break;
		}
		ts->outbuf = NULLBUF;
	} while(i);
}

/* Process incoming TELNET characters */
void
tserv_input(ts)
register struct telnet *ts;
{
	void doopt(),dontopt(),willopt(),wontopt(),answer();
	char *memchr();
	register int i;
	register struct mbuf *bp;
	char c;

	bp = ts->inbuf;

	/* Optimization for very common special case -- no special chars */
	if(ts->state == TS_DATA){
		while(bp != NULLBUF &&
		      memchr(bp->data,IAC,(int)bp->cnt) == NULLCHAR) {
			if((i = writemsg(ts->fd, bp->data, (unsigned)bp->cnt)) == bp->cnt) {
				ts->inbuf = bp = free_mbuf(bp);
			} else if(i == -1) {
				log(ts->tcb,"error Telserver - write (%d %d %d)",
					errno, ts->fd, bp->cnt);
				close_tcp(ts->tcb);
				closemsgs(ts);
				return;
			} else {
				bp->cnt -= i;
				bp->data += i;
				return;
			}
		}
		if(bp == NULLBUF)
			return;
	}
	while(pullup(&(ts->inbuf),&c,1) == 1){
		bp = ts->inbuf;
		switch(ts->state){
		case TS_DATA:
			if(uchar(c) == IAC){
				ts->state = TS_IAC;
			} else {
				if(!ts->remote[TN_TRANSMIT_BINARY])
					c &= 0x7f;
				if(writemsg(ts->fd, &c, 1) != 1) {
					/* we drop a character here */
					return;
				}
			}
			break;
		case TS_IAC:
			switch(uchar(c)){
			case WILL:
				ts->state = TS_WILL;
				break;
			case WONT:
				ts->state = TS_WONT;
				break;
			case DO:
				ts->state = TS_DO;
				break;
			case DONT:
				ts->state = TS_DONT;
				break;
			case IAC:
				if(writemsg(ts->fd, &c, 1) != 1) {
					/* we drop a character here */
					return;
				}
				ts->state = TS_DATA;
				break;
			default:
				ts->state = TS_DATA;
				break;
			}
			break;
		case TS_WILL:
			willopt(ts,c);
			ts->state = TS_DATA;
			break;
		case TS_WONT:
			wontopt(ts,c);
			ts->state = TS_DATA;
			break;
		case TS_DO:
			doopt(ts,c);
			ts->state = TS_DATA;
			break;
		case TS_DONT:
			dontopt(ts,c);
			ts->state = TS_DATA;
			break;
		}
	}
}

/* called periodically from main loop */
void
tserv_scan()
{
	void tserv_try();
	register int i;

	for(i = 0; i < TSMAXSCAN; i += 1)
		if(tservtcb[i] != NULLTCB)
			tserv_try(tservtcb[i]);
}

int
tserv_addscan(tcb)
struct tcb *tcb;
{
	register int i;
	for(i = 0; i < TSMAXSCAN; i += 1)
		if(tservtcb[i] == NULLTCB) {
			tservtcb[i] = tcb;
			return i;
		}
	return -1;
}

void
tserv_rmvscan(tcb)
struct tcb *tcb;
{
	register int i;

	for(i = 0; i < TSMAXSCAN; i += 1)
		if(tservtcb[i] == tcb)
			tservtcb[i] = NULLTCB;
}
#endif /* TELSERV */
