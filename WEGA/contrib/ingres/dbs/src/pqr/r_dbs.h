/*
** ---  R_DBS.H --------- BASIC DECLARATIONS  ---------------------------------
**
**		INTERFACE  between
**			DBS and
**			REPORT (compiler/interpreter)
**
**		All declarations must match the
**		corresponding entries in DBS
**
**	Version: @(#)r_dbs.h		4.0	02/05/89
**
*/

# ifdef SINGLE

# define	MAX_NAME	12		/* h/dbs.h: MAXNAME	*/
# define	MAX_FIELD	255		/* h/aux.h:    MAXFIELD */
# define	INT		30		/* h/dbs.h: INT		*/
# define	FLOAT		31		/* h/dbs.h: FLOAT	*/
# define	CHAR		32		/* h/dbs.h: CHAR	*/

# ifdef MSDOS
# define	SETBUF	
# define	SIGN_EXT
# define	AA_atof		v6_atof
# define	AA_atoi		v6_atoi
# define	AA_atol		v6_atol
# define	AAbequal	bequal
# define	AAbmove		bmove
# define	AAi1toi2(x)	(x)
# define	AAlength	length
# define	AAscompare	scompare
# define	AAsequal	sequal
# define	ctoi(x)		(x)
# define	ctou(x)		((x) & 0377)
# define	sync()
# endif

/*  SETEXIT | RESET							*/
/*	In version 7, setexit is not defined, reset too:		*/
/*	this fakes it.							*/
# ifdef P8000
# include	<setret.h>
extern ret_buf		AAjmp_buf;
# define	setexit()	setret(AAjmp_buf)
# define	reset()		longret(AAjmp_buf, 1)
# else
# include	<setjmp.h>
extern jmp_buf		AAjmp_buf;
# define	setexit()	setjmp(AAjmp_buf)
# define	reset()		longjmp(AAjmp_buf, 1)
# endif

# else

# include	"../conf/conf.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/bs.h"

# define	MAX_NAME	MAXNAME
# define	MAX_FIELD	MAXFIELD
# endif

# define	MAX_TUP		7000
# define	MAX_DOM		300
# define	BIN		33		/* no match		*/

/* IDENTIFICATION STUFF */
extern char	*PQR_system;
