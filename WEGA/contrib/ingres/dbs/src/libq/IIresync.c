# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/pipes.h"
# include	"IIglobals.h"

extern	exit();
int	(*IIinterrupt)() =	exit;

/*
**  RESYNCHRONIZE PIPES AFTER AN INTERRUPT
**
**	The pipes are all cleared out.  This routines must be called
**	by all processes in the system simultaneously.  It should be
**	called from the interrupt catching routine.
*/
IIresync()
{
	register struct pipfrmt	*pbuf;
	struct pipfrmt		buf;

	signal(2, (int (*)()) 1);
	signal(3, (int (*)()) 1);
	pbuf = &buf;
	IIwrpipe(P_PRIME, pbuf);
	/* write the sync block out */
	IIwrpipe(P_INT, pbuf, IIw_down);

	/* read the sync block back on both pipes */
	/* must be in right order */
	IIrdpipe(P_INT, pbuf, IIr_front);
	IIrdpipe(P_INT, pbuf, IIr_down);

	/* flush remaining old data from buffer */
	IIrdpipe(P_PRIME, &IIeinpipe);

	/* Get out of a retrieve and clear errors */
	IIin_retrieve = 0;
	IIerrflag = 0;
	IIndoms = IIdoms = 0;

	/* reset the signal */
	signal(2, IIresync);
	signal(3, IIresync);
	/* allow the user to service the interrupt */
	(*IIinterrupt)(-1);

	/*
	** If IIinterupt returns the user might hang in a retrieve
	*/
	IIsyserr("RESYNC: Interupt returned");
}


# ifdef P8000
# include	<setret.h>
extern ret_buf		IIjmp_buf;
# define	setexit()	setret(IIjmp_buf)
# define	reset()		longret(IIjmp_buf, 1)
# else
# include	<setjmp.h>
extern jmp_buf		IIjmp_buf;
# define	setexit()	setjmp(IIjmp_buf)
# define	reset()		longjmp(IIjmp_buf, 1)
# endif

int		IIreset()	{ reset(); }

int		IIsetexit()	{ setexit(); }
