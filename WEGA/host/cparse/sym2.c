# include "mfile1"

int sputc(int c, FILE *f)
{
	return( putc( c, f ) );
	}

/* the .S file is big endian (Z8000): a word and a long, byte by byte */
void put16(FILE *f, int v)
{
	putc( (v >> 8) & 0xFF, f );
	putc( v & 0xFF, f );
	}

void put32(FILE *f, long v)
{
	put16( f, (int) ((v >> 16) & 0xFFFF) );
	put16( f, (int) (v & 0xFFFF) );
	}

void aobeg(void)
{
	}
