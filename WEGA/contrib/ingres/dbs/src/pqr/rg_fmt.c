/*
** ---  RG_FMT.C -------- PRINT PROCESSOR  ------------------------------------
**
**	Version: @(#)rg_fmt.c		4.0	02/05/89

**
*/

# include	"rg_int.h"

# define	PLUS		'+'
# define	MINUS		'-'
# define	ZERO		'0'
# define	POINT		'.'
# define	EXPONENT	'E'
# define	LEFT		'<'
# define	CENTER		'='
# define	RIGHT		'>'
# ifdef EBCDIC
# define	ESCAPE		0377
# else
# define	ESCAPE		0177
# endif


static char	DIGIT[]	= "0123456789ABCDEF";
static short	digits;
static short	digit_9;
static char	*last_digit;

static char	pic_buf[MAX_FIELD];
static int	pic_length;

static short	sign;
static short	do_sign;
static short	center;

static short	before;
static short	point;
static short	ignore;
static short	norm;
static short	exp_length;
static char	*exp_string;


fmt_byte(b)
int	b;
{
	register char		*col;
	register char		*rc;
	register int		len;
	register union anytype	*any;
	extern char		*conv_s();
	extern char		*AAbmove();
	extern char		*AApad();
	extern char		*print_type();

	any = &Print->sy_val;
	rc = (Print->sy_frmt == CHAR)? any->cptype: any->c0type;

	col = SYS_col;
	len = ctou(Print->sy_frml);

# ifdef RG_TRACE
	if (TR_FORMAT)
		printf("BYTE\tbase=%d  type %s%d length=%d\n",
			b, print_type(Print->sy_frmt), len, R_val);
# endif

	while (len--)
		if ((col = conv_s(RC_BYTE, col, -b)) >= Col_end)
		{
			col = Col_end;
			break;
		}

	if (R_val > 0)
	{
		if (R_val > (len = col - SYS_col))
			col = AApad(col, R_val - len);
		else
			col = SYS_col + R_val;
	}
	SYS_col = col;
}


fmt_word(b)
int	b;
{
	register char		*col;
	register char		*rc;
	register int		len;
	register union anytype	*any;
	extern char		*conv_s();
	extern char		*AApad();
	extern char		*print_type();

	any = &Print->sy_val;
	rc = (Print->sy_frmt == CHAR)? any->cptype: any->c0type;

	col = SYS_col;
	len = ctou(Print->sy_frml) / 2;

# ifdef RG_TRACE
	if (TR_FORMAT)
		printf("WORD\tbase=%d  type %s%d length=%d\n",
			b, print_type(Print->sy_frmt), len, R_val);
# endif

	while (len--)
	{
		RC_WORD;
		if ((col = conv_s(R_word, col, b)) >= Col_end)
		{
			col = Col_end;
			break;
		}
	}

	if (R_val > 0)
	{
		if (R_val > (len = col - SYS_col))
			col = AApad(col, R_val - len);
		else
			col = SYS_col + R_val;
	}
	SYS_col = col;
}


fmt_int(rc)
register char	*rc;
{
	extern char	*do_scale();

# ifdef RG_TRACE
	if (TR_FORMAT)
		printf("INT\n");
# endif

	if (R_val < 0)
		R_val = 0;
	else
		AApad(SYS_col, R_val);

	if (do_pic(do_scale(rc)))
		do_int();

	SYS_col += R_val;
}


fmt_real(rc)
register char	*rc;
{
	extern char	*do_scale();

# ifdef RG_TRACE
	if (TR_FORMAT)
		printf("REAL\n");
# endif

	if (R_val < 0)
		R_val = 0;
	else
		AApad(SYS_col, R_val);

	if (rc = do_scale(rc))
	{
		RC_WORD;
		before = var_to_int();
		if (before < 0)
			before = A_BEFORE;

		RC_WORD;
		point = var_to_int();
		if (point < 0)
			point = A_POINT;

		RC_WORD;
		ignore = var_to_int();
		RC_WORD;
		norm = var_to_int();
		if (bitset(B_NORM, ignore))
			norm = -norm;
		else
			setbit(B_NORM, ignore);
		if (norm == A_NORM)
			clrbit(B_NORM, ignore);
		else if (norm < L_NORM)
			norm = L_NORM;
		else if (norm > H_NORM)
			norm = H_NORM;

		RC_WORD;
		exp_length = var_to_int();
		exp_string = rc;
		do_real();
	}

	SYS_col += R_val;
}


fmt_char(from, to, blen)
int	from;
int	to;
int	blen;
{
	register int		olen;
	register int		slen;
	register char		*sadr;
	register SYMBOL		*s;
	register union anytype	*any;
	int			i;
	extern char		*print_type();

	s = Print;
	if (s->sy_frmt != CHAR)
	{
		rg_error("\tCHAR: Funny type %s%d\nCHAR-statement ignored",
			print_type(s->sy_frmt), s->sy_frml);
		return;
	}
	slen = ctou(s->sy_frml);
	/* sadr points behind string, to truncate trailing blanks */
	any =  &s->sy_val;
	sadr = any->cptype + slen;
	while (*(--sadr) == ' ')
	{
		if (slen == 1)
			break;
		--slen;
	}

	/* get the correct string length */
	i = slen;	/* save length   */
	slen = ck_len(&i, from, to);

	/* get the correct string address */
	any = &s->sy_val;
	sadr = any->cptype + slen;
	slen = i;	/* restore updated length */

	if ((olen = R_val) <= 0)
		olen = slen;
	else if (olen < slen)
		slen = olen;

	rg_batch(olen, blen, slen, sadr);
}


char	*fmt_graph(rc)
register char	*rc;
{
	register int	len;
	extern char	*do_graph();

	if ((len = R_val) < 0)
		R_val = len = 0;
	else
		AApad(SYS_col, len);
	rc = do_graph(rc);
	SYS_col += len;
	return (rc);
}


static char	*conv_s(val, pos, base)
int	val;
register char	*pos;
register int	base;
{
	register char	*b;
	register int	bytes;
	long		v;
	char		buffer[17];

	v = val;
	switch (base)
	{
	  case 2:
		bytes = 0;
		goto word_format;

	  case -2:
		bytes = 8;
		goto byte_format;

	  case 8:
		bytes = 10;
		goto word_format;

	  case -8:
		bytes = 13;
		goto byte_format;

	  case 10:
		bytes = 11;
		goto word_format;

	  case -10:
		bytes = 13;
		goto byte_format;

	  case 16:
		bytes = 12;
word_format:
		v &= 0xffffL;
		break;

	  case -16:
		bytes = 14;
byte_format:
		base = -base;
		v &= 0xffL;
	}
	for (b = buffer; b < &buffer[16]; )
		*b++ = '0';
	*b = ' ';

	do
	{
		*--b = DIGIT[v % base];
		v /= base;
	} while (v);

	for (b = &buffer[bytes]; b < &buffer[17]; )
		*pos++ = *b++;

	return (pos);
}


static char	*do_scale(rc)
register char	*rc;
{
	double			scale;
	register union anytype	*any;

	if (do_f8())
		return ((char *) 0);

	AAbmove(rc, &scale, sizeof (double));

# ifdef RG_TRACE
	if (TR_FORMAT)
		printf("SCALE\t%10.3f\n", scale);
# endif

	any = &Print->sy_val;
	any->f8type *= scale;	/* do scale */

	sign = PLUS;
	if (any->f8type < 0.0)
	{
		any->f8type = - any->f8type;
		sign = MINUS;
	}
	return (rc + sizeof (double));
}


static	do_f8()
{
	if (convert(Print))
	{
		rg_error("CANNOT convert CHAR to REAL\n\tINT- or REAL-statement ignored");
		return (TRUE);
	}
	return (FALSE);
}


static	do_pic(rc)
register char	*rc;
{
	register int	len;
	register char	*b;
	register int	have_sign;

	if (rc == (char *) 0)
		return (FALSE);

	center = RIGHT;
	do_sign = FALSE;
	digit_9 = 0;
	digits = 0;
	have_sign = 0;
	b = pic_buf;

	/* read the picture length and string */
	RC_WORD;
	pic_length = len = R_word;
	AAbmove(rc, b, len);

# ifdef RG_TRACE
	if (TR_FORMAT)
	{
		b[len] = '\0';	/* make a string for printing */
		printf("PIC\tlen=%d '%s'\n", len, b);
	}
# endif

	if (!len)
		return (TRUE);	/* ok, no picture */

	/* inline picture convert */
	rc = b;
	for ( ; len; --len)
	{
		switch (*b)
		{
		  case '\\':	/* get next character */
			--len;
			b++;
			break;

		  case LEFT:
			center = LEFT;
			goto skip;

		  case CENTER:
			center = CENTER;
			goto skip;

		  case RIGHT:
			center = RIGHT;
skip:
			b++;
			continue;

		  case '0':
			if (digit_9)
				break;	/* '0' is a normal character */
			goto inc_digits;

		  case '9':
			digit_9++;
			last_digit = rc;
inc_digits:
			digits++;
			*b = ESCAPE;
			break;

		  case MINUS:
		  case PLUS:
			if (have_sign++)
				break;
			if (*b == MINUS && sign == PLUS)
				sign = ' ';
			if (digits && !digit_9)
			{
				do_sign = TRUE;
				goto inc_digits;
			}
			*b = sign;
		}

		*rc++ = *b++;
	}

	pic_length = len = rc - pic_buf;
	if (len && !digit_9)
	{
		rg_error("INT: missing '9' in picture string");
		return (FALSE);
	}

	if (!have_sign)
	{
		do_sign = TRUE;
		if (digit_9 == digits)
			--digit_9;
		if (sign == PLUS)
			sign = ' ';
	}

	if (!R_val)
		R_val = len;
	else if (R_val < len)
	{
		/* sorry joker, you loose */
		rg_error("INT: output length '%d' < picture length '%d'",
			R_val, len);
		return (FALSE);
	}

# ifdef RG_TRACE
	if (TR_FORMAT)
	{
		b = pic_buf;
		b[pic_length] = '\0';	/* make a string for printing */
		printf("PIC-NEW\tlen=%d '%s'\n", pic_length, b);
		printf("\tdigit_9 =%d digits=%d do_sign=%d\n",
				digit_9, digits, do_sign);
	}
# endif
	return (TRUE);
}


static	do_int()
{
	register char		*b;	/* dig_buf */
	register char		*p;	/* pic_buf */
	register int		len;
	register union anytype	*any;
	register int		i;
	int			expon;
	int			i_dummy;
	char			c_dummy[1];
	extern char		*fcvt();
	extern char		*AAbmove();

	any = &Print->sy_val;
	p = fcvt(any->f8type, 0, &expon, &i_dummy);
	if ((len = expon) >= MAX_FIELD)
	{
		rg_error("INT: buffer length '%d' < digit length '%d'",
			MAX_FIELD - 1, len);
		goto joker;
	}
	if (len <= 0)
	{
		len = 1;
		p = c_dummy;
		*p = ZERO;
	}

# ifdef RG_TRACE
	if (TR_FORMAT)
		printf("INT\tlen=%d\n", len);
# endif

	if (!pic_length)
	{
		/* no picture */
		if (sign == MINUS)
		{
			AAbmove(p, pic_buf + 1, len);
			p = pic_buf;
			*p = MINUS;
			len++;
		}

		if (!R_val)
			R_val = len;

		if (len > R_val)
		{
			/* sorry joker, yoo loose */
			rg_error("INT(%d): output length '%d' < digit length '%d'",
				(int) any->f8type, R_val, len);
			goto joker;
		}
		goto dig_out;
	}

	/* picture */
	if ((i = do_sign? len + 1: len) > digits)
	{
		/* sorry joker, yoo loose again */
		rg_error("INT(%d): picture fields '%d' < digit length '%d'",
			(int) any->f8type, digits, i);
joker:
		cmove(SYS_col, R_val, '*');
		return;
	}

	digits -= len;
	digit_9 -= len;
	b = &p[len];
	p = ++last_digit;

	/* transfer digits */
	do
	{
		while (ctou(*(--p)) != ESCAPE)
			continue;
		*p = *(--b);
	} while (--len);

	/* transfer zeros */
	len = digit_9;
	while (--len >= 0)
	{
		--digits;
		while (ctou(*(--p)) != ESCAPE)
			continue;
		*p = ZERO;
	};

	/* transfer sign */
	if (do_sign)
	{
		--digits;
		while (ctou(*(--p)) != ESCAPE)
			continue;
		*p = sign;
	};

	/* transfer blanks */
	len = digits;
	while (--len >= 0)
	{
		while (ctou(*(--p)) != ESCAPE)
			continue;
		*p = ' ';
	};

	/* now move the complete picture to output */
	len = pic_length;
	p = pic_buf;
dig_out:
	b = SYS_col;

	/* center */
	if (center == CENTER)
		b += (R_val - len) / 2;
	else if (center == RIGHT)
		b += R_val - len;

	b = AAbmove(p, b, len);
	AApad(b, R_val - len);
}


static	do_real()
{
	register char	*cvt;
	register char	*p;
	register int	i;
	register int	save_before;
	register int	save_point;
	register int	hope;
	union anytype	*any;
	double		val;
	int		exp;
	extern char	*ecvt();
	extern char	*fcvt();

	save_before = before;
	hope = TRUE;
	any = &Print->sy_val;
	val = any->f8type;

	if (bitset(B_IGNORE, ignore))
	{
		cvt = fcvt(val, point, &exp, &do_sign);
		p = pic_buf;

		if (sign == MINUS)
			*p++ = MINUS;

		if (exp <= 0)
			*p++ = ZERO;
		else
		{
			i = exp;
			while (--i >= 0)
				*p++ = *cvt? *cvt++: ZERO;
		}

		if (point)
			*p++ = POINT;

		i = point;
		while (--i >= 0)
			*p++ = (exp++ < 0)? ZERO: *cvt? *cvt++: ZERO;
output:
		i = p - pic_buf;
		if (!R_val)
			R_val = i;
		if (R_val >= i)
		{
			/* output fits */
			p = SYS_col + R_val - i;
			AAbmove(pic_buf, p, i);
			return;
		}
		/* let's hope for another style */
	}

	if (hope)
	{
last_hope:
		before = save_before;
		save_point = point;
		if (bitset(B_NORM, ignore))
		{
			clrbit(B_NORM, ignore);
			ecvt(val, 1, &exp, &do_sign);
			save_point = exp - norm;
			before = (save_point < 0)? 0: save_point;
		}
		else
			hope = FALSE;

		cvt = ecvt(val, before + point, &exp, &do_sign);
		p = pic_buf;

		if (sign == MINUS)
			*p++ = MINUS;

		if (!(i = before))
			*p++ = ZERO;
		else
		{
			exp -= i;
			while (--i >= 0)
				*p++ = *cvt? *cvt++: ZERO;
		}

		if (point)
			*p++ = POINT;

		if ((i = save_point) < 0)
		{
			i = - i;
			exp += i;
			if ((save_point = point - i) < 0)
				i = point;
			while (--i >= 0)
				*p++ = ZERO;
		}
		else
			save_point = point;

		i = save_point;
		while (--i >= 0)
			*p++ = *cvt? *cvt++: ZERO;

		if (i = exp_length)
		{
			cvt = exp_string;
			while (--i >= 0)
				*p++ = *cvt++;
		}
		else
			*p++ = EXPONENT;
		if (exp < 0)
		{
			*p++ = MINUS;
			exp = - exp;
		}
		else
			*p++ = PLUS;
		*p++ = (exp / DEC) % DEC + ZERO;
		*p++ = exp % DEC + ZERO;
		goto output;
	}

	if (save_before > 1)
	{
		save_before = A_BEFORE;
		goto last_hope;
	}

	/* sorry joker, you loose */
	rg_error("REAL(%10.3f): output length '%d' < digit length '%d'",
		val, R_val, i);
	cmove(SYS_col, R_val, '*');
}


static char	*do_graph(rc)
register char	*rc;
{
	register int		i;
	register char		*p;
	register int		c;
	register int		rev;
	double			from;
	double			to;
	double			by;
	register union anytype	*any;

	rev = do_f8();

	/* read "from" argument */
	RC_WORD;
	if (!R_word)
	{
		/* not a variable, read value */
		AAbmove(rc, &from, sizeof (double));
		rc += sizeof (double);
	}
	else	/* variable, get the value of var */
		from = var_to_int();

	/* read "to" argument */
	RC_WORD;
	if (!R_word)
	{
		/* not a variable, read value */
		AAbmove(rc, &to, sizeof (double));
		rc += sizeof (double);
	}
	else	/* variable, get the value of var */
		to = var_to_int();

	/* read the "with" argument */
	c = *rc++;

	if (rev || !R_val)
		return (rc);

	p = SYS_col;
	if (rev = from > to)
	{
		by = from;
		from = to;
		to = by;
	}
	if ((by = (to - from) / R_val) != 0)
	{
		any = &Print->sy_val;
		if ((i = (any->f8type - from) / by) > R_val)
			i = R_val;
		else if (i < 0)
			i = 0;
		if (rev)
			p += R_val - i;
	}
	else
	{
		i = R_val;
		c = '*';
	}
	cmove(p, i, c);
	return (rc);
}
