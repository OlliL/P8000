/*
** ---  RP_ARG.C -------- R_CODE ARGUMENT PRINTER  ----------------------------
**
**	Version: @(#)rp_arg.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"r_code.h"

# define	NEW_LINE	putchar('\n'); fflush(stdout)

i1(msg)
register char	*msg;
{
  printf(" %s=int1", msg);
}


i2(msg)
register char	*msg;
{
  printf(" %s=int2", msg);
}


i4(msg)
register char	*msg;
{
  printf(" %s=long", msg);
}


f8(msg)
register char	*msg;
{
  printf(" %s=double", msg);
}


c0(msg, len)
register char	*msg;
register char	*len;
{
  printf(" %s=char[%s]", msg, len);
}


var(msg)
register char	*msg;
{
  printf(" %s=var", msg);
}


adr(msg)
register char	*msg;
{
  printf(" %s=rel_addr", msg);
}


main()
{
# ifdef SETBUF
  char	buf[BUFSIZ];

  setbuf(stdout, buf);
# endif

  PQR_system = "r_code argument printer";
  PQR();

  printf("R_CODE:\n");
  printf("-------\n");
  printf(" octal\n");
  printf("-------\n");

  printf("|%4o\tbdLT", bdLT); NEW_LINE;
  printf("|%4o\tbdEQ", bdEQ); NEW_LINE;
  printf("|%4o\tbdLE", bdLE); NEW_LINE;
  printf("|%4o\tbdGT", bdGT); NEW_LINE;
  printf("|%4o\tbdNE", bdNE); NEW_LINE;
  printf("|%4o\tbdGE", bdGE); NEW_LINE;
  printf("|%4o\tpushFALSE", pushFALSE); NEW_LINE;
  printf("|%4o\tpushTRUE", pushTRUE); NEW_LINE;
  printf("|%4o\tpushNULL", pushNULL); NEW_LINE;
  printf("|%4o\tpush1INT", push1INT); i1("value"); NEW_LINE;
  printf("|%4o\tpush2INT", push2INT); i2("value"); NEW_LINE;
  printf("|%4o\tpush4INT", push4INT); i4("value"); NEW_LINE;
  printf("|%4o\tpushREAL", pushREAL); f8("value"); NEW_LINE;
  printf("|%4o\tpushSTRING", pushSTRING); var("length"); c0("string", "length"); NEW_LINE;
  printf("|%4o\tpushCONST", pushCONST); i1("const"); NEW_LINE;
  printf("|%4o\tpushCOL", pushCOL); NEW_LINE;
  printf("|%4o\tpushLINE", pushLINE); NEW_LINE;
  printf("|%4o\tpushALLINES", pushALLINES); NEW_LINE;
  printf("|%4o\tpushPAGE", pushPAGE); NEW_LINE;
  printf("|%4o\tpushTUPLE", pushTUPLE); NEW_LINE;
  printf("|%4o\tpushLAST", pushLAST); var("dom"); var("from"); var("to"); NEW_LINE;
  printf("|%4o\tpushVALUE", pushVALUE); var("dom"); var("from"); var("to"); NEW_LINE;
  printf("|%4o\tpushNEXT", pushNEXT); var("dom"); var("from"); var("to"); NEW_LINE;
  printf("|%4o\tpushVAR", pushVAR); var("#"); NEW_LINE;
  printf("|%4o\tpushSWITCH", pushSWITCH); var("#"); NEW_LINE;
  printf("|%4o\trcNOT", rcNOT); NEW_LINE;
  printf("|%4o\trcOR", rcOR); NEW_LINE;
  printf("|%4o\trcAND", rcAND); NEW_LINE;
  printf("|%4o\trcEND_SET", rcEND_SET); NEW_LINE;
  printf("|%4o\trcMIN", rcMIN); NEW_LINE;
  printf("|%4o\trcADD", rcADD); NEW_LINE;
  printf("|%4o\trcSUB", rcSUB); NEW_LINE;
  printf("|%4o\trcMUL", rcMUL); NEW_LINE;
  printf("|%4o\trcDIV", rcDIV); NEW_LINE;
  printf("|%4o\trcDIV_R", rcDIV_R); NEW_LINE;
  printf("|%4o\trcMOD", rcMOD); NEW_LINE;
  printf("|%4o\trcRC_END", rcRC_END); NEW_LINE;
  printf("|%4o\trcRETURN", rcRETURN); NEW_LINE;
  printf("|%4o\trcRTTS", rcRTTS); NEW_LINE;
  printf("|%4o\trcEOTUP", rcEOTUP); NEW_LINE;
  printf("|%4o\trcALL", rcALL); NEW_LINE;
  printf("|%4o\trcONE", rcONE); NEW_LINE;
  printf("|%4o\trcDATE", rcDATE); NEW_LINE;
  main1();
}

main1()
{
  printf("|%4o\trcTIME", rcTIME); NEW_LINE;
  printf("|%4o\tprCOL", prCOL); NEW_LINE;
  printf("|%4o\tprLINE", prLINE); NEW_LINE;
  printf("|%4o\tprALLINES", prALLINES); NEW_LINE;
  printf("|%4o\tprNEWPAGE", prNEWPAGE); NEW_LINE;
  printf("|%4o\tprPAGE", prPAGE); NEW_LINE;
  printf("|%4o\tprTUPLE", prTUPLE); NEW_LINE;
  printf("|%4o\tprVAR", prVAR); var("#"); NEW_LINE;
  printf("|%4o\tprCONST", prCONST); i2("const"); NEW_LINE;
  printf("|%4o\tprSPACE", prSPACE); var("#"); NEW_LINE;
  printf("|%4o\tprNEWLINE", prNEWLINE); var("#"); NEW_LINE;
  printf("|%4o\tprNEWCOL", prNEWCOL); var("col"); NEW_LINE;
  printf("|%4o\tprLAST", prLAST); var("dom"); NEW_LINE;
  printf("|%4o\tprVALUE", prVALUE); var("dom"); NEW_LINE;
  printf("|%4o\tprNEXT", prNEXT); var("dom"); NEW_LINE;
  printf("|%4o\tfmtB_BIT", fmtB_BIT); var("length"); NEW_LINE;
  printf("|%4o\tfmtW_BIT", fmtW_BIT); var("length"); NEW_LINE;
  printf("|%4o\tfmtB_OCT", fmtB_OCT); var("length"); NEW_LINE;
  printf("|%4o\tfmtW_OCT", fmtW_OCT); var("length"); NEW_LINE;
  printf("|%4o\tfmtB_DEC", fmtB_DEC); var("length"); NEW_LINE;
  printf("|%4o\tfmtW_DEC", fmtW_DEC); var("length"); NEW_LINE;
  printf("|%4o\tfmtB_HEX", fmtB_HEX); var("length"); NEW_LINE;
  printf("|%4o\tfmtW_HEX", fmtW_HEX); var("length"); NEW_LINE;
  printf("|%4o\tprSTRING", prSTRING); var("length"); var("batch_length"); c0("output", "length"); NEW_LINE;
  printf("|%4o\tprINPUT", prINPUT); var("length"); var("batch_length"); c0("output", "length"); NEW_LINE;
  printf("|%4o\tfmtINT", fmtINT); var("length"); f8("scale"); i2("\n|\t\tpic_length"); c0("pic_string", "pic_length"); NEW_LINE;
  printf("|%4o\tfmtREAL", fmtREAL); var("length"); f8("scale"); var("before"); var("point"); var("ignore"); var("norm"); i2("\n|\t\texp_length"); c0("exp_string", "exp_length"); NEW_LINE;
  printf("|%4o\tfmtCHAR", fmtCHAR); var("length"); var("from"); var("to"); var("batch_length"); NEW_LINE;
  printf("|%4o\tfmtGRAPH", fmtGRAPH); var("length"); var("from"); f8("value"); var("to"); f8("value"); i1("with"); NEW_LINE;
  printf("|%4o\trcCASE", rcCASE); var("#"); adr("jumptable"); NEW_LINE;
  printf("|%4o\trcREPEAT", rcREPEAT); var("#"); adr("jump to"); NEW_LINE;
  printf("|%4o\tonBATCH", onBATCH); adr("jump over to"); NEW_LINE;
  printf("|%4o\tonB_LINE", onB_LINE); adr("jump over to"); NEW_LINE;
  printf("|%4o\tonE_LINE", onE_LINE); adr("jump over to"); NEW_LINE;
  printf("|%4o\tonB_PAGE", onB_PAGE); adr("jump over to"); NEW_LINE;
  printf("|%4o\tonE_PAGE", onE_PAGE); adr("jump over to"); NEW_LINE;
  printf("|%4o\trcJUMP", rcJUMP); adr("to"); NEW_LINE;
  printf("|%4o\trcCALL", rcCALL); adr("procedure on"); NEW_LINE;
  printf("|%4o\tsetVAR", setVAR); adr("end of expression on"); NEW_LINE;
  printf("|%4o\tsetSWITCH", setSWITCH); adr("end of expression on"); NEW_LINE;
  printf("|%4o\trcIF", rcIF); adr("then on"); NEW_LINE;
  printf("|%4o\trcTHEN", rcTHEN); adr("else on"); NEW_LINE;

  printf("-------\n");
  fflush(stdout);
}
