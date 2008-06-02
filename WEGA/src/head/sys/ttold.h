/* ttold.h */

/*
 * This module is contains the old terminal structure and defines for the old 
 * style gtty and stty calls.
 */

struct	sgttyb 
{
	char	sg_ispeed;
	char	sg_ospeed;
	char	sg_erase;
	char	sg_kill;
	int	sg_flags;
};

struct	tchars 
{
	char	t_intrc;
	char	t_quitc;
	char	t_startc;
	char	t_stopc;
	char	t_eofc;
	char	t_brkc;
};

/* 
 * modes
 */
# define O_TANDEM	01
# define O_CBREAK	02
# define O_LCASE	04
# define O_ECHO		010
# define O_CRMOD	020
# define O_RAW		040
# define O_ODDP		0100
# define O_EVENP	0200
# define O_NLDELAY	001400
# define O_TBDELAY	006000
# define O_XTABS	006000
# define O_CRDELAY	030000
# define O_VTDELAY	040000
# define O_BSDELAY	0100000

/* 
 * delays
 */
# define O_NL1		000400
# define O_NL2		001000
# define O_CR1		010000
# define O_CR2		020000
# define O_TAB1		0002000
# define O_TAB2		0004000
