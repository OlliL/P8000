/* unpack_run: copy the name of a symbol into the 14 byte table form, zero padded (sld unpack_run; 0x22e8..0x2360 of /bin/ld) */

static char sccsid[] = "@[$]copyname.c\t1.3  03/11/83 10:28:51 - 87wega3.2";

unpack_run(src, dst)
char *src;
char *dst;
{
	register char *s;
	register char *d;
	int n;
	int spare1, spare2;
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

/* pack_name: build the table form of a C string (sld pack_name; 0x2360..0x23e2 of /bin/ld, finding) */
pack_name(src, dst)
char *src;
char *dst;
{
	int spare1, spare2;
	char spare3;
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
		goto test;
body:
		*d++ = 0;
test:
		if (cnt-- != 0)
			goto body;
	}
}

