/*
** ---  RC_CMAP.C ------- CHARACTER TYPE MAP  ---------------------------------
**
**	Version: @(#)rc_cmap.c		4.0	02/05/89
**
*/

# include	"rc.h"


char	Cmap[] =
{
	CNTRL,	CNTRL,	CNTRL,	CNTRL,	/* \0 ^A ^B ^C	*/
	CNTRL,	CNTRL,	CNTRL,	CNTRL,	/* ^D ^E ^F ^G	*/
	CNTRL,	PUNCT,	PUNCT,	CNTRL,	/* \b \t \n ^K	*/
	CNTRL,	PUNCT,	CNTRL,	CNTRL,	/* ^L \r ^N ^O	*/
	CNTRL,	CNTRL,	CNTRL,	CNTRL,	/* ^P ^Q ^R ^S	*/
	CNTRL,	CNTRL,	CNTRL,	CNTRL,	/* ^T ^U ^V ^W	*/
	CNTRL,	CNTRL,	CNTRL,	CNTRL,	/* ^X ^Y ^Z ^[	*/
	CNTRL,	CNTRL,	CNTRL,	CNTRL,	/* ^\ ^] ^^ ^_	*/
	PUNCT,	OPATR,	OPATR,	OPATR,	/*     !  "  #	*/
	OPATR,	OPATR,	OPATR,	OPATR,	/*  $  %  &  '	*/
	OPATR,	OPATR,	OPATR,	OPATR,	/*  (  )  *  +	*/
	OPATR,	OPATR,	OPATR,	OPATR,	/*  ,  -  .  /	*/
	NUMBR,	NUMBR,	NUMBR,	NUMBR,	/*  0  1  2  3	*/
	NUMBR,	NUMBR,	NUMBR,	NUMBR,	/*  4  5  6  7	*/
	NUMBR,	NUMBR,	OPATR,	OPATR,	/*  8  9  :  ;	*/
	OPATR,	OPATR,	OPATR,	OPATR,	/*  <  =  >  ?	*/
	OPATR,	ALPHA,	ALPHA,	ALPHA,	/*  @  A  B  C	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  D  E  F  G	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  H  I  J  K	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  L  M  N  O	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  P  Q  R  S	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  T  U  V  W	*/
	ALPHA,	ALPHA,	ALPHA,	OPATR,	/*  X  Y  Z  [	*/
	OPATR,	OPATR,	OPATR,	ALPHA,	/*  \  ]  ^  _	*/
	OPATR,	ALPHA,	ALPHA,	ALPHA,	/*  `  a  b  c	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  d  e  f  g	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  h  i  j  k	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  l  m  n  o	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  p  q  r  s	*/
	ALPHA,	ALPHA,	ALPHA,	ALPHA,	/*  t  u  v  w	*/
	ALPHA,	ALPHA,	ALPHA,	OPATR,	/*  x  y  z  {	*/
	OPATR,	OPATR,	OPATR,	CNTRL,	/*  |  }  ~ del	*/
	0
};
