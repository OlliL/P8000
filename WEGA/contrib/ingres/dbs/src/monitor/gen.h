# include	"../conf/gen.h"

/*						*/
/*	Login, Logout				*/
/*	and monitor prompt symbole		*/
/*						*/

extern char	LOGIN[];
extern char	LOG_1[];
extern char	LOG_21[];
extern char	LOG_22[];
extern char	LOG_3[];
extern char	PROMPT[];
extern char	INPUT[];
extern char	EMPTY[];
extern char	FULL[];
extern char	EXECUTING[];
extern char	ERROR[];


/*						*/
/*	Character defining monitor commands.	*/
/*	Normally:	'\\'.			*/
/*						*/

# define	ESCAPE			'\\'


/*						*/
/*	Tunable pathname standards		*/
/*	and file name extensions		*/
/*						*/

extern char	BIN_ED[];
extern char	BIN_SH[];
extern char	QRYBUF[];
extern char	QRYTBUF[];
extern char	DAYFILE[];
# define	FILENAME		100
# define	QRYNAME			30


/*						*/
/*	Deepness for nested \include		*/
/*						*/

# define	INC_DEPTH		5


/*						*/
/*	Various traps and macros		*/
/*						*/

# ifdef	MACRO

extern char	BEGINTRAP[];
extern char	CATCHERROR[];
extern char	CONTINUETRAP[];
extern char	DATABASE[];
extern char	DEFAULT[];
extern char	EDITOR[];
extern char	ENDTRAP[];
extern char	ERRORCOUNT[];
extern char	EXECTRAP[];
extern char	PATHNAME[];
extern char	QUERYTRAP[];
extern char	READCOUNT[];
extern char	SHELL[];
extern char	TUPLECOUNT[];
extern char	USERCODE[];

# endif
