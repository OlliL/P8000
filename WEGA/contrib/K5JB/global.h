#if	defined(__TURBOC__) || defined(__STDC__) || defined(LATTICE)
#define	ANSIPROTO	1
#endif

#ifdef COH386	/* K5JB */
#undef NULL
#endif

#ifndef	__ARGS
#ifdef	ANSIPROTO
#define	__ARGS(x)	x
#else
#define	__ARGS(x)	()
#endif
#endif

/* The "interrupt" keyword is non-standard, so make it configurable */
#ifdef	__TURBOC__
#define	INTERRUPT	void interrupt
#else
#define	INTERRUPT	void
#endif

/* Global definitions used by every source file.
 * Some may be compiler dependent.
 */
#define MK_FP(seg,ofs)	((void far *) \
			   (((unsigned long)(seg) << 16) | (unsigned)(ofs)))
/* Reversed pokeb and peekb here in in code to agree with Turbo C - K5JB */
#define pokew(a,b,c)	(*((int  far*)MK_FP((a),(b))) = (int)(c))
#define pokeb(a,b,c)	(*((char far*)MK_FP((a),(b))) = (char)(c))
#define peekw(a,b)	(*((int  far*)MK_FP((a),(b))))
#define peekb(a,b)	(*((char far*)MK_FP((a),(b))))
#define movblock(so,ss,do,ds,c)	movedata(ss,so,ds,do,c)

#define outportw outport
#define inportw inport
#if defined(_OSK) && !defined(_UCC)
#define strchr index
#define strrchr rindex
char *strtok();
#else
/* for later Coherent compiler's benefit */
#ifdef COH386
#undef index
#undef rindex
#endif
#define index strchr
#define rindex strrchr
#endif

/* Indexes into binmode in files.c; hook for compilers that have special
 * open modes for binary files
 */
#define	READ_BINARY	0
#define	WRITE_BINARY	1
#define APPEND_BINARY   2
extern char *binmode[];

/* not all compilers grok defined() */
#ifdef NODEFINED
#define defined(x) (x)
#endif

/* These two lines assume that your compiler's longs are 32 bits and
 * shorts are 16 bits. It is already assumed that chars are 8 bits,
 * but it doesn't matter if they're signed or unsigned.
 */
#ifdef _UCC
#include <types.h>
#else
typedef long int32;		/* 32-bit signed integer */
typedef unsigned short int16;	/* 16-bit unsigned integer */

#endif

#define	uchar(x) ((unsigned char)(x))
#define	MAXINT16 (int16)65535L		/* Largest 16-bit integer */

/* Since not all compilers support structure assignment, the ASSIGN()
 * macro is used. This controls how it's actually implemented.
 */
#ifdef	NOSTRUCTASSIGN	/* Version for old compilers that don't support it */
#define	ASSIGN(a,b)	memcpy((char *)&(a),(char *)&(b),sizeof(b));
#else			/* Version for compilers that do */
#define	ASSIGN(a,b)	((a) = (b))
#endif

/* Define null object pointer in case stdio.h isn't included */
/* Note that Turbo C and Unix Sys V stdio.h define NULL as 0.  Coherent defines
 * NULL as (char *)0.  This code defined NULL as (void *)0.  To be consistent
 * I am voting with Turbo (Borland) and Unix C.  (Note this may spawn warnings
 * that constant is long in large and larger memory models.  See stdio.h.)
 * K5JB
 */
#ifndef	NULL
/* General purpose NULL pointer */
#define NULL 0
#endif
#define	NULLCHAR (char *)0	/* Null character pointer */
#define	NULLFP	 (int (*)())0	/* Null pointer to function returning int */
#define	NULLVFP	 (void (*)())0	/* Null pointer to function returning void */
#define	NULLFILE (FILE *)0	/* Null file pointer */

/* General purpose function macros */
#define	min(x,y)	((x)<(y)?(x):(y))	/* Lesser of two args */
#define	max(x,y)	((x)>(y)?(x):(y))	/* Greater of two args */

/* Convert an address to a LONG value for printing */
#ifdef MSDOS
long	ptr2long();			/* for fuzzy segment addresses */
#else
#define ptr2long(x)	((long) (x))	/* typecast suffices for others */
#endif

/* Extract a short from a long */
#define hiword(x)	((int16)((x) >> 16))
#define	loword(x)	((int16)(x))

/* Extract a byte from a short */
#define	hibyte(x)	(((x) >> 8) & 0xff)
#define	lobyte(x)	((x) & 0xff)

/* Extract nibbles from a byte */
#define	hinibble(x)	(((x) >> 4) & 0xf)
#define	lonibble(x)	((x) & 0xf)

#if	(defined(SYS5) || (defined(ATARI_ST) && defined(LATTICE)))
#define rindex	strrchr
#define index	strchr
#endif

#define CMARK versionf
/* Heavily used functions from the standard library */
char *index(),*rindex(),*malloc(),*calloc(),*tmpnam();

#ifndef	_OSK
char *ctime();
#endif
