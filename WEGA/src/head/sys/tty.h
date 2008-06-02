/* tty.h */

/*
 * A clist structure is the head of a linked list queue of characters.
 * The routines getc* and putc* manipulate these structures.
 */

struct clist 
{
	int		c_cc;		/* character count 		*/
	struct cblock 	*c_cf;		/* pointer to first 		*/
	struct cblock 	*c_cl;		/* pointer to last 		*/
};

/*
 * A tty structure is needed for each WEGA character device that
 * is used for normal terminal IO.
 */
# define NCC	8

struct tty 
{
	struct clist 	t_rawq;		/* raw input queue		*/
	struct clist 	t_canq;		/* canonical queue		*/
	struct clist 	t_outq;		/* output queue			*/
	struct cblock 	*t_buf;		/* buffer pointer		*/
	int		(* t_proc)();	/* routine for device functions */
	ushort		t_iflag;	/* input modes			*/
	ushort		t_oflag;	/* output modes			*/
	ushort		t_cflag;	/* control modes		*/
	ushort		t_lflag;	/* line discipline modes	*/
	unsigned long	t_state;	/* internal state		*/
	short		t_pgrp;		/* process group name		*/
	char		t_line;		/* line discipline		*/
	char		t_delct;	/* delimiter count		*/
	char		t_col;		/* current column		*/
	char		t_row;		/* current row			*/
	unsigned char	t_cc[NCC];	/* settable cntrol chars	*/
};

/*
 * The structure of a clist block
 */
# define CLSIZE	24			/* number of chars that a cblock*/
					/* can hold			*/
struct cblock 
{
	struct cblock 	*c_next;	/* ptr to next			*/
	char		c_first;	/* array offset into c_data	*/
	char		c_last;		/* array offset into c_data	*/
	char		c_data[CLSIZE];	/* the actual data		*/
};

extern struct cblock 	cfree[];
extern struct cblock 	*getcb();
extern struct cblock 	*getcf();
extern struct clist 	ttnulq;

/*
 * structure of the head of the freelist
 */
struct chead 
{
	struct cblock	*c_next;	/* ptr to next cblock		*/
	int		c_size;		/* initialized to CLSIZE	*/
};

extern struct chead 	cfreelist;

struct inter 
{
	int	cnt;
};

/*
 * control characters offsets these are the offsets into the ttcchar array to 
 * find the appropriate values
 */
# define VINTR		0		/* interrupt char		*/	
# define VQUIT		1		/* quit char			*/
# define VERASE		2		/* erase char			*/
# define VKILL		3		/* kill char			*/
# define VEOF		4		/* end-of-file char		*/
# define VMIN		4		/* minimum chars to get into 	*/
					/* rawq before waking up anyone	*/
# define VEOL		5		/* end-of-line char		*/
# define VTIME		5		/* ticks to timeout on rawq	*/

/*
 * default control chars
 */
# define CINTR		0x7F		/* DEL				*/
# define CQUIT		0x1C		/* FS, cntl \			*/
# define CERASE		0x08
# define CKILL		0x18
# define CEOF		0x04		/* cntl d			*/
# define CSTART		0x11		/* cntl q			*/
# define CSTOP		0x13		/* cntl s			*/
	
# define DELAYDEL	0x80 		/* delay delimiter char		*/
	
# define TTIPRI		28
# define TTOPRI		29

/*
 * limits
 */
extern int 	ttlowat[], tthiwat[];
# define TTYHOG		256		/* if TTYHOG chars in rawq,	*/
					/* through the remaining input	*/
					/* away				*/
# define TTXOLO		60		/* if input is blocked, and less*/
					/* than TTXOLO chars in the rawq*/
					/* restart input		*/
# define TTXOHI		180		/* if # of chars in rawq gets	*/
					/* above this, block input	*/

/*
 * input modes
 */
# define IGNBRK		0x0001		/* ignore break condition	*/
# define BRKINT		0x0002		/* interrupt on break		*/
# define IGNPAR		0x0004		/* ignore parity		*/
# define PARMRK		0x0008		/* mark parity			*/
# define INPCK		0x0010		/* input parity checking	*/
# define ISTRIP		0x0020		/* strip off parity		*/
# define INLCR		0x0040		/* map <NL> to <CR>		*/
# define IGNCR		0x0080		/* ignore <CR>			*/
# define ICRNL		0x0100		/* map <CR> to <NL>		*/
# define IUCLC		0x0200		/* map upper case to lower case	*/
# define IXON		0x0400		/* x-on/x-off on output enable	*/
# define IXANY		0x0800		/* any char restarts after x-off*/
# define IXOFF		0x1000		/* x-on/x-off on input enable	*/


/*
 * output modes
 */
# define OPOST		0x0001		/* post process output		*/
# define OLCUC		0x0002		/* map lower case to upper case	*/
# define ONLCR		0x0004		/* map <NL> to <CR><NL>		*/
# define OCRNL		0x0008		/* map <CR> to <NL>		*/
# define ONOCR		0x0010		/* don't output a <CR>		*/
# define ONLRET		0x0020		/* use <CR> dlays not <NL> dlays*/
# define OFILL		0x0040		/* for delay send nulls		*/
# define OFDEL		0x0080		/* for delay send <DEL>s	*/
# define NLDLY		0x0100		/* <NL> delay enable		*/
# define NL0		0x0000		/* <NL> no delay		*/
# define NL1		0x0100		/* <NL> delay			*/
# define CRDLY		0x0600		/* <CR> delay enable		*/
# define CR0		0x0000		/* <CR> no delay		*/
# define CR1		0x0200		/* <CR> delay			*/
# define CR2		0x0400		/* <CR> delay			*/
# define CR3		0x0600		/* <CR> delay			*/
# define TABDLY		0x1800		/* <TAB> delay enable		*/
# define TAB0		0x0000		/* <TAB> no delay		*/
# define TAB1		0x0800		/* <TAB> delay			*/
# define TAB2		0x1000		/* <TAB> delay			*/
# define TAB3		0x1800		/* expand <TAB>s to spaces	*/
# define BSDLY		0x2000		/* <BS> delay enable		*/
# define BS0		0x0000		/* <BS> no delay		*/
# define BS1		0x2000		/* <BS> delay			*/
# define VTDLY		0x4000		/* <VT> delay enable		*/
# define VT0		0x0000		/* <VT> no delay		*/
# define VT1		0x4000		/* <VT> delay			*/
# define FFDLY		0x8000		/* <FF> delay enable		*/
# define FF0		0x0000		/* <FF> no delay		*/
# define FF1		0x8000		/* <FF> delay			*/


/*
 * control modes
 */
# define CBAUD		0x000F		/* baud rate			*/
# define B0		0x0000		/* 0 baud -- hang up line	*/
# define B50		0x0001		/* 50 baud			*/
# define B75		0x0002		/* 75 baud			*/
# define B110		0x0003		/* 110 baud			*/
# define B134		0x0004		/* 134.5 baud			*/
# define B150		0x0005		/* 150 baud			*/
# define B200		0x0006		/* 200 baud			*/
# define B300		0x0007		/* 300 baud			*/
# define B600		0x0008		/* 600 baud			*/
# define B1200		0x0009		/* 1200 baud			*/
# define B1800		0x000A		/* 1800 baud			*/
# define B2400		0x000B		/* 2400 baud			*/
# define B4800		0x000C		/* 4800 baud			*/
# define B9600		0x000D		/* 9600 baud			*/
# define EXTA		0x000E		/* external A (19200 baud)	*/
# define EXTB		0x000F		/* external B (19200 baud)	*/
# define CSIZE		0x0030		/* character size		*/
# define CS5		0x0000		/* 5 bits			*/
# define CS6		0x0010		/* 6 bits (or $1.50)		*/
# define CS7		0x0020		/* 7 bits			*/
# define CS8		0x0030		/* 8 bits			*/
# define CSTOPB		0x0040		/* 2 stop bits if set--1 if not	*/
# define CREAD		0x0080		/* enable receiver		*/
# define PARENB		0x0100		/* parity enable		*/
# define PARODD		0x0200		/* odd parity if set--else even	*/
# define HUPCL		0x0400		/* hang up on last close	*/
# define CLOCAL		0x0800		/* direct attach -- else modem	*/


/*
 * line discipline 0 modes
 */
# define ISIG		0x0001		/* enable signals		*/
# define ICANON		0x0002		/* canonicalization (erase/kill)*/
# define XCASE		0x0004		/* upper case only escape enable*/
# define ECHO		0x0008		/* echo enable			*/
# define ECHOE		0x0010		/* echo erase as <BS><SP><BS>	*/
# define ECHOK		0x0020		/* echo <NL> after kill char	*/
# define ECHONL		0x0040		/* echo <NL>			*/
# define NOFLSH		0x0080		/* disable q flush after intr or*/
					/* quit				*/
# define ISPCI		0x2000		/* lindys special		*/
	
# define SSPEED		13		/* default speed: 9600 baud	*/

/*
 * Hardware bits
 */
# define DONE		0x0080
# define IENABLE	0x0040
# define OVERRUN	0x4000
# define FRERROR	0x2000
# define PERROR		0x1000


/*
 * Internal state
 */
# define TIMEOUT	0x00000001L	/* Delay timeout in progress	*/
# define WOPEN		0x00000002L	/* Waiting for open to complete */
# define ISOPEN		0x00000004L	/* Device is open		*/
# define TBLOCK		0x00000008L	/* blocked for input		*/
# define CARR_ON	0x00000010L	/* Software copy of carrier	*/
# define BUSY		0x00000020L	/* Output in progress		*/
# define OASLP		0x00000040L	/* Wakeup when output done	*/
# define IASLP		0x00000080L	/* Wakeup when input done	*/
# define TTSTOP		0x00000100L	/* Output stopped by ctl-s	*/
# define OEXTPROC	0x00000200L	/* Output external processing	*/
# define TACT		0x00000400L	/* timeout waiting on rawq	*/
# define ESC		0x00000800L	/* Last char was escape		*/
# define RTO		0x00001000L	/* timed out waiting on rawq	*/
# define TTIOW		0x00002000L	/* waiting for output q to empty*/
# define TTXON		0x00004000L	/* transmit an x-on		*/
# define TTXOFF		0x00008000L	/* transmit an x-off		*/
# define CLR_TS		0x00010000L	/* Clear to send		*/
# define XCLUSE		0x00020000L	/* Exclusive use mode		*/
# define IEXTPROC	0x00040000L	/* Input Externel Processing	*/
# define WCLOSE		0x00080000L	/* wait for close 		*/
# define LP		0x80000000L	/* lp driver aktiv		*/
	

/*
 * l_output status
 */
# define CPRES		1


/*
 * device commands
 */
# define T_OUTPUT	0		/* do output			      */
# define T_TIME		1		/* reset timeout  stop xmit of break  */
# define T_SUSPEND	2		/* stop all output		      */
# define T_RESUME	3		/* resume out (after T_SUSPEND)	      */
# define T_BLOCK	4		/* block input			      */
# define T_UNBLOCK	5		/* unblock input		      */
# define T_RFLUSH	6		/* flush in q (dev dependent stuff)   */
# define T_WFLUSH	7		/* flush out q (dev dependent stuff)  */
# define T_BREAK	8		/* send break			      */
# define T_XOUTPUT	9		/* do output from isr		      */


/*
 * Ioctl control packet
 */
struct termio 
{
	ushort		c_iflag;	/* input modes 			*/
	ushort		c_oflag;	/* output modes 		*/
	ushort		c_cflag;	/* control modes 		*/
	ushort		c_lflag;	/* line discipline modes 	*/
	char		c_line;		/* line discipline 		*/
	unsigned char	c_tcc[NCC];	/* control chars 		*/
};

/*
 *	This is the structure of the arguments to the lpr ioctl program
 *      called lp1sgtty() or lp2sgtty()
 */

struct lparms
{
	int	lines;			/* number of lines per page	*/
	int	cols;			/* number of columns per page	*/
	int	indent;			/* default indentation for      */
					/* line printer	                */
};
