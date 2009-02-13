/* TCP implementation. Follows RFC 793 as closely as possible */

#define TIMEZOMBIE		/* An aging report */
#define	DEF_WND	2048	/* Default receiver window */
#define	NTCB	19	/* # TCB hash table headers */
#define	DEF_MSS	512	/* Default maximum segment size */
#define	DEF_RTT	10000	/* Initial guess at round trip time (10 sec) */
#define	MSL2	30	/* Guess at two maximum-segment lifetimes */
/* Round trip timing parameters */
#define	AGAIN	8	/* Average RTT gain = 1/8 */
#define	DGAIN	4	/* Mean deviation gain = 1/4 */

/* TCP segment header -- internal representation
 * Note that this structure is NOT the actual header as it appears on the
 * network (in particular, the offset and checksum fields are missing).
 * All that knowledge is in the functions ntohtcp() and htontcp() in tcpsubr.c
 */
struct tcp {
	int16 source;	/* Source port */
	int16 dest;	/* Destination port */
	int32 seq;	/* Sequence number */
	int32 ack;	/* Acknowledgment number */
	char flags;	/* Flags, data offset */
#define	URG	0x20	/* URGent flag */
#define	ACK	0x10	/* ACKnowledgment flag */
#define	PSH	0x08	/* PuSH flag */
#define	RST	0x04	/* ReSeT flag */
#define	SYN	0x02	/* SYNchronize flag */
#define	FIN	0x01	/* FINal flag */
	int16 wnd;	/* Receiver flow control window */
	int16 up;	/* Urgent pointer */
	int16 mss;	/* Optional max seg size */
};
/* TCP options */
#define	EOL_KIND	0
#define	NOOP_KIND	1
#define	MSS_KIND	2

#define	TCPLEN		20
#define	MSS_LENGTH	4
/* Resequencing queue entry */
struct reseq {
	struct reseq *next;	/* Linked-list pointer */
	char tos;		/* Type of service */
	struct tcp seg;		/* TCP header */
	struct mbuf *bp;	/* data */
	int16 length;		/* data length */
};
#define	NULLRESEQ	(struct reseq *)0

/* TCP connection control block */
struct tcb {
	struct tcb *prev;	/* Linked list pointers for hash table */
	struct tcb *next;

	struct connection conn;

	char state;	/* Connection state */
#define	CLOSED		0	/* Must be 0 */
#define	LISTEN		1
#define	SYN_SENT	2
#define	SYN_RECEIVED	3
#define	ESTABLISHED	4
#define	FINWAIT1	5
#define	FINWAIT2	6
#define	CLOSE_WAIT	7
#define	CLOSING		8
#define	LAST_ACK	9
#define	TIME_WAIT	10

	char reason;		/* Reason for closing */
#define	NORMAL		0	/* Normal close */
#define	RESET		1	/* Reset by other end */
#define	TIMEOUT		2	/* Excessive retransmissions */
#define	NETWORK		3	/* Network problem (ICMP message) */

/* If reason == NETWORK, the ICMP type and code values are stored here */
	char type;
	char code;

	/* Send sequence variables */
	struct {
		int32 una;	/* First unacknowledged sequence number */
		int32 nxt;	/* Next sequence num to be sent for the first time */
		int32 ptr;	/* Working transmission pointer */
		int16 wnd;	/* Other end's offered receive window */
		int16 up;	/* Send urgent pointer */
		int32 wl1;	/* Sequence number used for last window update */
		int32 wl2;	/* Ack number used for last window update */
	} snd;
	int32 iss;		/* Initial send sequence number */
	int16 cwind;		/* Congestion window */
	int16 ssthresh;		/* Slow-start threshold */
	int32 resent;		/* Count of bytes retransmitted */

	/* Receive sequence variables */
	struct {
		int32 nxt;	/* Incoming sequence number expected next */
		int16 wnd;	/* Our offered receive window */
		int16 up;	/* Receive urgent pointer */
	} rcv;
	int32 irs;		/* Initial receive sequence number */
	int16 mss;		/* Maximum segment size */
	int32 rerecv;		/* Count of duplicate bytes received */

	int16 window;		/* Receiver window and send queue limit */

	char backoff;		/* Backoff interval */
	void (*r_upcall)();	/* Call when "significant" amount of data arrives */
	void (*t_upcall)();	/* Call when ok to send more data */
	void (*s_upcall)();	/* Call when connection state changes */
	char flags;		/* Control flags */
#define	FORCE	1		/* We owe the other end an ACK or window update */
#define	CLONE	2		/* Server-type TCB, cloned on incoming SYN */
#define	RETRAN	4		/* A retransmission has occurred */
#define	ACTIVE	8		/* TCB created with an active open */
#define	SYNACK	16		/* Our SYN has been acked */
	char tos;		/* Type of service (for IP) */

	struct mbuf *rcvq;	/* Receive queue */
	int16 rcvcnt;

	struct mbuf *sndq;	/* Send queue */
	int16 sndcnt;		/* Number of unacknowledged sequence numbers on
				 * send queue. NB: includes SYN and FIN, which don't
				 * actually appear on sndq!
				 */


	struct reseq *reseq;	/* Out-of-order segment queue */
	struct timer timer;	/* Retransmission timer */
	struct timer rtt_timer;	/* Round trip timer */
	int32 rttseq;		/* Sequence number being timed */
	int32 srtt;		/* Smoothed round trip time, milliseconds */
	int32 mdev;		/* Mean deviation, milliseconds */
#ifdef TIMEZOMBIE
	int32 lastheard;
#endif
	char *user;		/* User parameter (e.g., for mapping to an
				 * application control block
				 */
};
#define	NULLTCB	(struct tcb *)0
/* TCP statistics counters */
struct tcp_stat {
	int16 runt;		/* Smaller than minimum size */
	int16 checksum;		/* TCP header checksum errors */
	int16 conout;		/* Outgoing connection attempts */
	int16 conin;		/* Incoming connection attempts */
	int16 resets;		/* Resets generated */
	int16 bdcsts;		/* Bogus broadcast packets */
};
extern struct tcp_stat tcp_stat;

extern struct tcb *tcbs[];
extern int32 iss();
struct tcb *lookup_tcb();
struct tcb *create_tcb();
void rehash_tcb(),tcp_output(),tcp_input(),close_self(),dump_seg(),
	setstate();
struct mbuf *htontcp();

/* TCP primitives */
struct tcb *open_tcp();
int send_tcp(),recv_tcp(),close_tcp(),del_tcp();
void state_tcp(),tcp_dump();

extern int16 tcp_mss;
extern int16 tcp_window;
extern int32 tcp_irtt;
