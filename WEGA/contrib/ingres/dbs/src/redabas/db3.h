/*
**	J. Wenzlaff, ZKI der AdW
**
**	REDABAS 3 -- interface definitions
*/

/* REDABAS 3 version stamp */
/*define	REDABAS		3	   already defined		*/
# define	DB_MEMO		(REDABAS + 128)	/* with memo fields	*/

/* HEADER definitions */
# define	DB_HDR		32	/* header length without fields	*/
# define	  DB_RECORDS	4	/* offset(long) record count	*/
# define	  DB_DAY	3	/* offset(char) day of last	*/
					/* 	modification		*/
# define	  DB_MONTH	2	/* offset(char) month of last	*/
					/* 	modification		*/
# define	  DB_YEAR	1	/* offset(char) year of last	*/
					/* 	modification		*/
# define	  DB_RECLEN	10	/* offset(short) record length	*/
# define	  DB_HDRLEN	8	/* offset(short) header length	*/

/* FIELD definitions */
# define	DB_FLDMAX	128	/* max. no of fields		*/
# define	DB_FLDLEN	32	/* length of field description	*/
# define	  DB_F_NAME	0	/* offset(char [11]) field name	*/
# define	  DB_F_TYPE	11	/* offset(char) field type	*/
# define	  DB_F_LEN	16	/* offset(char) field length	*/
# define	  DB_F_DEC	17	/* offset(char) floating point	*/
					/* 	field length		*/

/* OTHER stuff */
# define	DB_RECMAX	4000	/* max. record length		*/
typedef		long		word;
