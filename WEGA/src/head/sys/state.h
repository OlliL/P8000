/* state.h */

/*
 *  state.h:
 *    constants, etc. dealing with machine status
 */

# define NUMREG	15

struct state 
{
	unsigned	s_reg[NUMREG];
	unsigned	s_sp;
	unsigned	s_eventid;
	unsigned	s_ps;
	unsigned	s_pcseg;
	unsigned	s_pc;
};

# define SEGFCW		0x8000		/* segmented bit in fcw 	   */
# define FCWBITS	0xF8FC		/* used flag and control word bits */
# define TBIT		0x0001		/* trace bit in PS		   */
# define TRPBIT		0x0002		/* trap bit: indicates trap 	   */
# define EBIT		0x0080		/* user error bit in PS: c-bit 	   */
# define EPUBIT		0x0100		/* running epu instruction	   */
# define SYSBIT		0x4000		/* running in system mode	   */
# define NVIBIT		0x0800		/* handling non vectored interrupt */

# define TRPMODE(ps)	((ps)  & TRPBIT)
# define EPUMODE(ps)	((ps)  & EPUBIT)
# define SYSMODE(ps)	((ps)  & SYSBIT)
# define USRMODE(ps)	(((ps) & SYSBIT) == 0)
# define HIPRI(ps)	(((ps) & NVIBIT) == 0)

# define RETRLO		4		/* system call return value (low)  */
# define RETRHI		5		/* system call return value (high) */
# define SCINS		0x7F00		/* system call instruction 	   */


/*
 *  states of floating point emulator:
 *  	FPSTEP - being single stepped
 *  	FPRUN - in floating point emulator (preemption enabled)
 */

# define FPRUN		0x0001
# define FPSTEP		0x0002
