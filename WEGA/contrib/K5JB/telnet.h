#define	LINESIZE	256	/* Length of local editing buffer */


/* Telnet command characters */
#define	IAC		255	/* Interpret as command */
#define	WILL		251
#define	WONT		252
#define	DO		253
#define	DONT		254

/* Telnet options */
#define	TN_TRANSMIT_BINARY	0
#define	TN_ECHO			1
#define	TN_SUPPRESS_GA		3
#define	TN_STATUS		5
#define	TN_TIMING_MARK		6
#define	NOPTIONS		6

/* Telnet protocol control block */
struct telnet {
	struct tcb *tcb;
	char state;

#define	TS_DATA	0	/* Normal data state */
#define	TS_IAC	1	/* Received IAC */
#define	TS_WILL	2	/* Received IAC-WILL */
#define	TS_WONT	3	/* Received IAC-WONT */
#define	TS_DO	4	/* Received IAC-DO */
#define	TS_DONT	5	/* Received IAC-DONT */

	char local[NOPTIONS];	/* Local option settings */
	char remote[NOPTIONS];	/* Remote option settings */
#ifdef	UNIX
	struct	mbuf	*inbuf;		/* ptr to buffer holding excess rcvd chars */
	struct	mbuf	*outbuf;	/* ptr to buffer holding excess to-send chars */
	int	fd;			/* file descriptor of pty */
#endif

	struct session *session;	/* Pointer to session structure */
};
#define	NULLTN	(struct telnet *)0
extern int refuse_echo;
struct telnet *open_telnet();
int send_tel();
