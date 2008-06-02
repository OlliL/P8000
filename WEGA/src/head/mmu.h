/* mmu.h */

struct	segd {			/* segment descriptor */
		unsigned sg_base;
		char sg_limit;
		char sg_attr;
};

struct segd ktsegd, kdsegd, kssegd;

#define	RD_AT	0001
#define	SYS_AT	0002
#define	CPUI_AT	0004
#define	EXC_AT	0010
#define	DMAI_AT	0020
#define	DIRW_AT	0040
#define	CHG_AT	0100
#define	REF_AT	0200

#define	RO	RD_AT
#define	RW	0

#define	SSEG	0
#define	SUTSEG	1
#define SUTSG2	2
#define	USEG	63
#define SUT2SEG	65
#define	STSEG	63		/* stack segment - segmented users *//* SEG */

#define	SBREAK	0xFFC9
#define	UBREAK	0xFFD1

#define SSEGW	0x0000		/* SEG */
#define SUTSEGW	0x0100		/* SEG */
#define SUTSGW2 0x0200		/* FPE in segment 1 change */
#define USEGW	0x3F00		/* SEG */

#define STACKSEG 0x3F00		/* SEG */

#define TEXT	0		/* SEG */
#define DATA	1		/* SEG */

#define NOCHANGE -1		/* SEG */
