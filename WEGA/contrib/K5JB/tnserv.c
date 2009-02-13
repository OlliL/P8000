#include "config.h"
#ifdef _TELNET
#include <stdio.h>
#include "sokname.h"	/* for SOKNAME */
#include "global.h"
#include "mbuf.h"
#include "timer.h"
#include "icmp.h"
#include "netuser.h"
#include "tcp.h"
#include "telnet.h"
#include "session.h"
#include "ftp.h"
#include "iface.h"
#include "ax25.h"
#include "lapb.h"
#include "finger.h"
#include "nr4.h"
#include <string.h>

struct tcb *tnet_tcb;
void log();

tn1(argc,argv)
char *argv[];
{
	struct socket lsocket;
	extern int32 ip_addr;
	void tnet_state();
	void t_state(),rcv_char();
	int atoi();

	/* Incoming Telnet */
	lsocket.address = ip_addr;
	if(argc < 2)
		lsocket.port = TELNET_PORT;
	else
		lsocket.port = atoi(argv[1]);
	tnet_tcb = open_tcp(&lsocket,NULLSOCK,TCP_SERVER,0,rcv_char,NULLVFP,
		tnet_state,0,NULLCHAR);	/* K5JB */
}
/* Handle incoming Telnet connect requests by creating a Telnet session,
 * then change upcall vector so it behaves like an ordinary Telnet session.
 *
 */
static void
tnet_state(tcb,unused,new)
struct tcb *tcb;
char unused,new;
{
	struct telnet *tn;
	struct session *s,*newsession();
#ifdef POLITE
	struct session *tmpsession;
#endif
	void tn_tx(),t_state(),sndmsg();
	char *a, *psocket();
#ifdef SOKNAME
	char *puname();
#endif
	extern struct session *current;
	int go();
#ifdef TN_MOTD
	extern char tn_motd[];
#endif
	switch(new){
	case ESTABLISHED:
		log(tcb,"open Telnet");
		/* Allocate a session descriptor */
		if((s = newsession()) == NULLSESSION){
			printf("\007Incoming Telnet call from %s refused; too many sessions\n",
#ifdef SOKNAME
			 puname(&tcb->conn.remote));
#else
			 psocket(&tcb->conn.remote));
#endif
			fflush(stdout);
			sndmsg(tcb,"Call rejected; too many sessions on remote system\015\012");
			close_tcp(tcb);
			return;
		}
#ifdef SOKNAME
		a = puname(&tcb->conn.remote.address);
#else
		a = inet_ntoa(tcb->conn.remote.address);
#endif
		if((s->name = (char *)malloc((unsigned)strlen(a)+1)) != NULLCHAR)
			strcpy(s->name,a);
		s->type = TELNET;
		s->parse = send_tel;
		/* Create and initialize a Telnet protocol descriptor */
		if((tn = (struct telnet *)calloc(1,sizeof(struct telnet))) == NULLTN){
			printf("\007Incoming Telnet call refused; no space\n");
			fflush(stdout);
			sndmsg(tcb,"Call rejected; no space on remote system\015\012");
			close_tcp(tcb);
			s->type = FREE;
			return;
		}
		tn->session = s;	/* Upward pointer */
		tn->state = TS_DATA;
		s->cb.telnet = tn;	/* Downward pointer */

		tcb->user = (char *)tn;	/* Upward pointer */
		tn->tcb = tcb;		/* Downward pointer */
		printf("\007Incoming Telnet session %lu from %s\n",
#ifdef SOKNAME
		 (long)(s - sessions),puname(&tcb->conn.remote));
#else
		 (long)(s - sessions),psocket(&tcb->conn.remote));
#endif
		fflush(stdout);
		tcb->s_upcall = t_state;
		tcb->t_upcall = tn_tx;	/* oops, was missing k33 */
#ifdef POLITE
		tmpsession = current; /* may need this later */
#endif
		current = s;    /* this may be only temporary if we are busy */
#ifdef TN_MOTD
		if(tn_motd[0])
			sndmsg(tcb,tn_motd);
#endif
#ifdef POLITE
		if((s - sessions) == 0)   /* if we aren't busy */
			go();
		else
			current = tmpsession;	/* back to what we were doing */
#else
		go();
#endif
		return;
	case CLOSED:
		/* This will only happen if the connection closed before
		 * the session was set up, e.g., if we refused it because
		 * there were too many sessions, or if the server is being
		 * shut down.
		 */
		if(tcb == tnet_tcb)
			tnet_tcb = NULLTCB;
		del_tcp(tcb);
		break;
	}
}
/* Shut down Telnet server */
tn0()
{
	if(tnet_tcb != NULLTCB)
		close_tcp(tnet_tcb);
}
void
sndmsg(tcb,msg)
struct tcb *tcb;
char *msg;
{
	struct mbuf *bp;

	bp = qdata(msg,(int16)strlen(msg));
	send_tcp(tcb,bp);
}
#endif /* _TELNET */
