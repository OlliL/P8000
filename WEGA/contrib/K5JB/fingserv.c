/*
 *
 *	Finger support...
 *
 *	Finger server routines.  Written by Michael T. Horne - KA7AXD.
 *	Copyright 1988 by Michael T. Horne, All Rights Reserved.
 *	Permission granted for non-commercial use and copying, provided
 *	that this notice is retained.
 *
 *  Fixed the jacking around with \n in unix systems so finger wouldn't
 *  send a continuous stream of characters - sheesh!  - K5JB
 */

#include "config.h"
#ifdef _FINGER
#include <stdio.h>
#include <string.h>
#include "sokname.h"
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

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* on amateur radio, unwritten convention is just end lines with CR, but
 * NOS keys on LF so we have to stick with the CR/LF convention here.
 * Made this discovery with k28.  Since MS-DOS files have CR/LF, they get
 * no special treatment, but Unix file \n is replaced by \r\n and OS9/OSK
 * files get \r replaced with \r\n.
 */

struct tcb *fing_tcb = NULLTCB;
int16 finger_notify = 1;
void free();

finger1(argc, argv)
int	argc;
char	*argv[];
{
	extern int32	ip_addr;
	struct socket	lsocket;
	void		fing_state();
	void		rcv_fing();
	int atoi();

	if (fing_tcb)
		return;
	/* start finger daemon */
	lsocket.address = ip_addr;
	if(argc < 2)
		lsocket.port = FINGER_PORT;
	else
		lsocket.port = atoi(argv[1]);
	fing_tcb = open_tcp(&lsocket, NULLSOCK, TCP_SERVER, 0, rcv_fing,
		NULLVFP, fing_state, 0, NULLCHAR);
	return;
}
/*
 *	Handle incoming finger connections and closures.
 *
 */
void
fing_state(tcb,unused,new)
struct tcb	*tcb;
char unused,		/* old state */
	new;		/* new state */
{
	struct finger	*fing;
	void snd_fing(),log();
	char *psocket();
#ifdef SOKNAME
	char *puname();
#endif

	switch(new){
	case ESTABLISHED:
		log(tcb,"open Finger");
		fing = (struct finger *) malloc(sizeof(struct finger));

		tcb->user = (char *)fing;	/* Upward pointer */
		fing->tcb = tcb;		/* Downward pointer */
		if (finger_notify)  {
			printf("You're being fingered by %s!\n",
#ifdef SOKNAME
				puname(&tcb->conn.remote));
#else
				psocket(&tcb->conn.remote));
#endif
			fflush(stdout);
		}
		return;
	case CLOSED:
		if (tcb == fing_tcb)
			fing_tcb = NULLTCB;
		if (tcb->user != NULLCHAR)
			free(tcb->user);
		del_tcp(tcb);
		break;
	}
}

/*
 *	Stop the finger server.
 */

finger0()
{
	if (fing_tcb != NULLTCB) {
		close_tcp(fing_tcb);
		fing_tcb = NULLTCB;
	}
	return;
}

#ifndef _TELNET	/* interesting dupe found */
/*
 *	Send a short message on a tcb
 */

static void
sndmsg(tcb, msg)
struct tcb	*tcb;		/* tcb to send on */
char		*msg;		/* message to send */
{
	struct mbuf *bp;

	bp = qdata(msg,(int16)strlen(msg));
	send_tcp(tcb,bp);
}
#endif

/*
 *	Finger receive upcall.  This is the guts of the finger server.
 *	The user to finger is read from the socket.  If only a newline
 *	is read, then send the remote host a list of all known 'users' on
 *	this system.
 */

void
rcv_fing(tcb, unused)
register struct tcb	*tcb;
int16 unused;
{
	FILE		*fuser;
	void filedir(),sndmsg();
	struct mbuf	*mbuf, *bp;
	char	*buf,
			*who,
			*finger_file,
			*path,
			temp[80],
			user[80];
	int	ch, cnt;
#if (UNIX || MAC || AMIGA || ATARI_ST || _OSK)
	static int savedch = FALSE;
#endif
	int	size;
	/* removed unnecessary assignment here - K5JB */
	if (((struct finger *) tcb->user) == NULLFING)	/* uh oh! */
		return;

	if(recv_tcp(tcb,&bp,FINGNAMELEN) == 0)
		return;
	if ((who = (char *)malloc(FINGNAMELEN + 1)) == NULLCHAR) { /* 8 or 11 bytes + 1 */
		free_p(bp);
		return;
	}

	cnt = pullup(&bp, who, FINGNAMELEN);	/* get 'user' name */
	who[cnt] = '\0';			/* NULL terminate it */
	free_p(bp);				/* all done with bp */

	if (*who == '\015' || *who == '\012') {	/* give him a user listing */
		int found = 0;

		path = (char *) malloc(strlen(fingerpath) + strlen(fingersuf)
			+ 2);
		/* create wildcard path to finger files */
		strcpy(path, fingerpath);
		strcat(path, "*");
		strcat(path, fingersuf);

		sndmsg(tcb, "Known users on this system:\015\012");
		for (filedir(path, 0, user); user[0] != '\0';
			filedir (path, 1, user))  {
			found++;
			*index(user, '.') = '\0';
			sprintf(temp, "        %s\015\012", user);
			sndmsg(tcb, temp);
		}
		if (!found)
			sndmsg(tcb, "None!\015\012");

		free(path);
	}
	else {
		buf = who;
		while (*buf != '\015' && *buf != '\012' && *buf != '\0')
			buf++;
		*buf = '\0';
		/*
		 *	Create path to user's finger file and see if the
		 *	the file exists.
		 */
		finger_file = (char *)malloc(strlen(fingerpath) + strlen(who)
				+ strlen(fingersuf) + 1);
		if (finger_file == NULLCHAR) {	/* uh oh... */
			free(who);		/* clean up */
			close_tcp(tcb);		/* close socket */
			return;
		}
		strcpy(finger_file, fingerpath);
		strcat(finger_file, who);
		strcat(finger_file, fingersuf);

		if((fuser = fopen(finger_file, binmode[READ_BINARY])) == NULLFILE){
			sprintf(temp, "User %s unknown on this system\015\012",who);
			sndmsg(tcb, temp);
		}
		else {				/* valid 'user' */

			/*
			 * Here's a tricky routine to make sure we get
			 * everything in, including "\r\n".  It's needed since
			 * UNIX files have only a '\n' for EOL.  What is
			 * REALLY NEEDED is a standardized routine for filling
			 * mbufs from file input so that each server doesn't
			 * have to do it themselves!  Ditto for emptying
			 * mbufs!  The problem of "\r\n" doesn't rear its
			 * ugly head with MessyDOS, but with UNIX boxes...
			 *
			 * This is REALLY UGLY -- I'll redo it sometime - K5JB
			 * I can't stand it any longer! Now is the time - 6/15/91
			 * Went back and put CR/LF back in for Unix.  At this point
			 * I will assume MS-DOS files used are correct - 1/7/92
			 */

			do{
				size = tcb->window;
				if ((mbuf = alloc_mbuf(size)) == NULLBUF) {
					fclose(fuser);	/* barf */
					free(who);
					free(finger_file);
					return;
				}
				buf = mbuf->data;	/* pointer to buffer */

#if (UNIX || MAC || AMIGA || ATARI_ST || _OSK)
				if(savedch){	/* unlikely case */
					*buf++ = '\015';
					*buf++ = '\012';
					mbuf->cnt += 2;
					size -= 2;
					savedch = FALSE;
				}
#endif
				while((ch = fgetc(fuser)) != EOF){
					switch((char)ch) {
						case '\032':	/* NO ^Z's! - they clear screens */
							break;
#if (UNIX || MAC || AMIGA || ATARI_ST || _OSK)
#ifdef _OSK
						case '\012': /* OS9/K and maybe The Mac use \r as end of */
							break;	 /* line so do this differently from Unix */
						case '\015':	 /* convert CR to CR/LF */
							ch = '\012';
#else
						case '\r':	/* ignore \r */
							break;
						case '\n':	/* convert LF to CR/LF */
#endif
							if(size == 1){ /* unlikely case, no room for 2 chars */
								savedch = TRUE;
								size = 0;	/* make it a short one - drop out */
								break;
							}
							*buf++ = '\015';
							mbuf->cnt++;
							size--;
								/* and fall through */
#endif
						default:
							*buf++ = (char)ch;
							mbuf->cnt++;
							size--;
							break;
					}
					if(size == 0)
						break;
				} /* !feof() */
				send_tcp(tcb, mbuf);	/* send info */
			}while(ch != EOF);	/* do - outside loop */
			fclose(fuser);
		}
		free(finger_file);
	}
	free(who);
	close_tcp(tcb);			/* close socket */
	return;
}
#endif
