# include	"../conf/gen.h"

struct MM_form
{
	struct MM_form	*f_next;		/* next field		*/
	struct MM_form	*f_prev;		/* previous field	*/
	int		(*f_func)();		/* field function	*/
	int		(*f_help)();		/* help function	*/
	char		*f_var;			/* field variable	*/
	char		*f_format;		/* format of variable	*/
	short		f_mode;			/* field specifications	*/
	short		f_line;			/* line no. of field	*/
	short		f_col;			/* column no. of field	*/
	short		f_len;			/* length of field	*/
	char		*f_field;		/* fieldbuffer		*/
};


	/* field specifications	*/
# define	F_START		020000		/* (first) start field	*/
# define	F_OUTPUT	000001		/* output field		*/
# define	F_MENUE		000002		/* menue  field		*/
# define	F_INPUT		000004		/* input  field		*/
# define	F_VALUE		000010		/* value  field		*/
# define	F_FIELD		000017		/* field type bits	*/
# define	F_NEED		000020		/* needed input field	*/
# define	F_SENDED	000040		/* input just given	*/
# define	F_FILLED	000100		/* in/output filled in	*/

# define	F_I1		000200		/* variable of type i1	*/
# define	F_I2		000400		/* variable of type i2	*/
# define	F_I4		001000		/* variable of type i4	*/
# define	F_F4		002000		/* variable of type f4	*/
# define	F_F8		004000		/* variable of type f8	*/
# define	F_C		010000		/* variable of type c	*/
# define	F_TYPE		(F_I1 | F_I2 | F_I4 | F_F4 | F_F8 | F_C)


	/* screen sequences */
# define	CS_CD		0		/* clear rest of screen	*/
# define	CS_CE		1		/* clear rest of line	*/
# define	CS_CM		2		/* move cursor		*/
# define	CS_SO		3		/* invers on		*/
# define	CS_SE		4		/* invers off		*/


	/* function keys */
# define	C_CONTINUE	-11		/* next record(s)	*/
# define	C_DELETE	-12		/* delete one char	*/
# define	C_FIELD		-13		/* cursor field begin	*/
# define	C_HELP		-14		/* select help		*/
# define	C_INSERT	-15		/* insert one char	*/
# define	C_LEFT		-16		/* cursor left		*/
# define	C_MENUE		-17		/* select menue		*/
# define	C_NEXT		-18		/* next field		*/
# define	C_PREV		-19		/* previous field	*/
# define	C_QUIT		-20		/* quit frame		*/
# define	C_RIGHT		-21		/* cursor right		*/


# ifndef SIGN_EXT
# define	AAi1toi2(val)	MM_i1toi2(val)
# endif


extern struct MM_form	*MM_need();
extern struct MM_form	*MMalloc();
extern char		*MM_cs[CS_SE + 1];
extern int		MM_cs_cm;
extern int		MMline;
extern int		MMcol;
