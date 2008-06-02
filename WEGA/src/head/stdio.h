/* stdio.h */

#define	BUFSIZ	512
#define	_NFILE	20
#ifndef FILE
extern	struct	_iobuf {
#ifdef vax
	int	_cnt;
	char	*_ptr;
#else
	unsigned char	*_ptr;
	int	_cnt;
#endif
	char	*_base;
	char	_flag;
	char	_file;
} _iob[_NFILE];
#endif

#define	_IOREAD	01
#define	_IOWRT	02
#define	_IONBF	04
#define	_IOMYBUF	010
#define	_IOEOF	020
#define	_IOERR	040
#define	_IOSTRG	0100
#define	_IORW	0200

#ifndef NULL
#define	NULL	0
#endif

#define	FILE	struct _iobuf
#define	EOF	(-1)

#define	stdin	(&_iob[0])
#define	stdout	(&_iob[1])
#define	stderr	(&_iob[2])
#define	getc(p)		(--(p)->_cnt<0? _filbuf(p):*(p)->_ptr++)
#define	getchar()	getc(stdin)
#define putc(x,p) (--(p)->_cnt<0? _flsbuf((unsigned)(x),p):((int)(*(p)->_ptr++=(x))))
#define	putchar(x)	putc(x,stdout)
#define	feof(p)		(((p)->_flag&_IOEOF)!=0)
#define	ferror(p)	(((p)->_flag&_IOERR)!=0)
#define	fileno(p)	p->_file

FILE	*fopen();
FILE	*fdopen();
FILE	*freopen();
FILE	*popen();
long	ftell();
char	*fgets();
char	*gets();

#define L_ctermid	9
#define L_cuserid	9
#define L_tmpnam	19
