/*
** ---  RG_CODE.C ------- PRINT R_CODE MAME  ----------------------------------
**
**		FOR TEST PURPOSES ONLY
**
**	Version: @(#)rg_code.c		4.0	02/05/89
**
*/

# include	"rg.h"


# ifdef	RG_TRACE

# ifdef ENUM_INT
char	*Code[H_3_BYTES];
# else
char	*Code[(int) H_3_BYTES];
# endif

rg_code()
{
	/* INITIALIZE R_CODE NAMES */

	Code[(int) bdLT]	= "bdLT";
	Code[(int) bdEQ]	= "bdEQ";
	Code[(int) bdLE]	= "bdLE";
	Code[(int) bdGT]	= "bdGT";
	Code[(int) bdNE]	= "bdNE";
	Code[(int) bdGE]	= "bdGE";
	Code[(int) pushFALSE]	= "pushFALSE";
	Code[(int) pushTRUE]	= "pushTRUE";
	Code[(int) pushNULL]	= "pushNULL";
	Code[(int) push1INT]	= "push1INT";
	Code[(int) push2INT]	= "push2INT";
	Code[(int) push4INT]	= "push4INT";
	Code[(int) pushREAL]	= "pushREAL";
	Code[(int) pushSTRING]	= "pushSTRING";
	Code[(int) pushCONST]	= "pushCONST";
	Code[(int) pushCOL]	= "pushCOL";
	Code[(int) pushLINE]	= "pushLINE";
	Code[(int) pushALLINES]	= "pushALLINES";
	Code[(int) pushPAGE]	= "pushPAGE";
	Code[(int) pushTUPLE]	= "pushTUPLE";
	Code[(int) pushLAST]	= "pushLAST";
	Code[(int) pushVALUE]	= "pushVALUE";
	Code[(int) pushNEXT]	= "pushNEXT";
	Code[(int) pushVAR]	= "pushVAR";
	Code[(int) pushSWITCH]	= "pushSWITCH";
	Code[(int) rcNOT]	= "rcNOT";
	Code[(int) rcOR]	= "rcOR";
	Code[(int) rcAND]	= "rcAND";
	Code[(int) rcEND_SET]	= "rcEND_SET";
	Code[(int) rcMIN]	= "rcMIN";
	Code[(int) rcADD]	= "rcADD";
	Code[(int) rcSUB]	= "rcSUB";
	Code[(int) rcMUL]	= "rcMUL";
	Code[(int) rcDIV]	= "rcDIV";
	Code[(int) rcDIV_R]	= "rcDIV_R";
	Code[(int) rcMOD]	= "rcMOD";
	Code[(int) rcRC_END]	= "rcRC_END";
	Code[(int) rcRETURN]	= "rcRETURN";
	Code[(int) rcRTTS]	= "rcRTTS";
	Code[(int) rcEOTUP]	= "rcEOTUP";
	Code[(int) rcALL]	= "rcALL";
	Code[(int) rcONE]	= "rcONE";
	Code[(int) rcDATE]	= "rcDATE";
	Code[(int) rcTIME]	= "rcTIME";
	Code[(int) prCOL]	= "prCOL";
	Code[(int) prLINE]	= "prLINE";
	Code[(int) prALLINES]	= "prALLINES";
	Code[(int) prNEWPAGE]	= "prNEWPAGE";
	Code[(int) prPAGE]	= "prPAGE";
	Code[(int) prTUPLE]	= "prTUPLE";
	Code[(int) prVAR]	= "prVAR";
	Code[(int) prCONST]	= "prCONST";
	Code[(int) prSPACE]	= "prSPACE";
	Code[(int) prNEWLINE]	= "prNEWLINE";
	Code[(int) prNEWCOL]	= "prNEWCOL";
	Code[(int) prLAST]	= "prLAST";
	Code[(int) prVALUE]	= "prVALUE";
	Code[(int) prNEXT]	= "prNEXT";
	Code[(int) fmtB_BIT]	= "fmtB_BIT";
	Code[(int) fmtW_BIT]	= "fmtW_BIT";
	Code[(int) fmtB_OCT]	= "fmtB_OCT";
	Code[(int) fmtW_OCT]	= "fmtW_OCT";
	Code[(int) fmtB_DEC]	= "fmtB_DEC";
	Code[(int) fmtW_DEC]	= "fmtW_DEC";
	Code[(int) fmtB_HEX]	= "fmtB_HEX";
	Code[(int) fmtW_HEX]	= "fmtW_HEX";
	Code[(int) prSTRING]	= "prSTRING";
	Code[(int) prINPUT]	= "prINPUT";
	Code[(int) fmtINT]	= "fmtINT";
	Code[(int) fmtREAL]	= "fmtREAL";
	Code[(int) fmtCHAR]	= "fmtCHAR";
	Code[(int) fmtGRAPH]	= "fmtGRAPH";
	Code[(int) rcCASE]	= "rcCASE";
	Code[(int) rcREPEAT]	= "rcREPEAT";
	Code[(int) onBATCH]	= "onBATCH";
	Code[(int) onB_LINE]	= "onB_LINE";
	Code[(int) onE_LINE]	= "onE_LINE";
	Code[(int) onB_PAGE]	= "onB_PAGE";
	Code[(int) onE_PAGE]	= "onE_PAGE";
	Code[(int) rcJUMP]	= "rcJUMP";
	Code[(int) rcCALL]	= "rcCALL";
	Code[(int) setVAR]	= "setVAR";
	Code[(int) setSWITCH]	= "setSWITCH";
	Code[(int) rcIF]	= "rcIF";
	Code[(int) rcTHEN]	= "rcTHEN";
}

# endif
