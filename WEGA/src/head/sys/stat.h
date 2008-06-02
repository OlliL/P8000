/* stat.h */

struct	stat
{
	dev_t		st_dev;		/* device of inode 		      */
	ino_t		st_ino;		/* inode number 		      */
	unsigned short 	st_mode; 	/* mode bits 			      */
	short		st_nlink;	/* number of links to file 	      */
	short  		st_uid;		/* owner's userid 		      */
	short  		st_gid;		/* owner's group id 		      */
	dev_t		st_rdev;	/* for special files 		      */
	off_t		st_size;	/* file size in characters 	      */
	time_t		st_atime;	/* time last accessed 		      */
	time_t		st_mtime;	/* time last modified 		      */
	time_t		st_ctime;	/* time originally created 	      */
};

# define S_IFMT		0170000		/* type of file mask 		      */
# define S_IFDIR	0040000		/* directory 			      */
# define S_IFCHR	0020000		/* character special 		      */
# define S_IFBLK	0060000		/* block special 		      */
# define S_IFREG	0100000		/* regular 			      */
# define S_IFIFO	0010000		/* fifo				      */
# define S_ISUID	0004000		/* set user id on execution 	      */
# define S_ISGID	0002000		/* set group id on execution 	      */
# define S_ISVTX	0001000		/* save swapped text even after use   */
# define S_IREAD	0000400		/* read permission, owner 	      */
# define S_IWRITE	0000200		/* write permission, owner 	      */
# define S_IEXEC	0000100		/* execute/search permission, owner   */
