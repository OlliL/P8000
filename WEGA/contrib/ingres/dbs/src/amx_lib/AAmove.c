# include	"AA_liba.h"


/* scan_dom to dom */
AAdmove(src, dst, len, pad)
char		*src;
register char	*dst;
int		len;
register int	pad;
{
	dst = AAbmove(src, dst, len);

	while (--pad >= 0)
		*dst++ = ' ';
}


/* scan_dom to c_var */
AAvmove(src, dst, len)
register char	*src;
register char	*dst;
register int	len;
{
	src += len++;
	while (--len && *--src == ' ')
		continue;
	dst += len++;
	*dst = 0;
	while (--len)
		*--dst = *src--;
}
