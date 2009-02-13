/* This version of telunix.c is the one that should be used with Unix
 * systems that have normal ptys on them.  My AT&T 3B2 has brain-damaged
 * pty driver so I had to booger telunix.c to get it to work.  Often I
 * distribute the NET package with the wrong telunix.c in it.  If this
 * is the case, replace the telunix.c in the package with this one.
 *
 * This version of telunix.c will work in Coherent.  Set up ttyp0, ttyp1
 * and ttyp2 in /etc/ttys (I used 1lPttyp0, for example), do a kill quit 1,
 * and use the start telunix command.
 * Users can be provided with a .profile that does stty -echo.  They will
 * have to use "eol unix" or they will get double prompts.  If you start
 * telunix with the default port (513), a user would do telnet yourhostname 513
 * to start the session.  If you don't use telnet, you can instead start
 * telunix 23 (telnet port) so any incoming telnet session gets the logon.
 * Thanks to VE5BC who heckled me into fixing this. - K5JB
 */
#include "unixopt.h"
#ifdef TELUNIX	/* rest of module */
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

#define TUMAXSCAN 3	/* max number of telunix clients active */
#define TURQSIZ	512	/* max data we will request from pty at once */

struct tcb *tnix_tcb = NULLTCB;
struct tcb *tnixtcb[TUMAXSCAN];	/* savebuf for tcb ptrs for scan routines */

int		/* moved here from sys5_io.c */
OpenPty()
{
	int		pty;
	int		letcnt=0, numcnt=0;

/* we will sort these out after I find out what is common - K5JB */

#ifdef COH386
	static char	*letters = "p", *numbers = "0123456789";
	static char master[] = "/dev/ptyXX";
#else
	static char	*letters = "pqrs", *numbers = "0123456789abcdef";
#ifdef hpux
	static char master[] = "/dev/ptym/ptyXX";
#else
	static char master[] = "/dev/ptyXX";
#endif
#endif
	static int	letmax, nummax;

	letmax=strlen(letters)-1, nummax=strlen(numbers)-1;
	do {
#ifdef hpux
		master[strlen("/dev/ptym/pty")] = letters[letcnt];
		master[strlen("/dev/ptym/ptyX")] = numbers[numcnt];
#else
		master[strlen("/dev/pty")] = letters[letcnt];
		master[strlen("/dev/ptyX")] = numbers[numcnt];
#endif
		if (letcnt > letmax) {
			return -1;
		} else if (++numcnt > nummax) {
			letcnt++;
			numcnt = 0;
		} 
	} while ((pty=open(master, O_RDWR | O_NDELAY)) < 0);
	return(pty);
}

/* Start telnet-unix server */
int
tnix1(argc,argv)
char *argv[];
{
	struct socket lsocket;
	extern int32 ip_addr;
	void tnix_state();
	int fd;

	/* test for presence of ptys */

	if((fd = OpenPty()) == -1)
		return -1;
	close(fd);

	lsocket.address = ip_addr;

	if(argc < 2)
		lsocket.port = LOGIN_PORT;
	else
		lsocket.port = atoi(argv[1]);

	tnix_tcb = open_tcp(&lsocket,NULLSOCK,TCP_SERVER,0,
			NULLVFP,NULLVFP,tnix_state,0,NULLCHAR);

	if(tnix_tcb == NULLTCB){
		fprintf(stderr,"start telunix fails rsn %d.\n",net_error);
		return(-1);
	}else
		log(tnix_tcb,"STARTED Telunix - (%d %x)", lsocket.port,tnix_tcb);
	return 0;
}

/* Shut down Telnet server */
int
tnix0()
{
	if(tnix_tcb != NULLTCB) {
		log(tnix_tcb,"STOPPED Telunix - (%x)",tnix_tcb);
		close_tcp(tnix_tcb);
		return(0);
	} else
		return -1;
}

/* Handle incoming Telnet-Unix connect requests */
static void
tnix_state(tcb,old,new)
struct tcb *tcb;
char old,new;
{
	register struct telnet *tn;
	int tnix_addscan();
	extern void tnix_rmvscan(),sndmsg();
	extern int send_tcp();
	extern struct mbuf *free_p();
	extern int del_tcp();
	extern int close_tcp();
	extern void free();

	tn = (struct telnet *)tcb->user;

	switch(new){
	case ESTABLISHED:
		/* Create and initialize a Telnet protocol descriptor */
		if((tn = (struct telnet *)calloc(1,sizeof(struct telnet))) == NULLTN){
			log(tcb,"reject Telunix - no space");
			sndmsg(tcb,"Rejected; no space on remote\015\012");
			close_tcp(tcb);
			return;
		}
		tn->session = NULLSESSION;
		tn->state = TS_DATA;
		tcb->user = (char *)tn;	/* Upward pointer */
		tn->tcb = tcb;		/* Downward pointer */
		tn->inbuf = NULLBUF;
		tn->outbuf = NULLBUF;
		if(tnix_addscan(tcb) < 0 ||
		   (tn->fd = OpenPty()) < 3) {	/* barf if <= stderr */
			tnix_rmvscan(tcb);
			log(tcb,"reject Telunix - no Unix ports");
			sndmsg(tcb,
			    "Rejected; no ports available on remote\015\012");
			close_tcp(tcb);
			return;
		}
		log(tcb,"open Telunix - (%d %x %d %d)",tn->fd,tcb,old,new);
		break;

	case FINWAIT1:
	case FINWAIT2:
	case CLOSING:
	case LAST_ACK:
	case TIME_WAIT:
		if(tn != NULLTN &&
		   tn->fd > 2) {
			log(tcb,"close Telunix - (%d %x %d %d)",
				tn->fd,tcb,old,new);
			close(tn->fd);
			tn->fd = 0;
		}
		tnix_rmvscan(tcb);
		break;

	case CLOSE_WAIT:
		/* flush that last buffer */
		if(tn != NULLTN &&
		   tn->outbuf != NULLBUF &&
		   tn->outbuf->cnt != 0) {
			send_tcp(tcb,tn->outbuf);
			tn->outbuf = NULLBUF;
		}
		close_tcp(tcb);
		break;
	
	case CLOSED:
		if(tn != NULLTN) {
			if(tn->fd > 2) {
				log(tcb,"close Telunix - (%d %x %d %d)",
					tn->fd,tcb,old,new);
				close(tn->fd);
				tn->fd = 0;
			}
			if(tn->inbuf != NULLBUF)
				free_p(tn->inbuf);
			if(tn->outbuf != NULLBUF)
				free_p(tn->outbuf);
			free((char *)tn);
		}
		tnix_rmvscan(tcb);
		del_tcp(tcb);
		if(tcb == tnix_tcb)
			tnix_tcb = NULLTCB;
		break;
	}
}

/* Telunix io interface.  Called periodically to process any waiting io.  */
void
tnix_try(tcb)
register struct tcb *tcb;
{
	extern void tnix_rmvscan();
	extern int recv_tcp();
	extern int send_tcp();
	extern void tnix_input();

	register struct telnet *tn;
	register int i;

	if((tn = (struct telnet *)tcb->user) == NULLTN || tn->fd < 3) {
		/* Unknown connection - remove it from queue */
		log(tcb,"error Telnet - tnix_try (%d)", tn);
		tnix_rmvscan(tcb);
		return;
	}
	/*
	 * First, check if there is any pending io for the pty:
	 */
	if(tn->inbuf != NULLBUF) 
		tnix_input(tn);

	if(tn->inbuf == NULLBUF)
		if(tcb->rcvcnt > 0 && recv_tcp(tcb,&(tn->inbuf),0) > 0)
			tnix_input(tn);

	/*
	 * Next, check if there is any io for tcp:
	 */
	i = 0;
	do{
		if(tn->outbuf == NULLBUF &&
		   (tn->outbuf = alloc_mbuf(TURQSIZ)) == NULLBUF)
			break;		/* can't do much without a buffer */
	
		if(tn->outbuf->cnt < TURQSIZ) {
			if((i = read(tn->fd, tn->outbuf->data + tn->outbuf->cnt,
				TURQSIZ - (int)tn->outbuf->cnt)) == -1) {
#ifdef COH386
				/* Coherent read() returns -1 on no data available */
				i = 0;
#endif
				if(errno != EAGAIN){
					log(tcb,"error Telunix - read (%d %d %d)",
						errno, tn->fd, TURQSIZ - tn->outbuf->cnt);
					close_tcp(tcb);
					break;
				}
			}
			if((tn->outbuf->cnt += i) < TURQSIZ)
				i = 0;	/* didn't fill buffer so don't retry */
		}
		if(tn->outbuf->cnt == 0 ||	/* nothing to send */
		   tcb->sndcnt > tcb->window)	/* too congested to send */
			break;
		if(send_tcp(tcb,tn->outbuf) < 0) {
			log(tcb,"error Telunix - send_tcp (%d %d %d)",
				net_error, tn->fd, tn->outbuf->cnt);
			close_tcp(tcb);
			tn->outbuf = NULLBUF;
			break;
		}
		tn->outbuf = NULLBUF;
	} while(i);
}

/* Process incoming TELNET characters */
void
tnix_input(tn)
register struct telnet *tn;
{
	void doopt(),dontopt(),willopt(),wontopt(),answer();
	char *memchr();
	register int i;
	register struct mbuf *bp;
	char c;

	bp = tn->inbuf;

	/* Optimization for very common special case -- no special chars */
	if(tn->state == TS_DATA){
		while(bp != NULLBUF &&
		      memchr(bp->data,IAC,(int)bp->cnt) == NULLCHAR) {
			if((i = write(tn->fd, bp->data, (int)bp->cnt)) == bp->cnt) {
				tn->inbuf = bp = free_mbuf(bp);
			} else if(i == -1) {
				log(tn->tcb,"error Telunix - write (%d %d %d)",
					errno, tn->fd, bp->cnt);
				close_tcp(tn->tcb);
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
	while(pullup(&(tn->inbuf),&c,1) == 1){
		bp = tn->inbuf;
		switch(tn->state){
		case TS_DATA:
			if(uchar(c) == IAC){
				tn->state = TS_IAC;
			} else {
				if(!tn->remote[TN_TRANSMIT_BINARY])
					c &= 0x7f;
				if(write(tn->fd, &c, 1) != 1) {
					/* we drop a character here */
					return;
				}
			}
			break;
		case TS_IAC:
			switch(uchar(c)){
			case WILL:
				tn->state = TS_WILL;
				break;
			case WONT:
				tn->state = TS_WONT;
				break;
			case DO:
				tn->state = TS_DO;
				break;
			case DONT:
				tn->state = TS_DONT;
				break;
			case IAC:
				if(write(tn->fd, &c, 1) != 1) {
					/* we drop a character here */
					return;
				}
				tn->state = TS_DATA;
				break;
			default:
				tn->state = TS_DATA;
				break;
			}
			break;
		case TS_WILL:
			willopt(tn,c);
			tn->state = TS_DATA;
			break;
		case TS_WONT:
			wontopt(tn,c);
			tn->state = TS_DATA;
			break;
		case TS_DO:
			doopt(tn,c);
			tn->state = TS_DATA;
			break;
		case TS_DONT:
			dontopt(tn,c);
			tn->state = TS_DATA;
			break;
		}
	}
}

/* called periodically from main loop */
void
tnix_scan()
{
	void tnix_try();
	register int i;

	for(i = 0; i < TUMAXSCAN; i += 1)
		if(tnixtcb[i] != NULLTCB)
			tnix_try(tnixtcb[i]);
}

int
tnix_addscan(tcb)
struct tcb *tcb;
{
	register int i;
	for(i = 0; i < TUMAXSCAN; i += 1)
		if(tnixtcb[i] == NULLTCB) {
			tnixtcb[i] = tcb;
			return i;
		}
	return -1;
}

void
tnix_rmvscan(tcb)
struct tcb *tcb;
{
	register int i;

	for(i = 0; i < TUMAXSCAN; i += 1)
		if(tnixtcb[i] == tcb)
			tnixtcb[i] = NULLTCB;
}
#endif /* TELUNIX */
