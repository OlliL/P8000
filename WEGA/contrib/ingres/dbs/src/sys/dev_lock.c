/************************************************************************/
/*									*/
/*		DBS LOCK DEVICE DRIVER					*/
/*		*************************				*/
/*		Release 6/3.4    31/10/89				*/
/*									*/
/************************************************************************/

# include	"../conf/gen.h"
# include	<sys/param.h>
# include	<sys/dir.h>
# ifdef P8000
# include	<sys/s.out.h>
# include	<sys/mmu.h>
# include	<sys/state.h>
# endif
# ifdef PDP
# define	KERNEL
# endif
# ifdef VAX
# define	KERNEL
# include	<sys/uio.h>
# endif
# ifdef EC1834
# include	<sys/a.out.h>
# endif
# include	<sys/user.h>
# include	<sys/proc.h>
# include	"../h/lock.h"

# define	DL_PRI			1
# define	ROOT_UID		0	/* user root		*/

# ifdef VAX
# define	uCOUNT			uio->uio_resid
# else
# define	uCOUNT			u.u_count
# endif
# define	uPID			u.u_procp->p_pid
# define	uRUID			u.u_ruid

# define	U_ERROR(error)		u.u_error = error
# define	OK			U_ERROR(0)
# define	NO_DEV			U_ERROR(19)
# define	CANT_LOCK		U_ERROR(-1)
# define	LOCK_NOT_SET		U_ERROR(-2)
# define	ILL_REQUEST		U_ERROR(-3)
# define	DRIVER_LOCKED		U_ERROR(-4)


# ifdef EC1834
/*									*/
/*	EC 1834 DEPENDENT PART OF LOCK DEVICE DRIVER			*/
/*									*/
# endif


# ifdef PDP
/*									*/
/*	PDP 11 DEPENDENT PART OF LOCK DEVICE DRIVER			*/
/*									*/
# endif


# ifdef VAX
/*									*/
/*	VAX 11 DEPENDENT PART OF LOCK DEVICE DRIVER			*/
/*									*/
# endif


# ifdef P8000
/*									*/
/*	P8000 DEPENDENT PART OF LOCK DEVICE DRIVER			*/
/*									*/

# define	dl_abort		u1abort
# define	dl_busy			u1busy
# define	dl_db_locked		u1db_locked
# define	dl_dev_pid		u1dev_pid
# define	dl_open			u1open
# define	dl_pending		u1pending
# define	dl_read			u1read
# define	dl_request		u1request
# define	dl_rm_all_locks		u1rm_all_locks
# define	dl_rm_one_lock		u1rm_one_lock
# define	dl_server		u1server
# define	dl_sleep		u1sleep
# define	dl_slot			u1slot
# define	dl_tab			u1tab
# define	dl_write		u1write
# define	spl0			evi
# define	spl7			dvi

/************************************************************************/
/***								      ***/
/***		DUMMY DEVICE DRIVER FOR USER DEVICE 1		      ***/
/***								      ***/
/************************************************************************/
/***								      ***/
/*********		INTERRUPT ROUTINE			*********/
/***								      ***/
/**/	usr1int(s) 						      /**/
/**/	register struct state	*s;				      /**/
/**/	{							      /**/
/**/		prstate(s);					      /**/
/**/		panic("DBS_LOCK: unexpected interrupt");	      /**/
/**/		spl0();					      /**/
/**/	}							      /**/
/***								      ***/
/*********		BLOCK I/O DEVICE			*********/
/***								      ***/
/**/	usr1open()	{ NO_DEV; }		 		      /**/
/***								      ***/
/**/	usr1close()	{ NO_DEV; }		 		      /**/
/***								      ***/
/**/	usr1strategy()	{ NO_DEV; }		 		      /**/
/***								      ***/
/*********		CHARACTER I/O DEVICE			*********/
/***								      ***/
/**/	u1ioctl()	{ }			 		      /**/
/***								      ***/
/**/	u1close()	{ }			 		      /**/
/***								      ***/
/************************************************************************/

# endif


/*									*/
/*	MACHINE INDEPENDENT PART OF LOCK DEVICE DRIVER			*/
/*									*/
struct dl_tab
{
	long		db;
	unsigned short	process;
	char		excl;
	char		waiting;
};
struct dl_tab	dl_tab[LOCKS]	= { 0 };
LOCKREQ		dl_server	= { 0 };
unsigned short	dl_dev_pid	=   0  ;
short		dl_busy		=  -1  ;
short		dl_pending	=   0  ;


dl_sleep(wchan)
register char		*wchan;
{
	spl0();
	sleep(wchan, DL_PRI);
	if (!dl_dev_pid)
	{
		DRIVER_LOCKED;
		return (1);
	}
	spl7();
	return (0);
}


dl_open()
{
	if (uRUID == ROOT_UID && !dl_dev_pid)
	{
		dl_busy = dl_pending = 0;
		dl_dev_pid = uPID;
	}
}


dl_abort(lpid)
register long		lpid;
{
	register int			pid;
	register struct dl_tab		*dt;
	register int			ruid;
	extern struct dl_tab		*dl_slot();

	pid = lpid;
	ruid = uRUID;
	if ((ruid != DBS_UID) && (ruid != ROOT_UID))
		goto ret;
	if (pid)
	{
		if (dt = dl_slot(pid, (struct dl_tab *) 0))
		{
			dt->waiting |= M_ABT;
			dl_rm_all_locks(dt);
		}
		goto ret;
	}
	dl_dev_pid = dl_pending = 0;
	dl_busy = -1;
	wakeup(&dl_server);
	wakeup(&dl_pending);
	for (dt = &dl_tab[LOCKS - 1]; dt >= dl_tab; --dt)
	{
		dl_rm_one_lock(dt);
		wakeup(dt);
	}
ret:
	spl0();
}


# ifdef VAX
dl_read(dev, uio)
dev_t		dev;
struct uio		*uio;
{
# else
dl_read()
{
	register char			*server;
# endif
	register int			i;

	i = uCOUNT;
	if (i != DBS_RSIZE)
	{
		ILL_REQUEST;
		return;
	}
	spl7();
	if (!dl_busy)
		dl_sleep(&dl_server);
	if (dl_busy <= 0)
	{
		spl0();
		return;
	}
# ifdef VAX
	dl_rw(uio, (caddr_t) &dl_server, UIO_READ);
# else
	for (server = (char *) &dl_server; i; --i)
		passc(*server++);
# endif
	dl_busy = 0;
	if (dl_pending)
		wakeup(&dl_pending);
	spl0();
}


dl_request(req)
register char		*req;
{
	register char			*p;
	register int			i;

	dl_pending++;
	while (dl_busy)
		if (dl_sleep(&dl_pending))
		{
			/* DRIVER LOCKED */
			spl0();
			return (1);
		}
	for (p = (char *) &dl_server, i = sizeof (LOCKREQ); i; --i)
		*p++ = *req++;
	--dl_pending;
	dl_busy++;
	wakeup(&dl_server);
	return (0);
}


# ifdef VAX
dl_write(dev, uio)
dev_t		dev;
struct uio	*uio;
# else
dl_write()
# endif
{
	register int			i;
	register LOCKREQ		*server;
	register struct dl_tab		*dt;
	register struct dl_tab		*table;
# ifndef VAX
	register char			*p;
# endif
	register unsigned short		act;
	LOCKREQ				req_buf;
	LOCKRES				res_buf;
	extern struct dl_tab		*dl_slot();
	extern struct dl_tab		*dl_db_locked();

	i = uCOUNT;
	act = uPID;
	if (i == DBS_SSIZE && dl_dev_pid == act)
	{
		/* SERVER */
		spl7();
		OK;
# ifdef VAX
		dl_rw(uio, (caddr_t) &res_buf, UIO_WRITE);
# else
		for (p = (char *) &res_buf; i; --i)
			*p++ = cpass();
# endif
		if (dt = dl_slot(res_buf.s_pid, (struct dl_tab *) 0))
		{
			if (res_buf.s_error)
				dt->waiting |= M_ERROR;
			wakeup(dt);
		}
		spl0();
		return;
	}

	/* DBS PROCESS */
	if (i != DBS_WSIZE)
	{
		ILL_REQUEST;
		return;
	}
	spl7();
	server = &req_buf;
# ifdef VAX
	dl_rw(uio, (caddr_t) server, UIO_WRITE);
# else
	for (p = (char *) server; i; --i)
		*p++ = cpass();
# endif
	if (server->l_act == A_ABT)
	{
		dl_abort(server->l_db);
		return;
	}
	if (server->l_act == A_INFO)
	{
		dl_request(server);
		return;
	}
	server->l_pid = act;
	if (!dl_dev_pid || !(dt = dl_slot(act, server)))
	{
		DRIVER_LOCKED;
		goto done;
	}
	if (server->l_mode & M_REL)
	{
		if (dt->excl != M_EXCL)
		{
			if (dl_request(server) || dl_sleep(dt))
				return;
			if (dt->waiting & M_ERROR)
			{
				dt->waiting &= ~M_ERROR;
				CANT_LOCK;
			}
			if (dt->waiting & M_ABT)
			{
				dt->waiting &= ~M_ABT;
				DRIVER_LOCKED;
			}
		}
		goto done;
	}

	/* special handling of data base locks */
	act = server->l_act;
	i = server->l_mode & M_MODE;
	if (act == A_RLS1)
	{
		/* remove data base lock */
		if (dt->excl)
			goto unlock_db;
		LOCK_NOT_SET;
	}
	else if (act == A_RLSA)
		dl_rm_all_locks(dt);
	else
	{
		while (table = dl_db_locked(dt, i == M_EXCL))
		{
			if (act == A_RTN)
			{
				CANT_LOCK;
unlock_db:
				dl_rm_one_lock(dt);
				goto done;
			}
			else
			{
				table->waiting |= M_ON;
				if (dl_sleep(table))
					return;
			}
		}
		dt->excl = i;
	}
done:
	if (!dt->excl)
		dl_rm_one_lock(dt);
	spl0();
}


dl_rm_all_locks(dt)
register struct dl_tab	*dt;
{
	register LOCKREQ		*server;
	LOCKREQ				req_buf;

	if (!dl_dev_pid)
	{
		DRIVER_LOCKED;
		return;
	}
	if (dt->excl != M_EXCL)
	{
		spl7();
		(server = &req_buf)->l_act = A_RLSA;
		server->l_pid = dt->process;
		if (dl_request(server))
			return;
		spl0();
	}
	dl_rm_one_lock(dt);
}


dl_rm_one_lock(dt)
register struct dl_tab	*dt;
{
	dt->db = 0L;
	dt->process = 0;
	dt->excl = 0;
	if (dt->waiting & M_ON)
	{
		wakeup(dt);
		dt->waiting = 0;
	}
	if (!dl_dev_pid)
		DRIVER_LOCKED;
}


struct dl_tab	*dl_slot(pid, enter)
register unsigned short	pid;
register LOCKREQ	*enter;
{
	register struct dl_tab		*dt;

	for (dt = &dl_tab[LOCKS - 1]; dt >= dl_tab; --dt)
		if (dt->process == pid)
			goto done;
	if (enter)
	{
		for (dt = &dl_tab[LOCKS - 1]; dt >= dl_tab; --dt)
			if (!dt->process)
			{
				dt->db = enter->l_db;
				dt->process = pid;
				dt->excl = 0;
				dt->waiting = 0;
				break;
			}
	}
done:
	if (dt < dl_tab)
		dt = (struct dl_tab *) 0;
	return (dt);
}


struct dl_tab	*dl_db_locked(dt, excl_req)
register struct dl_tab	*dt;
register int		excl_req;
{
	register struct dl_tab		*t;

	for (t = &dl_tab[LOCKS - 1]; t >= dl_tab; --t)
	{
		if (!t->process || !t->excl || t->db != dt->db)
			continue;
		if (t->excl == M_EXCL || excl_req)
			return (t);
	}
	return ((struct dl_tab *) 0);
}


# ifdef VAX
dl_rw(uio, addr, rw)
register struct uio	*uio;
register int		addr;
enum uio_rw		rw;
{
	register struct iovec		*iov;
	register int			error;
	register unsigned int		len;

	error = 0;
	uio->uio_offset = addr;
	while (uio->uio_resid > 0)
	{
		if (!(iov = uio->uio_iov)->iov_len)
		{
			uio->uio_iov++;
			if (--(uio->uio_iovcnt) < 0)
				panic("dlrw");
			continue;
		}
		len = iov->iov_len;
		if (error = uiomove((caddr_t) uio->uio_offset, (int) len, rw, uio))
			break;
		iov->iov_base += len;
		iov->iov_len -= len;
		uio->uio_offset += len;
		uio->uio_resid -= len;
	}
	if (error)
		U_ERROR(error);
}
# endif
