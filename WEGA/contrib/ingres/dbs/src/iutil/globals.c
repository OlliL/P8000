/*
**  GLOBALS -- DBS globals which belong everywhere
**
**	Variables in this module should be included by everything
**	DBS knows about.  The real purpose of this module is
**	so that actual definition of space can occur here (and
**	everything can be 'extern' everywhere else).
**
**	Defines:
**		AAlockdes -- the lock descriptor for the concurrency
**			device.
*/
int	AAlockdes =	-1;	/* the concurrency device descriptor */
