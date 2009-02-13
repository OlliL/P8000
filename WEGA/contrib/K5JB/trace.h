/* Definitions for packet dumping */

/* List of tracing functions for each interface. Entries are placed in
 * this table by the driver at attach time
 */

extern void (*tracef[])();

#define	TRACE_AX25	0
#define	TRACE_ETHER	1
#define	TRACE_IP	2
#define	NTRACE		3

#define	TRACE_SENT	0
#define	TRACE_RECV	1
#define	TRACE_LOOP	2
