char udev6wstr[] = "@[$]udev6.c		Rev : 1.1 " ;

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

usr6int (s) 
register struct state *s;
{
	prstate (s);
	panic ("usr6: unexpected interrupt");
	evi ();
}

/*
 ***  block i/o devices  ***
 */
usr6open () 
{
	u.u_error = ENODEV ;
} 

usr6close ()
{
	u.u_error = ENODEV ;
} 

usr6strategy ()
{
	u.u_error = ENODEV ;
} 

/*
 ***  character i/o devices  ***
 */
u6open () 
{
	u.u_error = ENODEV ;
}

u6close ()
{
	u.u_error = ENODEV ;
}

u6read () 
{
	u.u_error = ENODEV ;
}

u6write () 
{
	u.u_error = ENODEV ;
}

u6ioctl () 
{
	u.u_error = ENODEV ;
}
