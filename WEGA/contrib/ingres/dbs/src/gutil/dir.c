# include	"gen.h"
# ifdef NON_V7_FS

# ifdef DIR_BSD4
# ifdef VAX
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(# )dir.h	7.1 (Berkeley) 6/4/86
 */

# define	DIRBLKSIZ	512
# endif

# ifdef HP_UX
/*
** HPUX_ID:	@(#)ndir.h	27.3    85/07/08
** UNISRC_ID:	@(#)ndir.h	26.1	84/07/03
*/

# define	DIRBLKSIZ	1024
# endif

# define	MAXNAMLEN	255

struct	direct
{
	long	d_ino;			/* inode number of entry */
	short	d_reclen;		/* length of this record */
	short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};

/*
 * Definitions for library routines operating on directories.
 */
struct _dirdesc
{
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
	char	dd_buf[DIRBLKSIZ];
};
# endif

extern struct _dirdesc		*opendir();
extern void			closedir();
extern struct direct		*readdir();


struct _dirdesc			*Dir_ptr;
struct _dirdesc			*Dir_2ptr;


open_dir(dir)
register char	*dir;
{
	if (Dir_ptr)
		Dir_2ptr = Dir_ptr;
	if (Dir_ptr = opendir(dir))
		return (1);
	if (Dir_2ptr)
	{
		Dir_ptr = Dir_2ptr;
		Dir_2ptr = (struct _dirdesc *) 0;
	}
	return (0);
}


close_dir()
{
	if (Dir_ptr)
	{
		closedir(Dir_ptr);
		if (Dir_2ptr)
		{
			Dir_ptr = Dir_2ptr;
			Dir_2ptr = (struct _dirdesc *) 0;
		}
		else
			Dir_ptr = (struct _dirdesc *) 0;
	}
}


char	*read_dir()
{
	static struct direct		*dir;

	dir = readdir(Dir_ptr);
	return (dir? dir->d_name: ((char *) 0));
}


# endif
