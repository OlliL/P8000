/*	<sys/types.h> must be included.	*/

#define	UTMP_FILE	"/etc/utmp"
#define	WTMP_FILE	"/usr/adm/wtmp"
#define	ut_name	ut_user

struct utmp
  {
	char ut_line[8] ;		/* device name (console, lnxx) */
	char ut_user[8] ;		/* User login name */
	long ut_time ;			/* time entry was made */
  } ;
