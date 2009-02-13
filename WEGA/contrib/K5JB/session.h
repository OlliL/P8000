extern int mode;
#define	CMD_MODE	1	/* Command mode */
#define	CONV_MODE	2	/* Converse mode */

/* Session control structure; only one entry is used at a time */
struct session {
	int type;
#define	FREE	0
#ifdef _TELNET
#define	TELNET	1
#endif
#define	FTP	2
#define	AX25TNC	3
#ifdef _FINGER		/* K5JB*/
#define FINGER	4
#endif
#define NRSESSION 5

	char *name;	/* Name of remote host */
	union {
		struct ftp *ftp;
#ifdef _TELNET
		struct telnet *telnet;
#endif
#ifdef AX25
		struct ax25_cb *ax25_cb;
#ifdef NETROM
		struct nr4cb *nr4_cb ;
#endif
#endif
#ifdef _FINGER				/*K5JB*/
		struct finger *finger;
#endif
	} cb;
	int (*parse)();		/* Where to hand typed input when conversing */
	FILE *record;		/* Receive record file */
	char *rfile;		/* Record file name */
	FILE *upload;		/* Send file */
	char *ufile;		/* Upload file name */
	char noecho;		/* Echo flag */
};
#define	NULLSESSION	(struct session *)0
extern unsigned nsessions;
extern struct session *sessions;
extern struct session *current;
struct session *newsession();

extern int16 lport;
