/* SMTP Server state machine - see RFC 821
 *  enhanced 4/88 Dave Trulli nn2z
 *  fixed truncation of ill formed lines from foreign systems - K5JB
 *  fixed handling of eol in MSDOS mail files - K5JB
 */
#include <stdio.h>
#include <time.h>
#ifdef UNIX
#include <sys/types.h>
#endif
#include <ctype.h>
#include <string.h>
#include "options.h"
#include "global.h"
#include "mbuf.h"
#include "netuser.h"
#include "timer.h"
#include "tcp.h"
#include "smtp.h"

char *getname();
void mail_delete(),free(),log();
int queuejob(),tprintf(),mlock();
int validate_address();
long get_msgid();
struct list *addlist();
struct list * expandalias();
#ifdef REWRITE
#define RWBUFLEN 80
#define TRUE 1
#define FALSE 0

char *rewrite_address();
int dorewrite();
#endif

/* Command table */
static char *commands[] = {
	"helo",
#define HELO_CMD	0
	"noop",
#define NOOP_CMD	1
	"mail from:",
#define MAIL_CMD	2
	"quit",
#define QUIT_CMD	3
	"rcpt to:",
#define RCPT_CMD	4
	"help",
#define HELP_CMD	5
	"data",
#define DATA_CMD	6
	"rset",
#define RSET_CMD	7
		  NULLCHAR
};

/* Reply messages */
static char help[] = "214-Commands:\015\012\62\61\64-HELO NOOP MAIL QUIT RCPT HELP DATA RSET\015\012\62\61\64 End\015\012";
static char banner[] = "220 %s SMTP ready\015\012";
static char closing[] = "221 Closing\015\012";
static char ok[] = "250 Ok\015\012";
static char reset[] = "250 Reset state\015\012";
static char sent[] = "250 Sent\015\012";
static char ourname[] = "250 %s, Share and Enjoy!\015\012";
static char enter[] = "354 Enter mail, end with .\015\012";
static char ioerr[] = "452 Temp file write error\015\012";
static char mboxerr[] = "452 Mailbox write error\015\012";
static char badcmd[] = "500 Say WHAT??\015\012";
#ifdef notdef
static char syntax[] = "501 Syntax error\015\012";
#endif
static char badfrom[] = "503 Bad originator\015\012";	/* k35a */
static char needrcpt[] = "503 Bad recipient\015\012";
static char unknown[] = "550 <%s> address unknown\015\012";

static struct tcb *smtp_tcb;

/* Start up SMTP receiver service */
smtp1(argc,argv)
int argc;
char *argv[];
{
	struct socket lsocket;
	void r_mail(),s_mail();
	int atoi();
	extern int dotimer();
	static char *amount[] = { "","1800" };

	lsocket.address = ip_addr;
	if(argc < 2)
		lsocket.port = SMTP_PORT;
	else
		lsocket.port = atoi(argv[1]);

	smtp_tcb = open_tcp(&lsocket,NULLSOCK,
		TCP_SERVER,0,r_mail,NULLVFP,s_mail,0,NULLCHAR);	/* K5JB */
	dotimer(2,amount);	/* k34 */
}

/* Shutdown SMTP service (existing connections are allowed to finish) */
smtp0()
{
	if(smtp_tcb != NULLTCB)
		close_tcp(smtp_tcb);
}

/* SMTP connection state change upcall handler */
static void
s_mail(tcb,unused,new)
struct tcb *tcb;
char unused,new;
{
	struct mail *mp,*mail_create();

	switch(new){
#ifdef  QUICKSTART
	case SYN_RECEIVED:
#else
	case ESTABLISHED:
#endif
		if((mp = mail_create(tcb)) == NULLMAIL){
			close_tcp(tcb);
			break;
		}
		(void) tprintf(mp->tcb,banner,hostname);
		log(tcb,"open SMTP");
		break;
	case CLOSE_WAIT:
		close_tcp(tcb);
		break;
	case CLOSED:
		log(tcb,"close SMTP");
		mp = (struct mail *)tcb->user;
		mail_delete(mp);
		del_tcp(tcb);
		/* Check if server is being shut down */
		if(tcb == smtp_tcb)
			smtp_tcb = NULLTCB;
		break;
	}
}

/* SMTP receiver upcall handler */
static void
r_mail(tcb,cnt)
struct tcb *tcb;
int16 cnt;
{
	register struct mail *mp;
	char c;
	struct mbuf *bp;
	char *inet_ntoa();
	void docommand(),doline();

	if((mp = (struct mail *)tcb->user) == NULLMAIL){
		/* Unknown session */
		close_tcp(tcb);
		return;
	}
	recv_tcp(tcb,&bp,cnt);
	/* Assemble an input line in the session buffer.
	 * Return if incomplete
	 */
	while(pullup(&bp,&c,1) == 1){
		switch(c){
		case '\015':      /* Strip cr's */
#ifdef MSDOS
		case '\032':    /* Strip ctrl/Z's */
#endif
			continue;
		case '\012':      /* Complete line; process it */
			mp->buf[mp->cnt] = '\0';
			doline(mp);
			break;
		default:	/* Assemble line */
			mp->buf[mp->cnt++] = c;  /* too-long line fix - K5JB */
			if(mp->cnt == LINELEN-1){
				mp->buf[mp->cnt] = '\0'; /* stop that line */
				doline(mp);
			}
		break;
		}
	}
}

/* Process a line read on an SMTP connection (any state) */
static void
doline(mp)
register struct mail *mp;
{
	void docommand(),deliver();

	switch(mp->state){
	case COMMAND_STATE:
		docommand(mp);
		break;
	case DATA_STATE:
		tcp_output(mp->tcb);    /* Send ACK; disk I/O is slow */
		if(mp->buf[0] == '.' && mp->buf[1] == '\0'){
			mp->state = COMMAND_STATE;
		/* Also sends appropriate response */
			deliver(mp);
#if (defined(_OSK) && !defined(_UCC))
			tmpclose(mp->data);
#else
			fclose(mp->data);
#endif
			mp->data = NULLFILE;
			del_list(mp->to);
			mp->to = NULLLIST;
			break;
		}
		/* for UNIX mail compatiblity */
		if (strncmp(mp->buf,"From ",5) == 0)
			(void) putc('>',mp->data);
		/* Append to data file */
		if(fprintf(mp->data,"%s\n",mp->buf) < 0){
			mp->state = COMMAND_STATE;
			(void) tprintf(mp->tcb,ioerr);
		}
		break;
	}
	mp->cnt = 0;
}

/* Create control block, initialize */
static struct mail *
mail_create(tcb)
register struct tcb *tcb;
{
	register struct mail *mp;

	if((mp = (struct mail *)calloc(1,sizeof (struct mail))) == NULLMAIL)
		return NULLMAIL;
	mp->tcb = tcb;	  /* Downward pointer */
	tcb->user = (char *)mp; /* Upward pointer */
	return mp;
}

/* Free resources, delete control block */
static void
mail_delete(mp)
register struct mail *mp;
{

	if (mp == NULLMAIL)
		return;
	if(mp->system != NULLCHAR)
		free(mp->system);
	if(mp->from != NULLCHAR)
		free(mp->from);
	if(mp->data != NULLFILE)
#if (defined(_OSK) && !defined(_UCC))
		tmpclose(mp->data);
#else
		fclose(mp->data);
#endif
	del_list(mp->to);
	free((char *)mp);
}

/* Parse and execute mail commands */
static void
docommand(mp)
register struct mail *mp;
{
	register char **cmdp,*arg,*cp,*cmd;
	FILE *tmpfile();
	time_t t;
	char address_type;
#ifdef REWRITE
	char *newaddr, buf[RWBUFLEN];
#endif

	cmd = mp->buf;
	if(mp->cnt < 4){
		/* Can't be a legal SMTP command */
		(void) tprintf(mp->tcb,badcmd);
		return;
	}
	cmd = mp->buf;

	/* Translate entire buffer to lower case */
	for(cp = cmd;*cp != '\0';cp++)
		*cp = tolower(*cp);

	/* Find command in table; if not present, return syntax error */
	for(cmdp = commands;*cmdp != NULLCHAR;cmdp++)
		if(strncmp(*cmdp,cmd,strlen(*cmdp)) == 0)
			break;
	if(*cmdp == NULLCHAR){
		(void) tprintf(mp->tcb,badcmd);
		return;
	}
	arg = &cmd[strlen(*cmdp)];
	/* Skip spaces after command */
	while(*arg == ' ')
		arg++;
	/* Execute specific command */
	switch(cmdp-commands){
	case HELO_CMD:
		if(mp->system != NULLCHAR)
			free(mp->system);
		if((mp->system = malloc((unsigned)strlen(arg)+1)) == NULL){
			/* If the system is out of memory, just close */
			close_tcp(mp->tcb);
			break;
		} else {
			strcpy(mp->system,arg);
			(void) tprintf(mp->tcb,ourname,hostname);
		}
		break;
	case NOOP_CMD:
		(void) tprintf(mp->tcb,ok);
		break;
	case MAIL_CMD:
		if(mp->from != NULLCHAR)
			free(mp->from);
		if((mp->from = malloc((unsigned)strlen(arg)+1)) == NULLCHAR){
			/* If the system is out of memory, just close */
			close_tcp(mp->tcb);
			break;
		} else {
			if((cp = getname(arg)) == NULLCHAR){
				(void) tprintf(mp->tcb,badfrom);
				break;	/* memory leak here?  Note mp->from now unused */
			}
			strcpy(mp->from,cp);
			(void) tprintf(mp->tcb,ok);
		}
		break;
	case QUIT_CMD:
		(void) tprintf(mp->tcb,closing);
		close_tcp(mp->tcb);
		break;
	case RCPT_CMD:  /* Specify recipient - corrected getname, added 2 tests */
		if((cp = getname(arg)) == NULLCHAR){
			(void) tprintf(mp->tcb,needrcpt);	/* was syntax - k35a */
			break;
		}

#ifdef REWRITE
		if((newaddr = rewrite_address(cp)) != NULLCHAR){
			strcpy(buf,newaddr);
			cp = buf;
			free(newaddr);
		}
#endif
		/* check if address is ok */
		if ((address_type = validate_address(cp)) == BADADDR) {
			(void) tprintf(mp->tcb,unknown,cp);
			break;
		}
		/* if a local address check for an alias */
		if (address_type == LOCAL)
			expandalias(&mp->to, cp);
		else
			/* a remote address is added to the list */
			addlist(&mp->to, cp, address_type);

		(void) tprintf(mp->tcb,ok);
		break;
	case HELP_CMD:
		(void) tprintf(mp->tcb,help);
		break;
	case DATA_CMD:
		if(mp->to == NULLLIST){
			(void) tprintf(mp->tcb,needrcpt);
			break;
		}
		tcp_output(mp->tcb);    /* Send ACK; disk I/O is slow */
		if((mp->data = tmpfile()) == NULLFILE){
			(void) tprintf(mp->tcb,ioerr);
			break;
		}
		/* Add timestamp; ptime adds newline */
		time(&t);
		fprintf(mp->data,"Received: ");
		if(mp->system != NULLCHAR)
			fprintf(mp->data,"from %s ",mp->system);
		fprintf(mp->data,"by %s with SMTP\n\tid AA%ld ; %s",
				hostname, get_msgid(), ptime(&t));
		if(ferror(mp->data)){
			(void) tprintf(mp->tcb,ioerr);
		} else {
			mp->state = DATA_STATE;
			(void) tprintf(mp->tcb,enter);
		}
		break;
	case RSET_CMD:
		del_list(mp->to);
		mp->to = NULLLIST;
		mp->state = COMMAND_STATE;
		(void) tprintf(mp->tcb,reset);
		break;
	}
}

/* Given a string of the form <user@host>, extract the part inside the
 * brackets and return a pointer to it.  Wasn't working.  If it is null
 * we want to return null pointer.  While we are at it, reject "@host"
 * or " @host", "host" can be interpreted as "user" and error handled
 * later in mailing cycle. - k35a
 */
static
char *
getname(cp)
register char *cp;
{
	register char *cp1;

	if((cp = strchr(cp,'<')) == NULLCHAR)
		return NULLCHAR;
	cp++;   /* cp -> first char of name */
	if((cp1 = strchr(cp,'>')) == NULLCHAR)
		return NULLCHAR;
	*cp1 = '\0';
	if(*cp == '\0' || *cp == '@' || *cp == ' ')	/* k35a */
		return NULLCHAR;
	return cp;
}

/* Deliver mail to the appropriate mail boxes and delete temp file */
static
void
deliver(mp)
register struct mail *mp;
{
	int ret,router_queue(),mailit();

	/* send to the rqueue */
	if ((smtpmode & QUEUE) != 0) {
		ret = router_queue(mp->tcb,mp->data,mp->from,mp->to);
	if (ret != 0)
		(void) tprintf(mp->tcb,ioerr);
	} else {
		ret = mailit(mp->tcb,mp->data,mp->from,mp->to);
	if (ret != 0)
		(void) tprintf(mp->tcb,mboxerr);
	}
	if (ret == 0)
		(void) tprintf(mp->tcb,sent);
}

/* used to save local mail or reroute remote mail */
int
mailit(tcb,data,from,to)
struct tcb *tcb;
FILE *data;
char *from;
struct list *to;
{
	register struct list *ap;
	register FILE *fp;
	int c;
	char mailbox[50];
	char *cp;
	char *desthost;
	int fail = 0;
	time_t t;

	for(ap = to;ap != NULLLIST;ap = ap->next) {
		fseek(data,0L,0);       /* rewind */

		/* non local mail queue it */
		if (ap->type == DOMAIN) {
			/* strange I just found a bug ; after next line - 12/17/92 */
			if ((desthost = strchr(ap->val,'@')) != NULLCHAR)
				desthost++;
			fail = queuejob(tcb,data,desthost,ap->val,from);
		} else {
			/* strip off host name */
			if ((cp = strchr(ap->val,'@')) != NULLCHAR)
				*cp = '\0';

			/* truncate long user names */
			if (strlen(ap->val) > MBOXLEN)
				ap->val[MBOXLEN] = '\0';

			/* if mail file is busy save it in our smtp queue
			 * and let the smtp daemon try later.
			 */
			if (mlock(mailspool,ap->val))
				fail = queuejob(tcb,data,hostname,ap->val,from);
			else {
				sprintf(mailbox,"%s/%s.txt",mailspool,ap->val);
#ifdef MSDOS	/* eol fix in mail files - K5JB  */
				if((fp = fopen(mailbox,"a+t")) != NULLFILE)
#else
				if((fp = fopen(mailbox,"a+")) != NULLFILE)
#endif
				{
				/* this newline stuff is to strip off surplus newlines that
				 * are added by various mailing agents.  This _should_ work
				 * as with _OS9 - K5JB
				 */
					int newlines = 0;	/* k35 */
					time(&t);
					fprintf(fp,"From %s %s",from,ctime(&t));
					while((c = getc(data)) != EOF){
						if(c == '\n'){
							newlines++;
							continue;
						}
						for(;newlines;newlines--)
							if(putc('\n',fp) == EOF)
								break;	/* next line will catch error */
						if(putc(c,fp) == EOF)
							break;
					}
					if(ferror(fp))
						fail = 1;
					else	/* Leave only one blank line between msgs */
						fprintf(fp,"\n\n");
					fclose(fp);
					printf("New mail arrived for %s\n",ap->val);
					fflush(stdout);
				} else
					fail = 1;
				(void) rmlock(mailspool,ap->val);
				if (fail)
					break;
				log(tcb,"SMTP recv: To: %s From: %s",ap->val,from);
			} /* else */
		} /* else */
	} /* for */
	return(fail) ;
}

/* Return Date/Time in Arpanet format in passed string */
char *
ptime(t)
long *t;
{
	/* Print out the time and date field as
	 *	      "DAY day MONTH year hh:mm:ss ZONE"
	 */
   register struct tm *ltm;
   static char tz[4];
   static char str[40];
   extern char *getenv();
   extern struct tm *localtime();
   char *p;
   static char *days[7] = {
    "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };

   static char *months[12] = {
     "Jan","Feb","Mar","Apr","May","Jun",
     "Jul","Aug","Sep","Oct","Nov","Dec" };

	/* Fill the tm structure */
	ltm = localtime(t);

	/* this version caused timezone to go away when localtime() was called
	a second time - see note below */

	if (*tz == '\0'){	/* did some work to prevent array over-writes */
		if ((p = getenv("TZ")) == NULLCHAR)
			strcpy(tz,"GMT");
		else{
			if(ltm->tm_isdst && strlen(p) >= 7)
				p += 4;      /* TZ will be 7 chars for CST6CDT */
			/*	p[3] = '\0';	 This was scrambling the env vbl TZ! */
			strncpy(tz,p,3);
		}
	}

	/* rfc 822 format */
	sprintf(str,"%s, %.2d %s %02d %02d:%02d:%02d %.3s\n",
		days[ltm->tm_wday],
		ltm->tm_mday,
		months[ltm->tm_mon],
		ltm->tm_year,
		ltm->tm_hour,
		ltm->tm_min,
		ltm->tm_sec,
		tz);
	return(str);
}

long
get_msgid()
{
	char sfilename[LINELEN];
	char s[20];
	register long sequence = 0;
	FILE *sfile;
	long atol();

	sprintf(sfilename,"%s/sequence.seq",mailqdir);
	sfile = fopen(sfilename,"r");	/* this can fail but we recover later */

	/* if sequence file exists, get the value, otherwise set it */
	if (sfile != NULLFILE) {
		(void) fgets(s,sizeof(s),sfile);
		sequence = atol(s);
	/* Keep it in range of and 8 digit number to use for dos name prefix. */
		if (sequence < 0L || sequence > 99999999L )
			sequence = 0;
		fclose(sfile);
	}

	/* increment sequence number, and write to sequence file */
	/* this was a tough bug to find.  If user has no directories set up
	 * we want this to fail gracefully - K5JB */
	if((sfile = fopen(sfilename,"w")) == NULLFILE)
		return 0l;
	fprintf(sfile,"%ld",++sequence);
	fclose(sfile);
	return sequence;
}

#ifdef  MSDOS
/* Illegal characters in a DOS filename */
static char baddoschars[] = "\"[]:|<>+=;,";
#endif

/* test if mail address is valid */
int
validate_address(s)
char *s;
{
	char *cp;
	int32 addr;
	int32 mailroute();

	/* if address has @ in it the check dest address */
	if ((cp = strchr(s,'@')) != NULLCHAR) {
		cp++;
		/* 1st check if its our hostname
		 * if not then check the hosts file and see
		 * if we can resolve ther address to a known site
		 * or one of our aliases
		 */
		if (strcmp(cp,hostname) != 0) {
			if ((addr = mailroute(cp)) == 0 && (smtpmode & QUEUE) == 0)
				return BADADDR;
			if (addr != ip_addr)
				return DOMAIN;
		}

		/* on a local address remove the host name part */
		*--cp = '\0';
	}

	/* if using an external router leave address alone */
	if ((smtpmode & QUEUE) != 0)
		return LOCAL;

	/* check for the user%host hack */
	/* k35 changed from strchr() so user%host2%host2@host would work */
	if ((cp = strrchr(s,'%')) != NULLCHAR) {
		*cp = '@';
		cp++;
		/* reroute based on host name following the % separator */
		if (mailroute(cp) == 0)
			return BADADDR;
		else
			return DOMAIN;
	}

#ifdef MSDOS    /* dos file name checks */
	/* Check for characters illegal in MS-DOS file names */
	for(cp = baddoschars;*cp != '\0';cp++){
		if(strchr(s,*cp) != NULLCHAR)
			return BADADDR;
	}
#endif
	return LOCAL;
}

/* place a mail job in the outbound queue */
int
queuejob(tcb,dfile,host,to,from)
struct tcb *tcb;
FILE *dfile;
char *host,*to,*from;
{
	FILE *fp;
	char tmpstring[50];
	char prefix[9];
	register int c;

	sprintf(prefix,"%ld",get_msgid());
	log(tcb,"SMTP queue job %s To: %s From: %s",prefix,to,from);
	mlock(mailqdir,prefix);	/* this can fail, but we won't return yet */
	sprintf(tmpstring,"%s/%s.txt",mailqdir,prefix);
#ifdef MSDOS
	if((fp = fopen(tmpstring,"wt")) == NULLFILE)
#else
	if((fp = fopen(tmpstring,"w")) == NULLFILE)
#endif
	{
		(void) rmlock(mailqdir,prefix);
		return 1;
	}
	while((c = getc(dfile)) != EOF){
		if(c == 0x1a)	/* found ^z making it into n.txt - K5JB */
			continue;
		if(putc(c,fp) == EOF)
			break;
	}
	if(ferror(fp)){
		fclose(fp);
		(void) rmlock(mailqdir,prefix);
		return 1;
	}
	fclose(fp);
	sprintf(tmpstring,"%s/%s.wrk",mailqdir,prefix);
#ifdef MSDOS
	if((fp = fopen(tmpstring,"wt")) == NULLFILE)
#else
	if((fp = fopen(tmpstring,"w")) == NULLFILE)
#endif
	{
		(void) rmlock(mailqdir,prefix);
		return 1;
	}
	fprintf(fp,"%s\n%s\n%s\n",host,from,to);
	fclose(fp);
	(void) rmlock(mailqdir,prefix);
	return 0;
}

/* Deliver mail to the appropriate mail boxes */
int
router_queue(tcb,data,from,to)
struct tcb *tcb;
FILE *data;
char *from;
struct list *to;
{
	int c;
	register struct list *ap;
	FILE *fp;
	char tmpstring[50];
	char prefix[9];

	sprintf(prefix,"%ld",get_msgid());
	mlock(routeqdir,prefix);
	sprintf(tmpstring,"%s/%s.txt",routeqdir,prefix);
#ifdef MSDOS
	if((fp = fopen(tmpstring,"wt")) == NULLFILE)
#else
	if((fp = fopen(tmpstring,"w")) == NULLFILE)
#endif
	{
		(void) rmlock(routeqdir,prefix);
		return 1;
	}
	fseek(data,0L,0);       /* rewind */
	while((c = getc(data)) != EOF)
		if(putc(c,fp) == EOF)
			break;
	if(ferror(fp)){
		fclose(fp);
		(void) rmlock(routeqdir,prefix);
		return 1;
	}
	fclose(fp);
	sprintf(tmpstring,"%s/%s.wrk",routeqdir,prefix);
#ifdef MSDOS
	if((fp = fopen(tmpstring,"wt")) == NULLFILE)
#else
	if((fp = fopen(tmpstring,"w")) == NULLFILE)
#endif
	{
		(void) rmlock(routeqdir,prefix);
		return 1;
	}
	fprintf(fp,"From: %s\n",from);
	for(ap = to;ap != NULLLIST;ap = ap->next) {
		fprintf(fp,"To: %s\n",ap->val);
	}
	fclose(fp);
	(void) rmlock(routeqdir,prefix);
	log(tcb,"SMTP rqueue job %s From: %s",prefix,from);
	return 0;
}

/* add an element to the front of the list pointed to by head
** return NULLLIST if out of memory.
*/
struct list *
addlist(head,val,type)
struct list **head;
char *val;
int type;
{
	register struct list *tp;

	tp = (struct list *)calloc(1,sizeof(struct list));
	if (tp == NULLLIST)
		return NULLLIST;

	tp->next = NULLLIST;

	/* allocate storage for the char string */
	if ((tp->val = (char *)malloc((unsigned)strlen(val)+1)) == NULLCHAR) {
		(void) free((char *)tp);
		return NULLLIST;
	}
	strcpy(tp->val,val);
	tp->type = type;

	/* add entry to front of existing list */
	if (*head == NULLLIST)
		*head = tp;
	else {
		tp->next = *head;
		*head = tp;
	}
	return tp;
}

#define SKIPWORD(X) while(*X && *X!=' ' && *X!='\t' && *X!='\n' && *X!= ',') X++;
#define SKIPSPACE(X) while(*X ==' ' || *X =='\t' || *X =='\n' || *X == ',') X++;

/* check for and alias and expand alias into a address list */
struct list *
expandalias(head, user)
struct list **head;
char *user;
{
	void rip();
	FILE *fp;
	register char *s,*p;
	int inalias;
	struct list *tp;
	char buf[LINELEN];

		/* no alias file found */
	if ((fp = fopen(alias, "r")) == NULLFILE)
		return addlist(head, user, LOCAL);

	inalias = 0;
	while (fgets(buf,LINELEN,fp) != NULLCHAR) {
		p = buf;
		if ( *p == '#' || *p == '\0')
			continue;
		rip(p);

		/* if not in an matching entry skip continuation lines */
		if (!inalias && isspace(*p))
			continue;

		/* when processing an active alias check for a continuation */
		if (inalias) {
			if (!isspace(*p))
				break;  /* done */
		} else {
			s = p;
			SKIPWORD(p);
			*p++ = '\0';    /* end the alias name */
			if (strcmp(s,user) != 0)
				continue;       /* no match go on */
			inalias = 1;
		}

		/* process the recipients on the alias line */
		SKIPSPACE(p);
		while(*p != '\0' && *p != '#') {
			s = p;
			SKIPWORD(p);
			if (*p != '\0')
				*p++ = '\0';

			/* find hostname */
			if (strchr(s,'@') != NULLCHAR)
				tp = addlist(head,s,DOMAIN);
			else
				tp = addlist(head,s,LOCAL);
			SKIPSPACE(p);
		}
	}
	(void) fclose(fp);

	if (inalias)    /* found and processed and alias. */
		return tp;

	/* no alias found treat as a local address */
	return addlist(head, user, LOCAL);
}

#ifdef REWRITE	/* swiped from NOS */

static int
Star(s,p,argv)
register char *s;
register char *p;
register char **argv;
{
	int splatmat();

	char *cp = s;
	while (splatmat(cp, p, argv) == FALSE)
		if(*++cp == '\0')
			return -1;
	return (int)(cp - s);
}

/* shortened the RE pattern matcher here to only what we need - K5JB */
int
splatmat(s,p,argv)
register char *s;
register char *p;
register char **argv;
{
	register int cnt;

	for(; *p; s++,p++){
		switch(*p){
			default:
				if(*s != *p)
					return FALSE;
			continue;
		case '*':
			/* Trailing star matches everything. */
			if(argv == (char **)0)
				return *++p ? 1 + Star(s, p, (char **)0) : TRUE;
			if(*++p == '\0')
				cnt = strlen(s);
			else
				if((cnt = Star(s, p, argv+1)) == -1)
					return FALSE;
			*argv = malloc(cnt+1);
			strncpy(*argv,s,cnt);
			*(*argv + cnt) = '\0';
			return TRUE;
		}
	}
	return *s == '\0';
}

/* Read the rewrite file for lines where the first word is a regular
 * expression and the second word are rewriting rules. The special
 * character '$' followed by a digit denotes the string that matched
 * a '*' character. The '*' characters are numbered from 1 to 9.
 * Example: the line "*@*.* $2@$1.ampr.org" would rewrite the address
 * "foo@bar.xxx" to "bar@foo.ampr.org".
 * $H is replaced by our hostname, and $$ is an escaped $ character.
 * If the third word on the line has an 'r' character in it, the function
 * will recurse with the new address.
 */
char *
rewrite_address(addr)
char *addr;
{
	char *argv[10], buf[RWBUFLEN], *cp, *cp2, *retstr;
	int cnt;
	FILE *fp;
	extern char hostname[],*Rewritefile;
	void rip();

	if ((fp = fopen(Rewritefile,"rt")) == NULLFILE)
		return NULLCHAR;

	memset((char *)argv,0,10*sizeof(char *));
	while(fgets(buf,sizeof(buf),fp) != NULLCHAR) {
		if(*buf == '#')	 /* skip commented lines */
			continue;

		if((cp = strchr(buf,' ')) == NULLCHAR) /* get the first word */
			if((cp = strchr(buf,'\t')) == NULLCHAR)
				continue;
		*cp = '\0';
		if((cp2 = strchr(buf,'\t')) != NULLCHAR){
			*cp = ' ';
			cp = cp2;
			*cp = '\0';
		}
		if(!splatmat(addr,buf,argv))
			continue;	   /* no match */
		rip(++cp);
		/* scan past additional whitespaces */
		while (*cp == ' ' || *cp == '\t') ++cp;
		cp2 = retstr = (char *) calloc(1,RWBUFLEN);
		while(*cp != '\0' && *cp != ' ' && *cp != '\t')
			if(*cp == '$'){
				if(isdigit(*(++cp)))
					if(argv[*cp - '0'-1] != '\0')
						strcat(cp2,argv[*cp - '0'-1]);
				if(*cp == 'h' || *cp == 'H') /* Our hostname */
					strcat(cp2,hostname);
				if(*cp == '$')  /* Escaped $ character */
					strcat(cp2,"$");
				cp2 = retstr + strlen(retstr);
				cp++;
			}else
				*cp2++ = *cp++;
		for(cnt=0; argv[cnt] != NULLCHAR; ++cnt)
			free(argv[cnt]);
		fclose(fp);
		/* If there remains an 'r' character on the line, repeat
		 * everything by recursing.
		 */
		if(strchr(cp,'r') != NULLCHAR || strchr(cp,'R') != NULLCHAR) {
			if((cp2 = rewrite_address(retstr)) != NULLCHAR) {
				free(retstr);
				return cp2;
			}
		}
		return retstr;
	}
	fclose(fp);
	return NULLCHAR;
}

#ifdef REWRITECMD	/* smtp EXPN can be used instead of this - K5JB */
int
dorewrite(argc,argv)
int argc;
char *argv[];
{
	char *address = rewrite_address(argv[1]);
	printf("to: %s\n", address ? address : argv[1]);
	if(address)
		free(address);
	return 0;
}
#endif /* REWRITECMD */
#endif /* REWRITE */
