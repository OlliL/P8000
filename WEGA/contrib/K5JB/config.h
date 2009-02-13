/*
 * just about everything except "MSDOS" is defined here, or in options.h.
 * and config.inc.  Makefile only has minimum to make it agree with compiler
 * used.  Some of the code that can be enabled here is incomplete or not
 * guaranteed to work.  Suspect code is marked "CE"  caveat emptor).
 */

#ifndef GOT_CONFIGH
#define GOT_CONFIGH

/* Software options */
#define	SERVERS		/* Include TCP servers */
#define	TRACE		/* Include packet tracing code */
#define	_TELNET		/* Telnet server - don't undef this in Unix */
#ifdef	_TELNET
#define	TN_MOTD		/* a Telnet message of the day for connectee, +50 bytes */
#endif
#define	NSESSIONS 10	/* Number of interactive clients */
#define	TYPE		/* Include type command */
#define	FLOW		/* Enable local tty flow control */
#define	_FINGER		/* Enable Finger - was in makefile - adds 2924 bytes */
#define	NETROM		/* NET/ROM network support - adds 20856 bytes */
#define	SEGMENT		/* NOS type ax25 frame segmentation - adds 556 bytes */
#define	CUTE_FTP	/* a logon assistant in ftpcli, adds 270 bytes */
#define	AX_VERS_DEFAULT V2	/* Alternate: V1. If undefined, will do V1 */

/* edit these as you see fit.  main.c was getting to be a mess */
/* Typical ones shown here for Unix and MS-DOS */

#ifdef UNIX
#define STARTOPT \
 "start: discard, echo, finger, ftp, remote, smtp, telnet, telserv, telunix"
#define STOPOPT \
 "stop: discard, echo, finger, ftp, remote, smtp, telnet, telserv, telunix"
#else
#define STARTOPT \
 "start options: discard, echo, finger, ftp, remote, smtp, telnet"
#define STOPOPT \
 "stop options: discard, echo, finger, ftp, remote, smtp, telnet"
#endif

/* see options.h for commonly changed options:

	DRSI			DRSI standard driver
	MDKISSPORT	Extended KISS driver for up to 16 ports
	MULPORT		Grapes Multiport code
	AX25_HEARD	A "heard" and "jheard" function
	PACKET		Sufficient FTP Corp. packet driver to do G8BPQ, apparently
					Also works with Ethernet packet drivers
*/

/* Hardware configuration */
#define	SLIP		/* Serial line IP */
#define	KISS		/* KISS TNC code, implies AX25 - adds 36,850 bytes */
#undef	ETHER		/* General purpose, only tested with packet driver */

/* untested things */
#undef	PC_EC		/* 3-Com 3C501 Ethernet controller -- CE */
#undef	NRS  		/* NET/ROM async interface -- CE */

#if defined(NRS)
#undef	NETROM
#define	NETROM		/* NRS implies NETROM */
#endif

#if defined(ETHER) && !defined(PACKET) /* ETHER requires packet driver */
#define  PACKET
#endif

/* note that if DRSI, COMBIOS, PACKET, or MDKISSPORT defined in options.h,
 * AX25 will be defined
 */

/* shortened this to include only things I mess with */
#if defined(NETROM) || defined(KISS)
#undef	AX25		/* may have been defined in options.h */
#define	AX25		/* AX.25 subnet code - with KISS adds 32306 bytes */
#undef	AXMBX
#define	AXMBX
#endif

/* undef AXMBX to cut 11,048 bytes and leave ax.25 with only chat session
#undef AXMBX
*/

#ifdef	AXMBX
#define	SID2		/* for ax25 mbox - adds 738 bytes */
#define	MB_BUTTIN	/* A Butt-in function for mailbox, 242 bytes */
#endif

#ifdef AX25
#undef VCIP_SSID	/* add the ax25 vcipcall command - takes 384 bytes. obsolete with k36 */
#define	AX_MOTD		/* an AX.25 or Netrom motd for connectee 84 bytes */
#endif

/* KISS TNC, SLIP, NRS, PACKET or COMBIOS implies ASY */
#if (defined(KISS) || defined(PACKET) || defined(NRS) || defined(SLIP) \
	|| defined(SLFP) || defined(COMBIOS))
#undef	ASY		/* may have been defined in options.h */
#define	ASY		/* Asynch driver code */
#endif
#endif	/* GOT_CONFIGH */
