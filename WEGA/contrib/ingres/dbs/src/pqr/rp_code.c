/*
** ---  RP_CODE.C ------- PRINT R_CODE NUMBER  --------------------------------
**
**	Version: @(#)rp_code.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"r_code.h"

# define	print(m, c)	printf("|%5x\t|%5d\t|%5o\t|\t%s\n", c, c, c, m); fflush(stdout)

main()
{
# ifdef SETBUF
	char	buf[BUFSIZ];

	setbuf(stdout, buf);
# endif

	PQR_system = "r_code printer";
	PQR();

	printf("R_CODE:\n");
	printf("--------------------------------------------\n");
	printf("|  hex\t|  dec\t|  oct\t|\tcode\n");
	printf("--------------------------------------------\n");
	print("bdLT", bdLT);
	print("bdEQ", bdEQ);
	print("bdLE", bdLE);
	print("bdGT", bdGT);
	print("bdNE", bdNE);
	print("bdGE", bdGE);
	print("pushFALSE", pushFALSE);
	print("pushTRUE", pushTRUE);
	print("pushNULL", pushNULL);
	print("push1INT", push1INT);
	print("push2INT", push2INT);
	print("push4INT", push4INT);
	print("pushREAL", pushREAL);
	print("pushSTRING", pushSTRING);
	print("pushCONST", pushCONST);
	print("pushCOL", pushCOL);
	print("pushLINE", pushLINE);
	print("pushALLINES", pushALLINES);
	print("pushPAGE", pushPAGE);
	print("pushTUPLE", pushTUPLE);
	print("pushLAST", pushLAST);
	print("pushVALUE", pushVALUE);
	print("pushNEXT", pushNEXT);
	print("pushVAR", pushVAR);
	print("pushSWITCH", pushSWITCH);
	print("rcNOT", rcNOT);
	print("rcOR", rcOR);
	print("rcAND", rcAND);
	print("rcEND_SET", rcEND_SET);
	print("rcMIN", rcMIN);
	print("rcADD", rcADD);
	print("rcSUB", rcSUB);
	print("rcMUL", rcMUL);
	print("rcDIV", rcDIV);
	print("rcDIV_R", rcDIV_R);
	print("rcMOD", rcMOD);
	print("rcRC_END", rcRC_END);
	print("rcRETURN", rcRETURN);
	print("rcRTTS", rcRTTS);
	print("rcEOTUP", rcEOTUP);
	print("rcALL", rcALL);
	print("rcONE", rcONE);
	print("rcDATE", rcDATE);
	print("rcTIME", rcTIME);
	print("prCOL", prCOL);
	print("prLINE", prLINE);
	print("prALLINES", prALLINES);
	print("prNEWPAGE", prNEWPAGE);
	print("prPAGE", prPAGE);
	print("prTUPLE", prTUPLE);
	print("prVAR", prVAR);
	print("prCONST", prCONST);
	print("prSPACE", prSPACE);
	print("prNEWLINE", prNEWLINE);
	print("prNEWCOL", prNEWCOL);
	print("prLAST", prLAST);
	print("prVALUE", prVALUE);
	print("prNEXT", prNEXT);
	print("fmtB_BIT", fmtB_BIT);
	print("fmtW_BIT", fmtW_BIT);
	print("fmtB_OCT", fmtB_OCT);
	print("fmtW_OCT", fmtW_OCT);
	print("fmtB_DEC", fmtB_DEC);
	print("fmtW_DEC", fmtW_DEC);
	print("fmtB_HEX", fmtB_HEX);
	print("fmtW_HEX", fmtW_HEX);
	print("prSTRING", prSTRING);
	print("prINPUT", prINPUT);
	print("fmtINT", fmtINT);
	print("fmtREAL", fmtREAL);
	print("fmtCHAR", fmtCHAR);
	print("fmtGRAPH", fmtGRAPH);
	print("rcCASE", rcCASE);
	print("rcREPEAT", rcREPEAT);
	print("onBATCH", onBATCH);
	print("onB_LINE", onB_LINE);
	print("onE_LINE", onE_LINE);
	print("onB_PAGE", onB_PAGE);
	print("onE_PAGE", onE_PAGE);
	print("rcJUMP", rcJUMP);
	print("rcCALL", rcCALL);
	print("setVAR", setVAR);
	print("setSWITCH", setSWITCH);
	print("rcIF", rcIF);
	print("rcTHEN", rcTHEN);
	printf("--------------------------------------------\n");
	printf("|  hex\t|  dec\t|  oct\t|\tcode\n");
	printf("--------------------------------------------\n");
	main1();
}

main1()
{
	printf("\fR_CODE:\n");
	printf("--------------------------------------------\n");
	printf("|  hex\t|  dec\t|  oct\t|\tcode\n");
	printf("--------------------------------------------\n");
	print("bdEQ", bdEQ);
	print("bdGE", bdGE);
	print("bdGT", bdGT);
	print("bdLE", bdLE);
	print("bdLT", bdLT);
	print("bdNE", bdNE);
	print("fmtB_BIT", fmtB_BIT);
	print("fmtB_DEC", fmtB_DEC);
	print("fmtB_HEX", fmtB_HEX);
	print("fmtB_OCT", fmtB_OCT);
	print("fmtCHAR", fmtCHAR);
	print("fmtGRAPH", fmtGRAPH);
	print("fmtINT", fmtINT);
	print("fmtREAL", fmtREAL);
	print("fmtW_BIT", fmtW_BIT);
	print("fmtW_DEC", fmtW_DEC);
	print("fmtW_HEX", fmtW_HEX);
	print("fmtW_OCT", fmtW_OCT);
	print("onBATCH", onBATCH);
	print("onB_LINE", onB_LINE);
	print("onB_PAGE", onB_PAGE);
	print("onE_LINE", onE_LINE);
	print("onE_PAGE", onE_PAGE);
	print("rcADD", rcADD);
	print("rcAND", rcAND);
	print("rcDIV", rcDIV);
	print("rcDIV_R", rcDIV_R);
	print("rcMIN", rcMIN);
	print("rcMOD", rcMOD);
	print("rcMUL", rcMUL);
	print("rcNOT", rcNOT);
	print("rcOR", rcOR);
	print("rcSUB", rcSUB);
	print("prCOL", prCOL);
	print("prCONST", prCONST);
	print("prINPUT", prINPUT);
	print("prLAST", prLAST);
	print("prLINE", prLINE);
	print("prALLINES", prALLINES);
	print("prNEWCOL", prNEWCOL);
	print("prNEWLINE", prNEWLINE);
	print("prNEWPAGE", prNEWPAGE);
	print("prNEXT", prNEXT);
	print("prPAGE", prPAGE);
	print("prSPACE", prSPACE);
	print("prSTRING", prSTRING);
	print("prTUPLE", prTUPLE);
	print("prVALUE", prVALUE);
	print("prVAR", prVAR);
	print("push1INT", push1INT);
	print("push2INT", push2INT);
	print("push4INT", push4INT);
	print("pushALLINES", pushALLINES);
	print("pushCOL", pushCOL);
	print("pushCONST", pushCONST);
	print("pushFALSE", pushFALSE);
	print("pushLAST", pushLAST);
	print("pushLINE", pushLINE);
	print("pushNEXT", pushNEXT);
	print("pushNULL", pushNULL);
	print("pushPAGE", pushPAGE);
	print("pushREAL", pushREAL);
	print("pushSTRING", pushSTRING);
	print("pushSWITCH", pushSWITCH);
	print("pushTRUE", pushTRUE);
	print("pushTUPLE", pushTUPLE);
	print("pushVALUE", pushVALUE);
	print("pushVAR", pushVAR);
	print("rcALL", rcALL);
	print("rcCALL", rcCALL);
	print("rcCASE", rcCASE);
	print("rcDATE", rcDATE);
	print("rcEND_SET", rcEND_SET);
	print("rcEOTUP", rcEOTUP);
	print("rcIF", rcIF);
	print("rcJUMP", rcJUMP);
	print("rcONE", rcONE);
	print("rcRC_END", rcRC_END);
	print("rcREPEAT", rcREPEAT);
	print("rcRETURN", rcRETURN);
	print("rcRTTS", rcRTTS);
	print("rcTHEN", rcTHEN);
	print("rcTIME", rcTIME);
	print("setSWITCH", setSWITCH);
	print("setVAR", setVAR);
	printf("--------------------------------------------\n");
	printf("|  hex\t|  dec\t|  oct\t|\tcode\n");
	printf("--------------------------------------------\n");
	fflush(stdout);
}
