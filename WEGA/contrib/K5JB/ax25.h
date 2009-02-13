/* AX.25 datagram (address) sub-layer definitions */

/* Maximum number of digipeaters */
#define	MAXDIGIS	8	/* 8 (N5OWK) digipeaters plus src/dest */
#define	ALEN		6	/* Number of chars in callsign field */
#define	AXALEN		7	/* Total AX.25 address length, including SSID */

/* Internal representation of an AX.25 address */
struct ax25_addr {
	char call[ALEN];
	char ssid;
#define	SSID		0x1e	/* Sub station ID */
#define	REPEATED	0x80	/* Has-been-repeated bit in repeater field */
#define	E		0x01	/* Address extension bit */
#define	C		0x80	/* Command/response designation */
};
#define	NULLAXADDR	(struct ax25_addr *)0
/* Our AX.25 address */
extern struct ax25_addr mycall;

/* AX.25 broadcast address: "QST   -0" in shifted ASCII */
extern struct ax25_addr ax25_bdcst;

/* Internal representation of an AX.25 header */
struct ax25 {
	struct ax25_addr dest;			/* Destination address */
	struct ax25_addr source;		/* Source address */
	struct ax25_addr digis[MAXDIGIS];	/* Digi string */
	int ndigis;				/* Number of digipeaters */
	int cmdrsp;				/* Command/response */
};

/* C-bit stuff */
#define	UNKNOWN		0
#define	COMMAND		1
#define	RESPONSE	2
#define	C_NOPOLL	3

#define	PID_IP		0xcc	/* ARPA Internet Protocol */
#define	PID_ARP		0xcd	/* ARPA Address Resolution Protocol */
#define	PID_NETROM	0xcf	/* NET/ROM */
#define	PID_NO_L3	0xf0	/* No level 3 protocol */
#define	PID_ROSE1	0x01	/* Rose Switch X.25 Protocol (N5OWK) */
#define PID_SEGMENT     0x08    /* Segmentation fragment */

#define SEG_FIRST       0x80    /* First segment of a sequence */
#define SEG_REM         0x7f    /* Mask for # segments remaining */


/* from ax25.c */

extern struct interface *interface;	/* K5JB */

#ifndef UNIX
int ax_send __ARGS((struct mbuf *bp,struct interface *interface,
	int32 gateway,char precedence,char delay,char throughput,
	char reliability));
int ax_output __ARGS((struct interface *interface,char *dest,char *source,
	char pid,struct mbuf *data));
#endif
