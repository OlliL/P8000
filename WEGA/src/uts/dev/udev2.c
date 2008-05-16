char udev2wstr[] = "@[$]udev2.c		Rev : 1.1 " ;

/*
 * This file contains all the dummy device driver routines included
 * in the bdevsw and cdevsw tables in conf.c. 
 * Each of these dummy routines is equivalent to nodev() (subr.c).
 */

#include	"../h/param.h"
#include	"../h/sysinfo.h"
#include	"../h/systm.h"
#include	"../h/dir.h"
#include	"../h/state.h"
#include	"../h/mmu.h"
#include	"../h/s.out.h"
#include	"../h/user.h"

usr2int (s) 
register struct state *s;
{
	prstate (s);
	panic ("usr2: unexpected interrupt");
	evi ();
}

/*
 ***  block i/o devices  ***
 */
usr2open () 
{
	u.u_error = ENODEV ;
} 

usr2close ()
{
	u.u_error = ENODEV ;
} 

usr2strategy ()
{
	u.u_error = ENODEV ;
} 

/*
 ***  character i/o devices  ***
 */
u2open () 
{
	u.u_error = ENODEV ;
}

u2close ()
{
	u.u_error = ENODEV ;
}

u2read () 
{
	u.u_error = ENODEV ;
}

u2write () 
{
	u.u_error = ENODEV ;
}

u2ioctl () 
{
	u.u_error = ENODEV ;
}
