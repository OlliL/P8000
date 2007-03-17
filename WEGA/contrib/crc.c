/* berechnet EPROM Checksummen */

#include <stdio.h>

static void usage(char *);

int main(int argc, char **argv) {
	char d,e,a1,a2;
	int i,begin=0,end=-1,c;
	FILE *f;
	char *filename;

	if(argc < 2 || argc > 4 )
		usage(argv[0]);
		
	filename = argv[1];
	
	if(argc > 2)
		begin = atoi(argv[2]);
		
	if(argc > 3)
		end = atoi(argv[3]);
    
	if( end != -1 && begin > end ) {
		fprintf(stderr, "begin adress cannot be higher than end adress\n");
		usage(argv[0]);
	}

	if( ! ( f = fopen(filename, "rb") ) ) {
		fprintf(stderr, "cannot open %s \n", filename);
		usage(argv[0]);
	}

	fseek(f,begin,SEEK_SET);

	d=0xff; e=0xff;

	c=begin;

	while(!feof(f)) {
		if( end != -1 && c == end )
			break;
		else
			c++;
		a1 = fgetc(f);
		if( ferror(f) ) {
			printf("File error\n");
			break;
		}
		if( feof(f) ) {
			break;
		}
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
		if ((a1 & 1)>0)	{a1=a1 >> 1; a1=a1 | 0x80;}
			else	{a1=a1 >> 1; a1=a1 & 0x7f;}
		a1=a1 & 0xf0;
		a1=a1 ^ e;
		e=a1;
		a1=a2;
		a1=a1 & 0xe0;
		a1=a1 ^ d;
		d=e;
		e=a1;
	} 
	fclose(f);
	printf ("%s %02x%02x\n","crc_checksum:",d & 0xff,e & 0xff);

	return 0;
}

static void usage(char *progname) {
	printf("usage: %s file [start adress] [end adress]\n", progname);
	exit(1);
}
