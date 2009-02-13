/*
 *
 *	Finger support...
 *
 *	Finger client routines.  Written by Michael T. Horne - KA7AXD.
 *	Copyright 1988 by Michael T. Horne, All Rights Reserved.
 *	Permission granted for non-commercial use and copying, provided
 *	that this notice is retained.
 * I'll retain it, but I cleaned it up - K5JB
 */

#include <stdio.h>
#include "config.h"	/*K5JB*/

#ifdef _FINGER
#include <string.h>
#include "global.h"
#include "mbuf.h"
#include "timer.h"
#include "internet.h"
#include "icmp.h"
#include "netuser.h"
#include "tcp.h"
#include "ftp.h"
#include "telnet.h"
#include "iface.h"
#include "ax25.h"
#include "lapb.h"
#include "finger.h"
#include "session.h"
#include "nr4.h"

extern char	badhost[],hostname[];
int free_finger();

/*
 *
 *	Open up a socket to a remote (or the local) host on its finger port.
 *
 */

int
dofinger(argc,argv)
int	argc;
char	*argv[];
{
	void f_state(),fingcli_rcv();
	char *inet_ntoa();
	int32 resolve();
	struct session	*s;
	struct tcb	*tcb;
	struct socket	lsocket,fsocket;
	struct finger	*finger,*alloc_finger();
	char *host;
	int go();
	char *usage = "usage: finger [user | user@host | @host]\n";

	if (argc < 2){
		printf("%s",usage);
		return(1);
	}

	lsocket.address = ip_addr;
	lsocket.port = lport++;

/*
 *	Extract user/host information.  It can be of the form:
 *
 *	finger user,			# finger local user
 *	finger user@host,		# finger remote user
 *	finger @host			# finger host (give system status)
 *
 */
	if ((finger = alloc_finger()) == NULLFING)
		return(1);

	if ((host = strchr(argv[1], '@')) == NULLCHAR) {
		fsocket.address = ip_addr;	/* no host, use local */
		host = hostname;		/* use local host name */
	}else{
		*host++ = '\0';		/* null terminate user name */
		if (*host == '\0') {	/* must specify host */
			printf("finger: no host specified\n%s",usage);
			free_finger(finger);
			return(1);
		}
		if ((fsocket.address = resolve(host)) == 0) {
			printf(badhost, host);
			free_finger(finger);
			return(1);
		}
	}
	if ((finger->user = (char *)malloc(strlen(argv[1]) + 3)) == NULLCHAR){
		free_finger(finger);
		return(1);
	}
	sprintf(finger->user,"%s\015\012",argv[1]);

	fsocket.port = FINGER_PORT;		/* use finger wnp */

	/* Allocate a session descriptor */
	if ((s = newsession()) == NULLSESSION){
		printf("Too many sessions\n");
		free_finger(finger);
		return 1;
	}
	current = s;
	s->cb.finger = finger;
	finger->session = s;

	if ((s->name = (char *)malloc(strlen(host)+1)) != NULLCHAR)
		strcpy(s->name, host);

	s->type = FINGER;
	s->parse = NULLFP;	/* K5JB */

	tcb = open_tcp(&lsocket, &fsocket, TCP_ACTIVE, 0,
	 fingcli_rcv, (void (*)()) 0, f_state, 0, (int *) finger);

	finger->tcb = tcb;
	tcb->user = (char *)finger;
	/* with finger, we will make an exception and stay in CMD_MODE */
	go();
	return 0;
}

/*
 *	Allocate a finger structure for the new session
 */
struct finger *
alloc_finger()
{
	struct finger *tmp;

	if ((tmp = (struct finger *) malloc(sizeof(struct finger))) == NULLFING)
		return(NULLFING);
	tmp->session = NULLSESSION;
	tmp->user = NULLCHAR;
	return(tmp);
}

/*
 *	Free a finger structure
 */
int
free_finger(finger)
struct finger *finger;
{
	void free(),freesession();
	if (finger != NULLFING) {
		if (finger->session != NULLSESSION)
			freesession(finger->session);
		if (finger->user != NULLCHAR)
			free(finger->user);
		free(finger);
	}
	return 0;
}

/* Finger receiver upcall routine */
void
fingcli_rcv(tcb, cnt)
register struct tcb	*tcb;
int16			cnt;
{
	struct mbuf	*bp;
	char		*buf;

	/* Make sure it's a valid finger session */
	if ((struct finger *) tcb->user == NULLFING) {
		return;
	}

	/* Hold output if we're not the current session */
#ifdef notdef /* need to remove the mode with change to cmdmode() k34 */
	if (mode != CONV_MODE || current == NULLSESSION
		|| current->type != FINGER)
		return;
#endif
	if (current == NULLSESSION || current->type != FINGER)
		return;

	/*
	 *	We process the incoming data stream and make sure it
	 *	meets our requirments.  A check is made for control-Zs
	 */

	if (recv_tcp(tcb, &bp, cnt) > 0)
		while (bp != NULLBUF) {
			buf = bp->data;
			while(bp->cnt--) {
				switch(*buf) {
					case '\012':	/* ignore LF */
					case '\032':	/* NO ^Z's! */
						break;
					case '\015':
#ifdef notdef /* what the hell are we doing with fputc()? */
#ifdef _OSK
						fputc('\015', stdout);
#else
						fputc('\012', stdout);
#endif
#endif
						printf("\n");
						break;
					default:
						fputc(*buf, stdout);
						break;
				}
				buf++;
			}
			bp = free_mbuf(bp);
		}
	fflush(stdout);
}

/* State change upcall routine */
void
f_state(tcb,unused,new)
register struct tcb	*tcb;
char unused,		/* old state */
			new;		/* new state */
{
	struct finger	*finger;
	char		notify = 0;
	extern char	*tcpstates[];
	extern char	*reasons[];
	extern char	*unreach[];
	extern char	*exceed[];
	struct mbuf	*bp;
	int cmdmode();

	finger = (struct finger *)tcb->user;

	if(current != NULLSESSION && current->type == FINGER)
		notify = 1;

	switch(new){	/* some of this is a dupe of telnet.c state handler */

	case CLOSE_WAIT:
		if(notify)
			printf("%s\n",tcpstates[new]);
		close_tcp(tcb);
		break;

	case CLOSED:	/* finish up */
		if(notify) {
			printf("%s (%s", tcpstates[new], reasons[tcb->reason]);
			if (tcb->reason == NETWORK){
				switch(tcb->type){
				case DEST_UNREACH:
					printf(": %s unreachable",unreach[tcb->code]);
					break;
				case TIME_EXCEED:
					printf(": %s time exceeded",exceed[tcb->code]);
					break;
				}
			}
			printf(")\n");
			cmdmode();
		}
		if(finger != NULLFING)
			free_finger(finger);
		del_tcp(tcb);
		break;
	case ESTABLISHED:
		if (notify) {
			printf("%s\n",tcpstates[new]);
		}
		printf("[%s]\n", current->name);
		bp = qdata(finger->user, (int16) strlen(finger->user));
		send_tcp(tcb, bp);
		break;
		
	default:
		if(notify)
			printf("%s\n",tcpstates[new]);
		break;
	}
	fflush(stdout);
}

#endif
