/*
**	J. Wenzlaff, ZKI der AdW
**
**	REDABAS 2 -- interface definitions
*/

/* REDABAS 2 version stamp */
/*define	REDABAS		2	   already defined		*/

/* HEADER definitions */
# define	DB_HDR		8	/* header length without fields	*/
# define	  DB_RECORDS	1	/* offset(short) record count	*/
# define	  DB_DAY	3	/* offset(char) day of last	*/
					/* 	modification		*/
# define	  DB_MONTH	4	/* offset(char) month of last	*/
					/* 	modification		*/
# define	  DB_YEAR	5	/* offset(char) year of last	*/
					/* 	modification		*/
# define	  DB_RECLEN	6	/* offset(short) record length	*/

/* FIELD definitions */
# define	DB_FLDMAX	32	/* max. no of fields		*/
# define	DB_FLDLEN	16	/* length of field description	*/
# define	  DB_F_NAME	0	/* offset(char [11]) field name	*/
# define	  DB_F_TYPE	11	/* offset(char) field type	*/
# define	  DB_F_LEN	12	/* offset(char) field length	*/
# define	  DB_F_DEC	15	/* offset(char) floating point	*/
					/* 	field length		*/
/* LENGTH of head record */
# define	DB_HEAD		(DB_HDR + (DB_FLDMAX * DB_FLDLEN) + 1)

/* OTHER stuff */
# define	DB_RECMAX	1000	/* max. record length		*/
typedef		short		word;
