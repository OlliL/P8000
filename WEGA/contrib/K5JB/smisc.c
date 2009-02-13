/* Miscellaneous servers - revised logging for sys reset and exit - K5JB */
#include <stdio.h>
#include <time.h>
#include "sokname.h"
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "timer.h"
#include "tcp.h"
#include "remote.h"

static struct tcb *disc_tcb,*echo_tcb;
static struct socket remsock;
void log(),genlog();
int atoi();

/* Start up discard server */
dis1(argc,argv)
int argc;
char *argv[];
{
	struct socket lsocket;
	void disc_recv(),misc_state();

	lsocket.address = ip_addr;
	if(argc < 2)
		lsocket.port = DISCARD_PORT;
	else
		lsocket.port = (int16)atoi(argv[1]);
	disc_tcb = open_tcp(&lsocket,NULLSOCK,TCP_SERVER,0,disc_recv,NULLVFP,
		misc_state,0,NULLCHAR);	/* K5JB */
}
/* Start echo server */
echo1(argc,argv)
int argc;
char *argv[];
{
	void echo_recv(),echo_trans(),misc_state();
	struct socket lsocket;

	lsocket.address = ip_addr;
	if(argc < 2)
		lsocket.port = ECHO_PORT;
	else
		lsocket.port = (int16)atoi(argv[1]);
	echo_tcb = open_tcp(&lsocket,NULLSOCK,TCP_SERVER,0,echo_recv,echo_trans,
		misc_state,0,NULLCHAR);	/* K5JB */

}

/* Start remote exit/reboot server */
rem1(argc,argv)
int argc;
char *argv[];
{
	void uremote();
	int open_udp();

	remsock.address = ip_addr;
	if(argc < 2)
		remsock.port = REMOTE_PORT;
	else
		remsock.port = (int16)atoi(argv[1]);
	open_udp(&remsock,uremote);
}

/* Shut down miscellaneous servers */
dis0()
{
	if(disc_tcb != NULLTCB)
		close_tcp(disc_tcb);
}
echo0()
{
	if(echo_tcb != NULLTCB)
		close_tcp(echo_tcb);
}
rem0()
{
	int del_udp();
	del_udp(&remsock);
}
/* Discard server receiver upcall */
static
void
disc_recv(tcb,cnt)
struct tcb *tcb;
int16 cnt;
{
	struct mbuf *bp;

	if(recv_tcp(tcb,&bp,cnt) > 0)
		free_p(bp);			/* Discard */
}

/* Echo server receive
 * Copies only as much will fit on the transmit queue
 */
static
void
echo_recv(tcb,cnt)
struct tcb *tcb;
int cnt;
{
	struct mbuf *bp;
	int acnt;

	if(cnt == 0){
		close_tcp(tcb);
		return;
	}
	acnt = min(cnt,tcb->snd.wnd);
	if(acnt > 0){
		/* Get only as much will fit in the send window */
		recv_tcp(tcb,&bp,tcb->snd.wnd);
		send_tcp(tcb,bp);
	}
}
/* Echo server transmit
 * Copies anything that might have been left in the receiver queue
 */
static
void
echo_trans(tcb,cnt)
struct tcb *tcb;
int16 cnt;
{
	struct mbuf *bp;

	if(tcb->rcvcnt > 0){
		/* Get only as much will fit in the send window */
		recv_tcp(tcb,&bp,cnt);
		send_tcp(tcb,bp);
	}
}

/* Log connection state changes; also respond to remote closes */
static
void
misc_state(tcb,old,new)
register struct tcb *tcb;
char old,new;
{
	switch(new){
	case ESTABLISHED:
		log(tcb,"open %d",tcb->conn.local.port);
		break;
	case CLOSE_WAIT:
		close_tcp(tcb);
		break;
	case CLOSED:
		log(tcb,"close %d",tcb->conn.local.port);
		del_tcp(tcb);
		/* Clean up if server is being shut down */
		if(tcb == disc_tcb)
			disc_tcb = NULLTCB;
		else if(tcb == echo_tcb)
			echo_tcb = NULLTCB;
		break;
	}
}

/* Process remote exit/reset command */
void
uremote(sock,cnt)
struct socket *sock;
int16 cnt;
{
	struct mbuf *bp;
	struct socket fsock;
	char *psocket(),command;
#ifdef SOKNAME
	char *puname();
#endif
	int recv_udp(),doexit();
	extern int exitval;

	recv_udp(sock,&fsock,&bp);
	command = pullchar(&bp);
	switch(uchar(command)){
		case SYS_RESET:
#ifdef SOKNAME
			genlog(puname(&fsock),"REMOTE RESET");
#else
			genlog(psocket(&fsock),"REMOTE RESET");
#endif
	/* sysreset reboots MS-DOS if enabled in pc.c, else does nothing.
	 * In Unix, it performs exit(3).
	 */
#ifdef UNIX
			exitval = 3;
#endif
			sysreset();
			break;	/* necessary if sysreset does nothing */

#ifdef NOTUSED	/* if you want to use this, define a SYS_RESTART in remote.h */
		case SYS_RESTART:

#ifdef SOKNAME
			genlog(puname(&fsock),"REMOTE RESTART");
#else
			genlog(psocket(&fsock),"REMOTE RESTART");
#endif
			exitval = 4;
			doexit();
#endif

		case SYS_EXIT:

#ifdef SOKNAME
			genlog(puname(&fsock),"REMOTE EXIT");
#else
			genlog(psocket(&fsock),"REMOTE EXIT");
#endif
			exitval = 2;
			doexit();	/* this will distinguish from a normal or error exit */
	}
}
