/*
** ---  RG_CASE.C ------- CASE PROCESSOR  -------------------------------------
**
**	Version: @(#)rg_case.c		4.0	02/05/89
**
*/

# include	"rg_int.h"


/* |      |							*/
/* |r-code|							*/
/* |======|							*/
/* |prVAR |	entry:	R_rc points to &jumptable		*/
/* | var# |		(& = R-CODE offset)			*/
/* |------|							*/
/* |rcCASE|		R_val =	case#				*/
/* |------|							*/
/* |case# |	exit:	pointer	to R-CODE after	case		*/
/* |------|							*/
/* |&jmptb|	 values	after statment number			*/
/* |======|  +-+   +-+   +-+	 +-+   +-+   +-+		*/
/* |case 1|  |1|   |2|   |3|	 |4|   |5|   |6|		*/
/* |RETURN|  +-+   +-+   +-+	 +-+   +-+   +-+		*/
/* |------|							*/
/* |	..  |							*/
/* |------|							*/
/* |case i|<-------------------------R_rc			*/
/* |RETURN|							*/
/* |------|							*/
/* |	..  |							*/
/* |------|							*/
/* |case n|							*/
/* |RETURN|							*/
/* |------|		   [rc]---+				*/
/* | out  |			  |				*/
/* |RETURN|			  |				*/
/* |======|			|				*/
/*-|&case1|<-R_rc<-R_rc<-R_rc<--+-----jt			*/
/*j|------|  =jt   =jt   =jt	  |				*/
/*u|	..  |  =rc   =rc	  |				*/
/*m|------|  =df		  |				*/
/*p|&casei|<--------------rc<---rc  RC_WORD			*/
/*t|------|			  |   (rc+=sizeof (short))	*/
/*a|	..  |<--------------------+-----rc			*/
/*b|------|			  |				*/
/*l|&casen| if rc out	of range->|				*/
/*e|------|			  |				*/
/*-| &out |<--------df<---df<---df<---df			*/
/* |======|		      =[rc]				*/
/* | next |<--------------------+---------return		*/
/* |	    |							*/


char	*rg_case()
{
	register char		*rc;	/* R-CODE pointer                */
	register char		*jt;	/* pointer to case jump	table    */
	register char		*df;	/* pointer to default (out) case */
	register union anytype	*any;

/*1*/	df = R_rc;
	rc = df;
	jt = rc;

/*2*/	df += sizeof (short) * R_val;

	any = &Print->sy_val;
/*3*/	rc += sizeof (short) * (int) any->f8type;

	if (rc < jt || rc > df)
/*4*/		rc = df;

	RC_WORD;
/*5*/	R_rc = R_word + jt;	/* first read offset, and then update	!!! */

/*6*/	return (df + sizeof (short));

}
