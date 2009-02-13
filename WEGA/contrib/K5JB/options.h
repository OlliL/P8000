/* options.h contains more commonly changed options.  config.h contains the
 * rest.  DRSI and MDKISSPORT, if defined here, will ensure AX25 is defined
 * even though it is normally defined in config.h.
 * Note that DRSI, COMBIOS, PACKET, SERIALTEST and the video attributes,
 * except for CUTE_VIDEO, not apply to Unix.  See also unixopt.h for Unix.
 * Incremental sizes apply only to MS-DOS, and are approximate.
*/

#ifndef GOT_OPTH
#define GOT_OPTH

/* These first ones are for MS-DOS only! But, clumsy me, I forget... */
#ifdef MSDOS
#undef	DRSI /* DRSI standard driver, also alter config.inc + 8004 bytes */
/* Note, combios is preliminary and still evolving */
#undef	COMBIOS /* Combios/mbbios/xxxbios interface - adds 1764 bytes */
/* For packet driver, define PACKET.  This packet driver has only be tested
 * with the G8BPQ scheme, and to some extent with Ethernet packet driver.
 * Should work with Baycom.  Adds 2552 bytes */
#define	PACKET	/* with k37, start including these two */
#define	PACKET_POLL /* Use polled receive on pipe packet driver */
#define SERIALTEST /* SERIALTEST only applies to MS-DOS; counts rx overruns */
			/* on serial port.  Costs 178 bytes */

/* In the following, use attribute bit pattern, xbbbifff, where bbb is
 * background, i is intensity, fff is forground.  Generally leave x = 0.
 * Example: 0x1f is bright white on blue, 0x0f is bright wht on blk.
 * These attributes were chosen to be reasonable on mono and most color tubes.
 * Only used if you define CUTE_VIDEO, no harm otherwise...
 */
#define	OUR_NORMAL 0x07
#define	OUR_BOLD 0x1f
#define	OUR_REVERSE 0xf0
#define	OUR_CUTE OUR_BOLD	/* one we will use as our contrasting video */

/* PACKET or COMBIOS implies ASY */
#if defined(PACKET) || defined(COMBIOS)
#undef	ASY
#define	ASY      /* Asynch driver code */
#endif

#endif /* MSDOS */

/* MS-DOS doesn't do the shell escape well, use another DV window.
 * This advice applys to multitasking OSs too.  During shell escapes
 * NET is dead as a doornail.
 */
#define SHELL

#ifdef MSDOS
#undef SHELL
#endif

/* The following are for all operating systems */

#define	MDKISSPORT /* Multidrop KISS, up to 16 pseudo ports, 1016 bytes */
#define	MULPORT /* Grapes Multiport code - adds 1398 bytes */
#define	AX25_HEARD /* Heard list - adds 3716 bytes */
#define	CUTE_VIDEO  /* Contrast video on incoming sessions. Adds 484 Bytes */
#undef	TRACE_TIME	/* Trace timestamping */
#define	SEG_CMD	/* ax25 segment command */
#define	REWRITE	/* rewrite.net file for smtp server */
#define	REWRITECMD	/* rewrite command for testing rewrite.net */
#undef	FORWARD	/* Enable sending received frames to monitor program +432 */

#if defined(DRSI) || defined(MDKISSPORT) || defined(COMBIOS) || defined(PACKET)
#define AX25
#endif

/* The following should be in config.h but I change this file more often and
 * less files are affected by changing options.h.  Main.c and ax25cmd.c were
 * getting to be a mess so I made some macros to eliminate the deeply nested
 * ifdefs.  Edit this and the ATTACHOPTs to suit your configuration. Note
 * that there is a tab on second line.  Put vcipcall here if you use it.
 */

#ifdef AX25_HEARD
#define AXCMDDISP \
"ax25 subcommands: digipeat heard maxframe mboxcall mycall paclen pthresh reset\n\
	retry segment status t1 t2 t3 t4 vers window"
#else
#define AXCMDDISP \
"ax25 subcommands: digipeat maxframe mboxcall mycall paclen pthresh reset retry\n\
	segment status t1 t2 t3 t4 vers window"
#endif

/* Edit this attach display to suit the hardware you have chosen.  Note that
 * the [port] arg only applies with MDKISSPORT defined.  Maximum configuration
 * would have slip|ax25|nrs for the hardware type.
 */

#ifdef UNIX
#define ATTACHOPT \
 "attach asy 0 <ttyname> slip|ax25 <label> 0 <mtu> <speed> [port]"
#else
#define ATTACHOPT \
 "\nattach asy <addr> <vect> slip|ax25 <label> <buffers> <mtu> <speed> [port]"
#endif


/* if you want to make a little custom thing for signon screen, you must
 * have CUTE_VIDEO defined...
 */

/*
This is exactly centered
#define	CUSTOM " Custom version for Bob, W6SWE \n"
*/
#undef	CUSTOM

#endif /* GOT_OPTH */
