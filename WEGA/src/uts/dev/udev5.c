char udev5wstr[] = "@[$]udev5.c		Rev : 1.1 " ;

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

usr5int (s) 
register struct state *s;
{
	prstate (s);
	panic ("usr5: unexpected interrupt");
	evi ();
}

/*
 ***  block i/o devices  ***
 */
usr5open () 
{
	u.u_error = ENODEV ;
} 

usr5close ()
{
	u.u_error = ENODEV ;
} 

usr5strategy ()
{
	u.u_error = ENODEV ;
} 

/*
 ***  character i/o devices  ***
 */
u5open () 
{
	u.u_error = ENODEV ;
}

u5close ()
{
	u.u_error = ENODEV ;
}

u5read () 
{
	u.u_error = ENODEV ;
}

u5write () 
{
	u.u_error = ENODEV ;
}

u5ioctl () 
{
	u.u_error = ENODEV ;
}
