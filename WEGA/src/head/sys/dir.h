/* dir.h */

# ifndef DIRSIZ
# define DIRSIZ		14
# endif DIRSIZ

struct	direct
{
	ino_t	d_ino;
	char	d_name[DIRSIZ];
};
