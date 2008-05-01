/*-
 * Copyright (c) 2007 Oliver Lehmann <oliver@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: crc.c,v 1.5 2008/05/01 16:31:11 olivleh1 Exp $
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

static void usage();

main(argc, argv)
char **argv;
{
	char d,e,a1,a2;
	int i,c = 0;
	int begin = 0;
	int end = -1;
	int f;
	char *filename;

	if(argc < 2 || argc > 4 )
		usage(argv[1]);

	filename = argv[1];

	if(argc > 2)
		begin = atoi(argv[2]);

	if(argc > 3)
		end = atoi(argv[3]);

	if( end != -1 && begin > end ) {
		fprintf(stderr, "begin adress cannot be higher than end adress\n");
		usage(argv[1]);
	}

#ifdef O_BINARY
	f = open(filename, O_RDONLY|O_BINARY);
#else
	f = open(filename, O_RDONLY);
#endif
	if (f<0) {
		fprintf(stderr, "cannot open %s \n", filename);
		usage(argv[1]);
	}

	d=0xff; e=0xff;

	while( read(f, &a1, sizeof(a1)) ) {
		if( c == end )
			break;

		if( c >= begin ) {

			a1=a1 ^ d;
			d=a1;
			for (i=0;i<4;i++)
				{if ((a1 & 1)> 0)	{a1=a1 >> 1; a1=a1 | 0x80;}
				else			{a1=a1 >> 1; a1=a1 & 0x7f;}}
			a1=a1 & 0x0f;
			a1=a1 ^ d;
			d=a1;
			for (i=0;i<3;i++)
				{if ((a1 & 1)>0)	{a1=a1 >> 1; a1=a1 | 0x80;}
				else			{a1=a1 >> 1; a1=a1 & 0x7f;}}
			a2=a1;
			a1=a1 & 0x1f;
			a1=a1 ^ e;
			e=a1;
			a1=a2;
			if ((a1 & 1)>0) {a1=a1 >> 1; a1=a1 | 0x80;}
				else	{a1=a1 >> 1; a1=a1 & 0x7f;}
			a1=a1 & 0xf0;
			a1=a1 ^ e;
			e=a1;
			a1=a2;
			a1=a1 & 0xe0;
			a1=a1 ^ d;
			d=e;
			e=a1;
			a1=0xff;
		}
		c++;
	}
	close(f);
	printf ("%s %02x%02x\n","crc_checksum:",d & 0xff,e & 0xff);

	return 0;
}

static void usage(progname)
char *progname;
{
	printf("usage: %s file [start adress] [end adress]\n", progname);
	exit(1);
}
