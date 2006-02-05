/*-
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and	binary forms, with or without
 * modification, are permitted provided	that the following conditions
 * are met:
 * 1. Redistributions of source	code must retain the above copyright
 *    notice, this list	of conditions and the following	disclaimer.
 * 2. Redistributions in binary	form must reproduce the	above copyright
 *    notice, this list	of conditions and the following	disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials	mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the	University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or	promote	products derived from this software
 *    without specific prior written permission.
 *
 * THIS	SOFTWARE IS PROVIDED BY	THE REGENTS AND	CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT	SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES;	LOSS OF	USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH	DAMAGE.
 */

/*
 * uuencode [input] output
 *
 * Encode a file so it can be mailed to	a remote system.
 */
/*#include <sys/stat.h>*/

#define S_IRWXU 0000700
#define S_IRWXG 0000070
#define S_IRWXO 0000007
#define S_IRUSR 0000400
#define S_IWUSR 0000200
#define S_IRGRP 0000040
#define S_IWGRP 0000020
#define S_IROTH 0000004
#define S_IWOTH 0000002

#include <stdio.h>

int
main(argc, argv)
	int argc;
	char *argv[];
{
	extern int optind;
	extern int errno;

	while (getopt(argc, argv, "") != EOF)
		usage();
	argv +=	optind;
	argc -=	optind;

	switch(argc) {
	case 2:			/* optional first argument is input file */
		if (!freopen(*argv, "r", stdin)	) {
			(void)fprintf(stderr, "uuencode: %s: %s.\n",
			    *argv, errno);
			exit(1);
		}
		++argv;
		break;
	case 1:
		break;
	case 0:
	default:
		usage();
	}

	(void)printf("begin 777 %s\n", *argv);
	encode();
	(void)printf("end\n");
	if (ferror(stdout)) {
		(void)fprintf(stderr, "uuencode: write error.\n");
		exit(1);
	}
	exit(0);
}

/* ENC is the basic 1 character	encoding function to make a char printing */
#define	ENC(c) ((c) ? ((c) & 077) + ' ': '`')

/*
 * copy	from in	to out,	encoding as you	go along.
 */
encode()
{
	register int ch, n;
	register char *p;
	char buf[80];

	while (n = fread(buf, 1, 45, stdin)) {
		ch = ENC(n);
		if (putchar(ch)	== EOF)
			break;
		for (p = buf; n	> 0; n -= 3, p += 3) {
			ch = *p	>> 2;
			ch = ENC(ch);
			if (putchar(ch)	== EOF)
				break;
			ch = (*p << 4) & 060 | (p[1] >>	4) & 017;
			ch = ENC(ch);
			if (putchar(ch)	== EOF)
				break;
			ch = (p[1] << 2) & 074 | (p[2] >> 6) & 03;
			ch = ENC(ch);
			if (putchar(ch)	== EOF)
				break;
			ch = p[2] & 077;
			ch = ENC(ch);
			if (putchar(ch)	== EOF)
				break;
		}
		if (putchar('\n') == EOF)
			break;
	}
	if (ferror(stdin)) {
		(void)fprintf(stderr, "uuencode: read error.\n");
		exit(1);
	}
	ch = ENC('\0');
	(void)putchar(ch);
	(void)putchar('\n');
}

usage()
{
	(void)fprintf(stderr,"usage: uuencode [infile] remotefile\n");
	exit(1);
}
