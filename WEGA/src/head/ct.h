/* ct.h */

/*
 * cartridge tape device header file
 */

#define	CTIOFF	(('c'<<8)|0)	/* seek forward file */
#define	CTIOFR	(('c'<<8)|1)	/* seek reverse file */
#define	CTIOBF	(('c'<<8)|2)	/* seek forward block */
#define	CTIOBR	(('c'<<8)|3)	/* seek reverse block */
