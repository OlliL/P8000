
static char sccsid[] = "@[$]copyname.c\t1.3  03/11/83 10:28:51 - 87wega3.2";
int spare05d6 = 0;	/* unused word after the stamp in the data of the original (0x05d6); guess: a variable of copyname.c */

/* unpack_run: copy a symbol name in the table form (8 bytes, or a long name with its continuation entries) to dst,
   zero padded; the number of continuation entries (sld unpack_run, Plexus cp8c; 0x22e8..0x2360 of /bin/ld) */
unpack_run(src, dst)
char *src;
char *dst;
{
	int n;
	register char *s;
	register char *d;
	char ch;

	n = 0;
	s = src;
	d = dst;
	if ((ch = *s) & 0x80) {
		ch &= 0x7f;
		n = (ch + 5) / 14;
		ch = n * 14 + 8;
	} else
		ch = 8;
	goto test;
body:
	if ((*d++ = *s) != 0)
		s++;
test:
	if (ch-- != 0)
		goto body;
	return n;
}

/* pack_name: build the table form of a C string: up to 8 characters zero padded, a longer name with the length byte
   and continuation entries (sld pack_name; 0x2360..0x23e2 of /bin/ld) */
pack_name(src, dst)
char *src;
char *dst;
{
	register char *q;
	register char *d;
	register char *s;
	char cnt;

	s = src;
	q = src;
	d = dst;
	cnt = 0;
	goto test0;
body0:
	cnt++;
test0:
	if (*s++)
		goto body0;
	if (cnt <= 8)
		unpack_run(q, d);
	else {
		*d++ = cnt | 0x80;
		s--;
		goto test1;
body1:
		*d++ = *q++;
test1:
		if (q < s)
			goto body1;
		cnt -= 8;
		goto test2;
body2:
		cnt -= 14;
test2:
		if (cnt > 0)
			goto body2;
		while (cnt-- != 0)
			*d++ = 0;
	}
}

