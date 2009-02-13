/* added "dele" for file deletion.  It was already available in
	ftpserv.c, performed general cleanup, added loop in login process, then
	added help to escape from a problem login. 9/2/91 - K5JB */
/* 11/9/93 Re-read RFC 959 and modified the logon helper.  Decided to
 * to look for 230 (succeed) instead of 550 (fail) to proceed.  Also added
 * another state, SETUP_STATE, before RECEIVING_STATE, to better control
 * data channel message flow, particularly the NOS "226 File send OK"
 * message while doing directory list or namelist.
 */
#define FTPPROMPT

/* FTP client (interactive user) code */
#define LINELEN 128     /* Length of command buffer */
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "icmp.h"
#include "timer.h"
#include "tcp.h"
#include "ftp.h"
#include "session.h"
#include "cmdparse.h"
#include "telnet.h"
#include "iface.h"
#include "ax25.h"
#include "lapb.h"
#include "finger.h"
#include "nr4.h"

extern int mode;	/* cmd mode or conv mode */
extern struct session *current;
extern char nospace[];
extern char badhost[];
char notsess[] = "Not an FTP session!\n";
char cantwrite[] = "Can't write %s\n";
char cantread[] = "Can't read %s\n";
#ifdef CUTE_FTP
char helpesc[] = "(Abort with \"close\" from command mode.)\n";
char *userprompt = "Enter user name: ";
void echo(),noecho();
#endif

int donothing(),doftpcd(),dolist(),doget(),dols(),doput(),dotype(),doabort(),
        domkdir(),dormdir(),dodele(); /* K5JB */

struct cmds ftpabort[] = {
	"",       donothing, 0, NULLCHAR,   NULLCHAR,
	"abort",  doabort,   0, NULLCHAR,   NULLCHAR,
	NULLCHAR, NULLFP,    0, NULLCHAR, NULLCHAR	/* removed message */
};

struct cmds ftpcmds[] = {
	"",         donothing,  0, NULLCHAR,               NULLCHAR,
	"cd",       doftpcd,    2, "cd <directory>",       NULLCHAR,
	"dir",      dolist,     0, NULLCHAR,               NULLCHAR,
	"get",      doget,      2, "get remotefile <localfile>",   NULLCHAR,
	"ls",       dols,       0, NULLCHAR,               NULLCHAR,
	"list",     dolist,     0, NULLCHAR,               NULLCHAR,
	"mkdir",    domkdir,    2, "mkdir <directory>",    NULLCHAR,
	"nlst",     dols,       0, NULLCHAR,               NULLCHAR,
	"rmdir",    dormdir,    2, "rmdir <directory>",    NULLCHAR,
	"put",      doput,      2, "put localfile <remotefile>",   NULLCHAR,
	"type",     dotype,     0, NULLCHAR,               NULLCHAR,
	"dele",     dodele,     2, "delete remotefile",    NULLCHAR, /* K5JB */
	NULLCHAR,   NULLFP,		0,	NULLCHAR,              NULLCHAR
};

/* Send a message on the control channel */
/*VARARGS*/
static int
sndftpmsg(ftp,fmt,arg)
struct ftp *ftp;
char *fmt;
char *arg;
{
	struct mbuf *bp;
	int16 len;

	len = strlen(fmt) + strlen(arg) + 10;   /* fudge factor */
	if((bp = alloc_mbuf(len)) == NULLBUF){
		printf(nospace);
		return 1;
	}
	sprintf(bp->data,fmt,arg);
	bp->cnt = strlen(bp->data);
	send_tcp(ftp->control,bp);
	return 0;
}


/* Parse user FTP commands */
static int
ftpparse(line,len)
char *line;
int16 len;
{
	struct mbuf *bp;
	int cmdparse();

	line[len] = '\0';	/* oops, was left out in couple of places. Seems all
								routines need this, or won't hurt - K5JB */

	switch(current->cb.ftp->state){
		case SETUP_STATE:
		case RECEIVING_STATE:
		case SENDING_STATE:
			/* The only command allowed in data transfer state is ABORT */
			if(cmdparse(ftpabort,line) == -1)
				printf("Transfer in progress; only ABORT is acceptable\n");
			fflush(stdout);
			break;
		case COMMAND_STATE:
			/* Save it now because cmdparse modifies the original */
			bp = qdata(line,len);

			if(cmdparse(ftpcmds,line) == -1){
				/* Send it direct */
				if(bp != NULLBUF)
					send_tcp(current->cb.ftp->control,bp);
				else
					printf(nospace);
			} else {
				free_p(bp);
			}
			fflush(stdout);
			break;
		case STARTUP_STATE:             /* Starting up autologin */
#ifdef CUTE_FTP
			printf("%sNot connected yet, ignoring: %s",helpesc,line);
#else
			printf("Not connected yet, ignoring: %s",line);
#endif
			fflush(stdout);
			break;
#ifdef CUTE_FTP
		case USER_STATE:	/* Send user name */
			return sndftpmsg(current->cb.ftp,"USER %s",line);
		case PASS_STATE:
			echo();		/* Send password, turn echo back on */
			return sndftpmsg(current->cb.ftp,"PASS %s",line);
#endif
	}
	return(0);	/* satisfy the compiler */
}

/* Handle top-level FTP command */
int
doftp(argc,argv)
int argc;
char *argv[];
{
	int32 resolve();
	int atoi(),go();
	char *inet_ntoa();
	void ftpccr(),ftpccs(),cooked();
	struct session *s;
	struct ftp *ftp,*ftp_create();
	struct tcb *tcb;
	struct socket lsocket,fsocket;

	lsocket.address = ip_addr;
	lsocket.port = lport++;
	if((fsocket.address = resolve(argv[1])) == 0){
		printf(badhost,argv[1]);
		return 1;
	}
	if(argc < 3)
		fsocket.port = FTP_PORT;
	else
		fsocket.port = atoi(argv[2]);

	/* Allocate a session control block */
	if((s = newsession()) == NULLSESSION){
		printf("Too many sessions\n");
		return 1;
	}
	current = s;
	if((s->name = malloc((unsigned)strlen(argv[1])+1)) != NULLCHAR)
		strcpy(s->name,argv[1]);
	s->type = FTP;
	s->parse = ftpparse;

	/* Allocate an FTP control block */
	if((ftp = ftp_create(LINELEN)) == NULLFTP){
		s->type = FREE;
		printf(nospace);
		return 1;
	}
	ftp->state = STARTUP_STATE;
	cooked();	/* try this here instead of where it was */
	s->cb.ftp = ftp;        /* Downward link */
	ftp->session = s;       /* Upward link */

	/* Now open the control connection */
	tcb = open_tcp(&lsocket,&fsocket,TCP_ACTIVE,
			0,ftpccr,NULLVFP,ftpccs,0,(char *)ftp);
	ftp->control = tcb;
	go();
	return 0;
}

/* Handle null line to avoid trapping on first command in table */
static int
donothing(argc,argv)
int argc;
char *argv[];
{

#ifdef FTPPROMPT
	printf("FTP> ");
	fflush(stdout);
#endif
	return(0);
}
/* Translate 'cd' to 'cwd' for convenience */
static int
doftpcd(argc,argv)
int argc;
char *argv[];
{
	register struct ftp *ftp;

	ftp = current->cb.ftp;
	return sndftpmsg(ftp,"CWD %s\015\012",argv[1]);
}
/* Translate 'mkdir' to 'xmkd' for convenience */
static int
domkdir(argc,argv)
int argc;
char *argv[];
{
	register struct ftp *ftp;

	ftp = current->cb.ftp;
	return sndftpmsg(ftp,"XMKD %s\015\012",argv[1]);
}
/* Translate 'rmdir' to 'xrmd' for convenience */
static int
dormdir(argc,argv)
int argc;
char *argv[];
{
	register struct ftp *ftp;

	ftp = current->cb.ftp;
	return sndftpmsg(ftp,"XRMD %s\015\012",argv[1]);
}
/* delete file, missing for some reason - K5JB */
static int
dodele(argc,argv)
int argc;
char *argv[];
{
	register struct ftp *ftp;

	ftp = current->cb.ftp;
	return sndftpmsg(ftp,"DELE %s\015\012",argv[1]);
}
/* Handle "type" command from user */
static int
dotype(argc,argv)
int argc;
char *argv[];
{
	register struct ftp *ftp;

	ftp = current->cb.ftp;
	if(argc < 2){
		switch(ftp->type){
			case IMAGE_TYPE:
				printf("Image\n");
				break;
			case ASCII_TYPE:
				printf("Ascii\n");
				break;
			}
#ifdef FTPPROMPT
			donothing();
#endif
	}else
		switch(*argv[1]){
			case 'i':
			case 'b':
				ftp->type = IMAGE_TYPE;
				sndftpmsg(ftp,"TYPE I\015\012","");
				break;
			case 'a':
				ftp->type = ASCII_TYPE;
				sndftpmsg(ftp,"TYPE A\015\012","");
				break;
			case 'l':
				ftp->type = IMAGE_TYPE;
				sndftpmsg(ftp,"TYPE L %s\015\012",argv[2]);
				break;
			default:
				printf("Invalid type %s\n",argv[1]);
				return 1;
		}
	return 0;
}

/* create data port, and send PORT message */
static void
ftpsetup(ftp,recv,send,state)
struct ftp *ftp;
void (*send)();
void (*recv)();
void (*state)();
{
	struct socket lsocket;
	struct mbuf *bp;

	lsocket.address = ip_addr;
	lsocket.port = lport++;

	/* Compose and send PORT a,a,a,a,p,p message */

	if((bp = alloc_mbuf(35)) == NULLBUF){   /* 5 more than worst case */
		printf(nospace);
		return;
	}
	/* I know, this looks gross, but it works! */
	sprintf(bp->data,"PORT %u,%u,%u,%u,%u,%u\015\012",
		hibyte(hiword(lsocket.address)),
		lobyte(hiword(lsocket.address)),
		hibyte(loword(lsocket.address)),
		lobyte(loword(lsocket.address)),
		hibyte(lsocket.port),
		lobyte(lsocket.port));
	bp->cnt = strlen(bp->data);
	send_tcp(ftp->control,bp);

	/* Post a listen on the data connection */
	ftp->data = open_tcp(&lsocket,NULLSOCK,TCP_PASSIVE,0,
		recv,send,state,0,(char *)ftp);
}

/* FTP Client Data channel State change upcall handler */
static void
ftpcds(tcb,old,new)
struct tcb *tcb;
char old,new;
{
	struct ftp *ftp;
	void ftpccr();

	if((ftp = (struct ftp *)tcb->user) == NULLFTP){
		/* Unknown connection, kill it */
		close_tcp(tcb);
		return;
	}
	switch(new){
		case ESTABLISHED:	/* added to deal with NOS's 226 message */
			if(ftp->state == SETUP_STATE)
				ftp->state = RECEIVING_STATE;	/* output stifled in this state */
		break;

		case FINWAIT2:
		case TIME_WAIT:
			if(ftp->state == SENDING_STATE){
				/* We've received an ack of our FIN, so
				 * return to command mode
				 */
				ftp->state = COMMAND_STATE;
				if(current != NULLSESSION && current->cb.ftp == ftp){
					printf("Put complete, %lu bytes sent\n",
					tcb->snd.una - tcb->iss - 2);
					fflush(stdout);
				}
			}
			break;
		case CLOSE_WAIT:
			close_tcp(tcb);
			if(ftp->state == RECEIVING_STATE){
				/* End of file received on incoming file */
#ifdef  CPM
				if(ftp->type == ASCII_TYPE)
					putc(CTLZ,ftp->fp);
#endif
				if(ftp->fp != stdout)
					fclose(ftp->fp);
				ftp->fp = NULLFILE;
				ftp->state = COMMAND_STATE;
				if(current != NULLSESSION && current->cb.ftp == ftp){
					printf("Get complete, %lu bytes received\n",
					tcb->rcv.nxt - tcb->irs - 2);
					fflush(stdout);
					/* Now flush NOS's 226 message if there is one */
					ftpccr(ftp->control,ftp->cnt);
				}
			}
			break;
		case CLOSED:
			ftp->data = NULLTCB;
			del_tcp(tcb);
			break;
	}

}
/* Start receive transfer. Syntax: get <remote name> [<local name>] */
static int
doget(argc,argv)
int argc;
char *argv[];
{
	void ftpdr();
	char *remotename,*localname;
	register struct ftp *ftp;
	char *mode;

	ftp = current->cb.ftp;
	if(ftp == NULLFTP){
		printf(notsess);
		return 1;
	}
	remotename = argv[1];
	if(argc < 3)
		localname = remotename;
	else
		localname = argv[2];

	if(ftp->fp != NULLFILE && ftp->fp != stdout)
		fclose(ftp->fp);
	ftp->fp = NULLFILE;

	if(ftp->type == IMAGE_TYPE)
		mode = binmode[WRITE_BINARY];
	else
		mode = "w";

	if((ftp->fp = fopen(localname,mode)) == NULLFILE){
		printf(cantwrite,localname);
		donothing();
		return 1;
	}
	ftp->state = SETUP_STATE;
	ftpsetup(ftp,ftpdr,NULLVFP,ftpcds);

        /* Generate the command to start the transfer */
	return sndftpmsg(ftp,"RETR %s\015\012",remotename);
}
/* List remote directory. Syntax: dir <remote directory/file> [<local name>] */
static int
dolist(argc,argv)
int argc;
char *argv[];
{
	void ftpdr();
	register struct ftp *ftp;

	ftp = current->cb.ftp;
	if(ftp == NULLFTP){
		printf(notsess);
		return 1;
	}
	if(ftp->fp != NULLFILE && ftp->fp != stdout)
		fclose(ftp->fp);
	ftp->fp = NULLFILE;

	if(argc < 3){
		ftp->fp = stdout;
	} else if((ftp->fp = fopen(argv[2],"w")) == NULLFILE){
		printf(cantwrite,argv[2]);
#ifdef FTPPROMPT
		donothing();
#endif
		return 1;
	}
	ftp->state = SETUP_STATE;
	ftpsetup(ftp,ftpdr,NULLVFP,ftpcds);
        /* Generate the command to start the transfer
         * It's done this way to avoid confusing the 4.2 FTP server
         * if there's no argument
         */
	if(argc > 1)
		return sndftpmsg(ftp,"LIST %s\015\012",argv[1]);
	else
		return sndftpmsg(ftp,"LIST\015\012","");
}
/* Abbreviated (name only) list of remote directory.
 * Syntax: ls <remote directory/file> [<local name>]
 */
static int
dols(argc,argv)
int argc;
char *argv[];
{
	void ftpdr();
	register struct ftp *ftp;

	ftp = current->cb.ftp;
	if(ftp == NULLFTP){
		printf(notsess);
		return 1;
	}
	if(ftp->fp != NULLFILE && ftp->fp != stdout)
		fclose(ftp->fp);
	ftp->fp = NULLFILE;

	if(argc < 3){
		ftp->fp = stdout;
	} else if((ftp->fp = fopen(argv[2],"w")) == NULLFILE){
		printf(cantwrite,argv[2]);
#ifdef FTPPROMPT
		donothing();
#endif
		return 1;
	}
	ftp->state = SETUP_STATE;
	ftpsetup(ftp,ftpdr,NULLVFP,ftpcds);
        /* Generate the command to start the transfer */
	if(argc > 1)
		return sndftpmsg(ftp,"NLST %s\015\012",argv[1]);
	else
		return sndftpmsg(ftp,"NLST\015\012","");
}
/* Start transmit. Syntax: put <local name> [<remote name>] */
static int
doput(argc,argv)
int argc;
char *argv[];
{
	void ftpdt();
	char *remotename,*localname;
	char *mode;
	struct ftp *ftp;

	if((ftp = current->cb.ftp) == NULLFTP){
		printf(notsess);
		return 1;
	}
	localname = argv[1];
	if(argc < 3)
		remotename = localname;
	else
		remotename = argv[2];

	if(ftp->fp != NULLFILE && ftp->fp != stdout)
		fclose(ftp->fp);

	if(ftp->type == IMAGE_TYPE)
		mode = binmode[READ_BINARY];
	else
		mode = "r";

	if((ftp->fp = fopen(localname,mode)) == NULLFILE){
		printf(cantread,localname);
#ifdef FTPPROMPT
		donothing();
#endif
		return 1;
	}
	ftp->state = SENDING_STATE;
	ftpsetup(ftp,NULLVFP,ftpdt,ftpcds);

        /* Generate the command to start the transfer */
	return sndftpmsg(ftp,"STOR %s\015\012",remotename);
}
/* Abort a GET or PUT operation in progress. Note: this will leave
 * the partial file on the local or remote system
 */
static int
doabort()
{
	register struct ftp *ftp;

	ftp = current->cb.ftp;

	/* Close the local file */
	if(ftp->fp != NULLFILE && ftp->fp != stdout)
		fclose(ftp->fp);
	ftp->fp = NULLFILE;

	switch(ftp->state){
		case SENDING_STATE:
			/* Send a premature EOF.
			 * Unfortunately we can't just reset the connection
			 * since the remote side might end up waiting forever
			 * for us to send something.
			 */
			close_tcp(ftp->data);
			printf("Put aborted\n");
			break;
		case SETUP_STATE:
		case RECEIVING_STATE:
			/* Just exterminate the data channel TCB; this will
			 * generate a RST on the next data packet which will
			 * abort the sender
			 */
			del_tcp(ftp->data);
			ftp->data = NULLTCB;
			printf("Get aborted\n");
			break;
		}
	ftp->state = COMMAND_STATE;
	fflush(stdout);
	return(0);
}
/* FTP Client Control channel Receiver upcall routine */
void
ftpccr(tcb,cnt)
register struct tcb *tcb;
int16 cnt;
{
	struct mbuf *bp;
	struct ftp *ftp;
	void doreply();
	char c;

	if((ftp = (struct ftp *)tcb->user) == NULLFTP){
	/* Unknown connection; kill it */
		close_tcp(tcb);
		return;
	}
	/* Hold output if we're not the current session */
	if(mode != CONV_MODE || current == NULLSESSION || current->cb.ftp != ftp
		|| ftp->state == RECEIVING_STATE)	/* latter added to defer NOS's */
		return;                             /* 226 message */

	if(recv_tcp(tcb,&bp,cnt) > 0){
		while(pullup(&bp,&c,1) == 1){
			switch(c){
				case '\015':      /* Strip cr's */
					continue;
				case '\012':      /* Complete line; process it */
					ftp->buf[ftp->cnt] = '\0';
					doreply(ftp);
					ftp->cnt = 0;
					break;
				default:        /* Assemble line */
					if(ftp->cnt != LINELEN - 1)
						ftp->buf[ftp->cnt++] = c;
					break;
			}
		}
		fflush(stdout);
	}
}


/* Process replies from the server */
static void
doreply(ftp)
register struct ftp *ftp;
{

	fwrite(ftp->buf,1,(unsigned)ftp->cnt,stdout);
	printf("\n");
#ifdef notdef /* only place used in NET */
	fputc('\n', stdout);
#endif
	if (ftp->cnt < 4) return;	/* note that space after 3 numbers is rqd */
	ftp->buf[4] = '\0';
	switch(ftp->state){
		case SETUP_STATE:
		case RECEIVING_STATE:
		case SENDING_STATE:
			if (ftp->buf[0] == '5')	/* any permanent neg completion reply */
				doabort();
			break;
#ifdef CUTE_FTP
		case STARTUP_STATE:
			if (!strcmp(ftp->buf, "220 ")){	/* note required space */
				ftp->state = USER_STATE;	/* in final line of multiline reply */
				printf(userprompt);
				fflush(stdout);
			}
			break;	/* other strings are legal too */
		case USER_STATE:	/* revised this part to loop the login - K5JB */
			if(!strcmp(ftp->buf, "331-"))	/* continuation k34a */
				break;
			if (!strcmp(ftp->buf, "331 ")) {
				ftp->state = PASS_STATE;
				noecho();
				printf("Password: ");
				fflush(stdout);
			}else
				ftp->state = COMMAND_STATE;	/* something's wacko */
			break;
		case PASS_STATE:
			/* Think I prefer looking for success */
			if(ftp->buf[0] == '2')	/* 230 Logged in */
				ftp->state = COMMAND_STATE;	/* probably made it in */
			else{
				ftp->state = USER_STATE;	/* loop back and try again */
				printf("%s%s",helpesc,userprompt);
				fflush(stdout);
			}
			break;
#else	/* this method removes all the echo/noecho stuff */
		case STARTUP_STATE:
				printf("\nEnter \"user\" followed by your username and later when requested, send \"pass\"\n");
				printf("followed by your password: ");
				ftp->state = COMMAND_STATE;
			break;
#endif
	}
#ifdef FTPPROMPT
	if(ftp->state == COMMAND_STATE)
		donothing();
#endif
}

/* FTP Client Control channel State change upcall routine */
static void
ftpccs(tcb,old,new)
register struct tcb *tcb;
char old,new;
{
	void ftp_delete();
	struct ftp *ftp;
	char notify = 0;
	extern char *tcpstates[];
	extern char *reasons[];
	extern char *unreach[];
	extern char *exceed[];
	int cmdmode();
	extern int noprompt;	/* k35 */

	/* Can't add a check for unknown connection here, it would loop
	 * on a close upcall! We're just careful later on.
	 */
	ftp = (struct ftp *)tcb->user;

	if(current != NULLSESSION && current->cb.ftp == ftp)
		notify = 1;

	switch(new){
		case CLOSE_WAIT:
			if(notify)
				printf("%s\n",tcpstates[new]);
			close_tcp(tcb);
			break;
		case CLOSED:    /* heh heh */
#ifdef NOTNEEDED
			if(mode == CMD_MODE && tcb->reason == RESET)
				noprompt = 1;	/* k35 */
#endif
			if(notify){
				printf("%s (%s",tcpstates[new],reasons[tcb->reason]);
				if(tcb->reason == NETWORK){
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
			}
			del_tcp(tcb);
			if(ftp != NULLFTP)
				ftp_delete(ftp);
			break;
		default:
			if(notify)
				printf("%s\n",tcpstates[new]);
			break;
	}
	if(notify && !(new == ESTABLISHED || new == SYN_SENT || new == LAST_ACK)) /* k35 */
		cmdmode();
	fflush(stdout);
}
