# include	"../conf/gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/* alignment for char * */
struct ptr
{
	char		d_ptr;
	char		*s_ptr;
}	v_ptr;
struct
{
	char		da_ptr;
	char		*sa_ptr[2];
}	va_ptr;

/* alignment for char */
struct
{
	char		d_char;
	char		s_char;
}	v_char;
struct
{
	char		da_char;
	char		sa_char[2];
}	va_char;

/* alignment for short */
struct
{
	char		d_short;
	short		s_short;
}	v_short;
struct
{
	char		da_short;
	short		sa_short[2];
}	va_short;

/* alignment for int */
struct
{
	char		d_int;
	int		s_int;
}	v_int;
struct
{
	char		da_int;
	int		sa_int[2];
}	va_int;

/* alignment for long */
struct
{
	char		d_long;
	long		s_long;
}	v_long;
struct
{
	char		da_long;
	long		sa_long[2];
}	va_long;

/* alignment for float */
struct
{
	char		d_float;
	float		s_float;
}	v_float;
struct
{
	char		da_float;
	float		sa_float[2];
}	va_float;

/* alignment for double */
struct
{
	char		d_double;
	double		s_double;
}	v_double;
struct
{
	char		da_double;
	double		sa_double[2];
}	va_double;

/* alignment for struct */
struct
{
	char		d_struct;
	struct ptr	s_struct;
}	v_struct;
struct
{
	char		da_struct;
	struct ptr	sa_struct[2];
}	va_struct;

AAdumptid(tid)
register struct tup_id	*tid;
{
	long			pageid;

	AApluck_page(tid, &pageid);
	printf("tid: %08lx/%04x\n", pageid, ctou(tid->line_id));
}

AApluck_page(tid, var)
register struct tup_id	*tid;
register struct lpage	*var;
{
	var->lpg0 = tid->pg0;
	var->lpg1 = tid->pg1;
	var->lpg2 = tid->pg2;
	var->lpgx = 0;
}

main()
{
	char	sign;
	long	page;

	printf("SIZEOF_TEST\n");
	printf("char\t%d\n", sizeof (char));
	printf("char *\t%d\n", sizeof (char *));
	printf("short\t%d\n", sizeof (short));
	printf("int\t%d\n", sizeof (int));
	printf("long\t%d\n", sizeof (long));
	printf("float\t%d\n", sizeof (float));
	printf("double\t%d\n", sizeof (double));

	printf("SIGN_EXT_TEST\n");
	sign = '\377';
	printf("sign\t%x\n", sign);

	printf("ALIGNMENT_TEST\n");
	printf("type\telement\tarray\n");
	printf("------------------------\n");
	printf("ptr\t%d\t%d\n",	((char *) &v_ptr.s_ptr) - ((char *) &v_ptr),
				((char *) va_ptr.sa_ptr) - ((char *) &va_ptr));
	printf("char\t%d\t%d\n",	((char *) &v_char.s_char) - ((char *) &v_char),
				((char *) va_char.sa_char) - ((char *) &va_char));
	printf("short\t%d\t%d\n",	((char *) &v_short.s_short) - ((char *) &v_short),
				((char *) va_short.sa_short) - ((char *) &va_short));
	printf("int\t%d\t%d\n",	((char *) &v_int.s_int) - ((char *) &v_int),
				((char *) va_int.sa_int) - ((char *) &va_int));
	printf("long\t%d\t%d\n",	((char *) &v_long.s_long) - ((char *) &v_long),
				((char *) va_long.sa_long) - ((char *) &va_long));
	printf("float\t%d\t%d\n",	((char *) &v_float.s_float) - ((char *) &v_float),
				((char *) va_float.sa_float) - ((char *) &va_float));
	printf("double\t%d\t%d\n",	((char *) &v_double.s_double) - ((char *) &v_double),
				((char *) va_double.sa_double) - ((char *) &va_double));
	printf("struct\t%d\t%d\n",	((char *) &v_struct.s_struct) - ((char *) &v_struct),
				((char *) va_struct.sa_struct) - ((char *) &va_struct));
	printf("------------------------\n");
	printf("PAGE %010lx ", page = 0x00000000L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0x000000ffL);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0x0000ff00L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0x0000ffffL);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0x00ff0000L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0x00ff00ffL);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0x00ffff00L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0x00ffffffL);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xff000000L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xff0000ffL);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xff00ff00L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xff00ffffL);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xffff0000L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xffff00ffL);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xffffff00L);
	AAdumptid(&page);
	printf("PAGE %010lx ", page = 0xffffffffL);
	AAdumptid(&page);
}
