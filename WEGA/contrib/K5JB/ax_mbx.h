/* mail message header */

struct mhdr {
	struct mhdr *next;
	struct mhdr *prev;
	int status;
#define STATREAD 1	/* Read already */
#define STATKILL 2   /* To be killed */
#define STATNREAD 4  /* Read in this session */
	int msgnr;
	char from[23];	/* fixing size here and using calloc saved 112 bytes */
	char date[16];
	int size;
	char subj[31];
	long fileptr;
	};

/* Defines for the ax.25 mailbox facility */

#define NUMMBX		10		/* max number of mailbox sessions */
#define MAXTQUEUE	5		/* max number of frames in ax.25 queue on file send */
#define MBXLINE	128		/* max length of line */

/* modified to match session struct to use existing tx upcalls */
struct mbx {
	int type ;				/* type of connection */
#define MBX_AX25	1		/* ax25 connection */
#ifdef NETROM
#define MBX_NETROM	2		/* net/rom transport connection */
#endif
	char *name;				/* Name of remote station - changed from name[10] */
	union {					/* to match struct session - free() used on exit */
		struct ax25_cb	*ax25_cb ;	/* ax.25 link control block */
#ifdef NETROM
		struct nr4cb *nr4_cb ;		/* net/rom link control block */
#endif
	} cb ;
	int (*parse)();		/* a dummy for now */
	FILE *tfile ;			/* Temporary file for message */
	char *to ;				/* To-address in form user or user@host */
	FILE *upload;			/* Send file descrip. - added */
	char *ufile;			/* send file name - added */
/* end of mod to match session struct */
	char *tofrom ;			/* Optional <from in to-address */
	char *tomsgid ;		/* Optional $msgid in to-address */
	struct mhdr *firsthdr;	/* message header struct */
	int msgs;       		/* nr messages to be read and */
	int state ;				/* mailbox state */
#define MBX_FREE  0		/* added */
#define MBX_CMD	1		/* in command mode */
#define MBX_SUBJ	2		/* waiting for a subject line */
#define MBX_DATA	3		/* collecting the message */
#define MBX_SEND	4		/* sending a file */
#define MBX_DISC	5		/* in abort from a disconnect */
#define MBX_ABORT 6		/* a user abort of download */
	char line[MBXLINE+1] ;	/* Room for null at end */
	char *lp ;				/* line pointer */
	int mbnum ;				/* which mailbox session is this? */
	int sid ;				/* Characteristics indicated by the SID */
								/* banner of the attaching station.  If */
								/* no SID was sent, this is zero.  If an */
								/* SID of any kind was received, it is */
								/* assumed that the station supports */
								/* abbreviated mail forwarding mode. */
#define	MBX_SID		0x01	/* Got any SID */
#define	MBX_SID_RLI	0x02	/* This is an RLI BBS, disconnect after F> */
								/* Space here for others, currently not of */
								/* interest to us. */
	char stype ;			/* BBS send command type (B,P,T, etc.) */
	struct timer mboxwait;		/* idle timer initially 5 minutes */
	struct timer prompter;	/* need a way to send prompt after file send */
	int promptwait;		/* check time used during file download */
} ;
#define 	NULLMBX	(struct mbx *)0

extern struct mbx *mbox[NUMMBX] ;
/* some things for reading mail selectively */
#define NULLMSGHDR (struct mhdr *)0
extern int ax25mbox ;
