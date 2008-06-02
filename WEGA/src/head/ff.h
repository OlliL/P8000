/* ff.h	*/

/* #define DEBUG        /* for bug squashing code */
/* #define UNIXV7       /* if this is Version 7 */
/* #define UNIXV6       /* if this is Version 6 */

#ifdef UNIXV7
#ifndef major
#include <sys/types.h>
#endif
#endif

#include <stdio.h>
#define FF_BSIZE   BUFSIZ       /* Size of a FF block */

#define NOFILE _NFILE           /* Number of system opens allowed */
#define NOFFFDS (NOFILE+NOFILE) /* # of active FF files */

#ifndef NULL
#define NULL ((char *)0)
#endif

extern int errno;

#define FF_CHKF (ff < &ff_streams[0] || ff >= &ff_streams[NOFFFDS] || ff->f_file == 0)

#define CHKBLK	(!(fb = fp->fn_qf) || fb->fb_bnum != block || (ff_flist.fr_forw != fb && ff_flist.fr_forw->fb_forw != fb))
/* (   !(fb = fp->fn_qf)
/*  || fb->fb_bnum != block
/*  || (   ff_flist.fr_forw != fb
/*	&& ff_flist.fr_forw->fb_forw != fb
/*     )
/* )
/**/

typedef
struct ff_file			/* actual distinct files open	*/
{   struct ff_buf *fn_qf;	/* Q of in-core blocks	*/
				/**   Must be First    **/
    char	fn_fd,		/* File Descriptor	*/
		fn_mode,	/* Open mode for file	*/
		fn_refs;	/* # of references	*/
#ifdef UNIXV7
    dev_t       fn_dev;         /* Device file is on    */
    ino_t       fn_ino;         /* Inode of file        */
#endif
#ifdef UNIXV6
    char        fn_minor,       /* Device file is on    */
		fn_major;
    short       fn_ino;         /* Inode of file        */
#endif
    short       fn_realblk;     /* Image of sys fil pos */
    long        fn_size;        /* Current file size    */
} Ff_file;
extern Ff_file ff_files[];

typedef
struct ff_stream {		/* streams open */

    char        f_mode,         /* Open mode for handle */
		f_count;	/* Reference count	*/
    Ff_file    *f_file; 	/* Fnode pointer	*/
    long	f_offset;	/* Current file position*/
} Ff_stream;			/*  or buffered amount	*/
extern Ff_stream ff_streams[];
				/* f_flag bits		*/
#define F_READ	01		/* File opened for read */
#define F_WRITE 02		/* File opened for write*/

typedef struct	ff_buf
{   struct ff_buf *fb_qf,	/* Q of blks associated */
	       *fb_qb,		/*  with this file	*/
	       *fb_forw,	/* Active block chain	*/
	       *fb_back;	/*  forward and back	*/
    Ff_file    *fb_file;	/* Fnode blk is q'd on	*/
    int 	fb_bnum,	/* Block # of this blk	*/
		fb_count;	/* Byte count of block	*/
    int 	fb_wflg;	/* Block modified flag	*/
    char	fb_buf[FF_BSIZE];/* Actual data buffer	*/
} Ff_buf;

typedef struct ff_rbuf		/* MUST Mirror fbuf !!! 	*/
{   int 	fr_count,
		fr_dummy;
    Ff_buf     *fr_forw,	/* These two must have	*/
	       *fr_back;	/* same vals as in fbuf */
} Ff_rbuf;
extern Ff_rbuf ff_flist;

typedef struct ff_st {

    int 	fs_seek,	/* Total seek sys calls */
		fs_read,	/*	 read  "    "	*/
		fs_write;	/*	 write "    "	*/
} Ff_stats;
extern Ff_stats ff_stats;

long       ff_size (),
	   ff_pos (),
	   ff_seek ();
Ff_buf    *ff_getblk (),
	  *ff_gblk (),
	  *ff_putblk ();
Ff_stream *ff_open (),
	  *ff_fdopen ();
