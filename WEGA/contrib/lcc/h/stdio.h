#ifndef _NFILE
#define _NFILE	20

#define BUFSIZ	512

/* buffer size for multi-character output to unbuffered files */
#define _SBFSIZ 8

typedef struct {
	unsigned char	*_ptr;
	int	_cnt;
	unsigned char	*_base;
	char	_flag;
	char	_file;
} FILE;

/*
 * _IOLBF means a file is a terminal; this means, in particular,
 * that output will be buffered line by line
 */
#define _IOREAD		0001
#define _IOWRT		0002
#define _IONBF		0004
#define _IOMYBUF	0010
#define _IOEOF		0020
#define _IOERR		0040
#define _IOLBF		0100
#define _IORW		0200

#ifndef NULL
#define NULL		((void *)0)
#endif
#ifndef EOF
#define EOF		(-1)
#endif

#define stdin		(&_iob[0])
#define stdout		(&_iob[1])
#define stderr		(&_iob[2])

#define _bufend(p)	_bufendtab[(p)->_file]
#define _bufsiz(p)	(_bufend(p) - (p)->_base)

#ifndef lint
#define getc(p)		(--(p)->_cnt >= 0 ? (int) *(p)->_ptr++ : _filbuf(p))
#define putc(x, p)	(--(p)->_cnt >= 0 ? \
			((int) (*(p)->_ptr++ = (unsigned char) (x))) : \
			_flsbuf((unsigned char) (x), (p)))
#define getchar()	getc(stdin)
#define putchar(x)	putc((x), stdout)
#define clearerr(p)	((void) ((p)->_flag &= ~(_IOERR | _IOEOF)))
#define feof(p)		((p)->_flag & _IOEOF)
#define ferror(p)	((p)->_flag & _IOERR)
#define fileno(p)	(p)->_file
#endif

extern FILE	_iob[_NFILE];
extern FILE	*fopen(), *fdopen(), *freopen(), *popen(), *tmpfile();
extern long	ftell();
extern void	rewind(), setbuf();
extern char	*ctermid(), *cuserid(), *fgets(), *gets(), *tempnam(), *tmpnam();
extern unsigned char *_bufendtab[];

#define L_ctermid	9
#define L_cuserid	9
#define P_tmpdir	"/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir) + 15)
#endif
