# include	"amx.h"

static char	*move(src, dst)
register char	*src;
register char	*dst;
{
	while (*src)
		*dst++ = *src++;

	return (dst);
}


mixed(src, dst)
register int	src;	/* type of src */
register int	dst;	/* type of dst */
{
	if (dst == NOTYPE || src == NOTYPE)
		return (TRUE);

	if (dst == opSTRING)
	{
		if (src == opSTRING)
			return (FALSE);		/* ok, both CHAR's */
		amxerror(196);
	}
	else if (src != opSTRING)
		return (FALSE);			/* ok, both NUMERIC's */
	else
		amxerror(63);

	return (TRUE);
}


char	*conv(type, src)
register int	type;
register int	src;	/* flag, set if operand is source */
{
	register char		*p;

	p = CONVERT[type];
	if (type == opCHAR && !src)
		p += 4;		/* skip ctoi for destination */

	return (p);
}


char	*value(f)
register AMX_FRM	*f;
{
	register char		*p;
	static char		buf[MAXLINE];
	extern char		*AA_iocv();
	extern char		*AA_locv();
	extern char		*move();
	extern char		*conv();
	extern char		*addr();

	p = buf;
	if (f->off == C_CONST)
	{
		switch (f->type)
		{
		  case opINT:		/* ICONST */
			p = move(AA_iocv(f->c_var), p);
			break;

		  case opLONG:		/* LCONST */
			p = move(AA_locv(*((long *) f->c_var)), p);
			break;

		  case opDOUBLE:	/* RCONST */
			p = move(f->c_var, p);
			break;

		  case opSTRING:	/* SCONST */
			yyexit(0, f->c_var);
		}
	}
	else if (f->off == C_VAR)
		p = move(f->c_var, p);
	else
	{
		/* domain */
		p = move(conv(f->type, f == &Source), p);
		p = move(addr(f), p);
		*p++ = ')';
	}

	*p = 0;
	return (buf);
}


char	*addr(f)
register AMX_FRM		*f;
{
	register char		*p;
	static char		buf[MAXLINE];
	extern char		*AA_iocv();
	extern char		*move();

	p = buf;
	if (f->off == C_CONST)
	{
		if (f->type == opSTRING)
		{
			/* move value */
			*p++ = '"';
			p = move(f->c_var, p);
			*p++ = '"';
		}
		else
		{
			/* get address of substituted value */
			p = move("&AA_var", p);
		}
	}
	else if (f->off == C_VAR)
	{
		if (f->type != opSTRING)
			*p++ = '&';
		p = move(f->c_var, p);
	}
	else
	{
		/* domain */
		*p++ = '&';
		p = move((f == &Source)? "AA_STUP": "AA_ptup", p);
		*p++ = '[';
		p = move(AA_iocv(f->off), p);
		*p++ = ']';
	}

	*p = 0;
	return (buf);
}


convert()
{
	register int		src_type;
	register int		dst_type;
	register int		src_off;
	register int		dst_off;
	register AMX_FRM	*src;
	register AMX_FRM	*dst;
	extern char		*conv();
	extern char		*value();
	extern char		*addr();

	Dom_set[Domains++] = Dom;
	src_type = (src = &Source)->type;
	src_off = src->off;
	dst_type = (dst = &Destination)->type;
	dst_off = dst->off;

	/* don't mix CHAR and NUMERIC */
	if (mixed(src_type, dst_type))
		return;

	if (src_type == opSTRING)
	{
		char_convert();
		return;
	}

	/* TRY TO OPTIMIZE:				 */
	/* 1:	i1 to i1				 */
	/* 2:	i1 to type (if even off)		 */
	/* 3:	type (if even off) to i1		 */
	/* 4:	type (if even off) to type (if even off) */
	/* 5:	type to type (if same types)		 */

	if ((!bitset(ODD, src_off) || src_type == opCHAR) &&
	    (!bitset(ODD, dst_off) || dst_type == opCHAR))
	{
		fprintf(FILE_tmp, "%s = ", value(dst));
		fprintf(FILE_tmp, "%s;\n", value(src));
		return;
	}

	if (src_off == C_CONST)
	{
		/* handle special case of constants */
		fprintf(FILE_tmp, "%s%s) = %s;\n",
			conv(src_type, 0), "&AA_var", value(src));
	}

	if (src_type == dst_type)
	{
		fprintf(FILE_tmp, "AAbmove(%s,", addr(src));
		fprintf(FILE_tmp, "%s,%d);\n", addr(dst), ctou(dst->len));
	}
	else
	{
		/* nothing found to optimize */
		num_convert();
	}
}


static char_convert()
{
	register AMX_FRM	*src;
	register AMX_FRM	*dst;
	register int		slen;
	register int		dlen;

	slen = ctou((src = &Source)->len);
	dlen = ctou((dst = &Destination)->len);

	if (src->off < 0)
	{
		/* c_var to dom */
		fprintf(FILE_tmp, "AApmove(%s,", addr(src));
		fprintf(FILE_tmp, "%s,%d,' '", addr(dst), dlen);
	}
	else if (dst->off < 0)
	{
		/* scan_dom to c_var */
		fprintf(FILE_tmp, "AAvmove(%s,", addr(src));
		fprintf(FILE_tmp, "%s,%d", addr(dst), slen);
	}
	/* scan_dom to dom */
	else if (slen < dlen)
	{
		/* AAlength(scan_dom) < AAlength(dom) */
		fprintf(FILE_tmp, "AAdmove(%s,", addr(src));
		fprintf(FILE_tmp, "%s,%d,%d", addr(dst), slen, dlen - slen);
	}
	else
	{
		/* AAlength(scan_dom) >= AAlength(dom) */
		fprintf(FILE_tmp, "AAbmove(%s,", addr(src));
		fprintf(FILE_tmp, "%s,%d", addr(dst), dlen);
	}
	fprintf(FILE_tmp, ");\n");
}


static num_convert()
{
	register AMX_FRM	*src;
	register AMX_FRM	*dst;

	fprintf(FILE_tmp, "AA");
	switch ((src = &Source)->type)
	{
	  case opINT:
	  case opLONG:
		putc('i', FILE_tmp);
		break;

	  default:
		putc('f', FILE_tmp);
	}
	fprintf(FILE_tmp, "%d_", ctou(src->len));
	switch ((dst = &Destination)->type)
	{
	  case opINT:
	  case opLONG:
		putc('i', FILE_tmp);
		break;

	  default:
		putc('f', FILE_tmp);
	}
	fprintf(FILE_tmp, "%d(%s,", ctou(dst->len), addr(src));
	fprintf(FILE_tmp, "%s);\n", addr(dst));
}
