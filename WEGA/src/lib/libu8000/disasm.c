/*
 * disasm.c: disasm, disinit (disasm(3)), the U8000 disassembler of /usr/lib/libu8000.a and slibu8000.a.
 * No source is known: written from the members (Library.SC: disasm.c 1.2, 01/29/83); built by ccv1 and
 * sccv1 without -O. The names of the static variables and functions are not in the object.
 */
#include	<disasm.h>

struct optab {
	char	*o_name;
	int	o_fmt;
};

static int	segmode;
static unsigned	(*getfunc)();
static int	(*symfunc)();
static char	opcode[18];
static char	operand[4][30];
static char	comment[20];
static int	size;
static int	space;
static int	symkind;
static char	*opp;
static char	*opp2;
static char	bad;
static char	fpbad;
static unsigned	word;
static unsigned	word2;
static int	word3;
static int	mode;
static int	fpop;
static int	fpflags;
static char	fmtb[] = "#%%%02x";
static char	fmtw[] = "#%%%04x";
static char	fmtl[] = "#%%%04x%04x";
static char	fmta[] = "%%%04x";
static ADDR	addr;
static char	n_cpi[] = "cpi";
static char	n_rldb[] = "rldb";
static char	n_trib[] = "trib";
static char	n_trtib[] = "trtib";
static char	n_trtirb[] = "trtirb";
static char	n_trdb[] = "trdb";
static char	n_trdrb[] = "trdrb";
static char	n_trtdrb[] = "trtdrb";
static char	n_trirb[] = "trirb";
static char	n_trtdb[] = "trtdb";
static char	n_ldi[] = "ldi";
static char	n_adcb[] = "adcb";
static char	n_adc[] = "adc";
static char	n_cpir[] = "cpir";
static char	n_ldir[] = "ldir";
static char	n_cpsi[] = "cpsi";
static char	n_cpsir[] = "cpsir";
static char	n_cpd[] = "cpd";
static char	n_ldd[] = "ldd";
static char	n_lddr[] = "lddr";
static char	n_cpsd[] = "cpsd";
static char	n_cpsdr[] = "cpsdr";
static char	n_cpdr[] = "cpdr";
static char	n_inb[] = "inb";
static char	n_ei[] = "ei";
static char	n_di[] = "di";
static char	n_rrdb[] = "rrdb";
static char	n_ldctl[] = "ldctl";
static char	n_ldk[] = "ldk";
static char	n_outb[] = "outb";
static char	n_sc[] = "sc";
static char	n_calr[] = "calr";
static char	n_jr[] = "jr";
static char	n_djnzb[] = "djnzb";
static char	n_djnz[] = "djnz";
static char	n_addb[] = "addb";
static char	n_addl[] = "addl";
static char	n_add[] = "add";
static char	n_andb[] = "andb";
static char	n_and[] = "and";
static char	n_bitb[] = "bitb";
static char	n_bit[] = "bit";
static char	n_call[] = "call";
static char	n_clrb[] = "clrb";
static char	n_clr[] = "clr";
static char	n_comb[] = "comb";
static char	n_comflg[] = "comflg";
static char	n_com[] = "com";
static char	n_cpb[] = "cpb";
static char	n_cpl[] = "cpl";
static char	n_cp[] = "cp";
static char	n_dab[] = "dab";
static char	n_decb[] = "decb";
static char	n_dec[] = "dec";
static char	n_divl[] = "divl";
static char	n_div[] = "div";
static char	n_exb[] = "exb";
static char	n_ex[] = "ex";
static char	n_extsb[] = "extsb";
static char	n_extsl[] = "extsl";
static char	n_exts[] = "exts";
static char	n_halt[] = "halt";
static char	n_incb[] = "incb";
static char	n_inc[] = "inc";
static char	n_iret[] = "iret";
static char	n_inir[] = "inir";
static char	n_ini[] = "ini";
static char	n_indr[] = "indr";
static char	n_ind[] = "ind";
static char	n_in[] = "in";
static char	n_jp[] = "jp";
static char	n_lda[] = "lda";
static char	n_ldar[] = "ldar";
static char	n_ldb[] = "ldb";
static char	n_ldctlb[] = "ldctlb";
static char	n_ldl[] = "ldl";
static char	n_ldm[] = "ldm";
static char	n_ld[] = "ld";
static char	n_ldps[] = "ldps";
static char	n_ldrb[] = "ldrb";
static char	n_ldrl[] = "ldrl";
static char	n_ldr[] = "ldr";
static char	n_multl[] = "multl";
static char	n_mult[] = "mult";
static char	n_mset[] = "mset";
static char	n_mres[] = "mres";
static char	n_mbit[] = "mbit";
static char	n_mreq[] = "mreq";
static char	n_negb[] = "negb";
static char	n_neg[] = "neg";
static char	n_nop[] = "nop";
static char	n_none[] = "";
static char	n_orb[] = "orb";
static char	n_or[] = "or";
static char	n_outi[] = "outi";
static char	n_out[] = "out";
static char	n_otir[] = "otir";
static char	n_otdr[] = "otdr";
static char	n_outd[] = "outd";
static char	n_popl[] = "popl";
static char	n_pop[] = "pop";
static char	n_pushl[] = "pushl";
static char	n_push[] = "push";
static char	n_resb[] = "resb";
static char	n_resflg[] = "resflg";
static char	n_res[] = "res";
static char	n_ret[] = "ret";
static char	n_rlb[] = "rlb";
static char	n_rrb[] = "rrb";
static char	n_rlcb[] = "rlcb";
static char	n_rrcb[] = "rrcb";
static char	n_sbcb[] = "sbcb";
static char	n_sbc[] = "sbc";
static char	n_sllb[] = "sllb";
static char	n_sdlb[] = "sdlb";
static char	n_slll[] = "slll";
static char	n_sdll[] = "sdll";
static char	n_slab[] = "slab";
static char	n_sdab[] = "sdab";
static char	n_slal[] = "slal";
static char	n_sdal[] = "sdal";
static char	n_setb[] = "setb";
static char	n_setflg[] = "setflg";
static char	n_set[] = "set";
static char	n_subb[] = "subb";
static char	n_subl[] = "subl";
static char	n_sub[] = "sub";
static char	n_tccb[] = "tccb";
static char	n_tcc[] = "tcc";
static char	n_testb[] = "testb";
static char	n_testl[] = "testl";
static char	n_test[] = "test";
static char	n_tsetb[] = "tsetb";
static char	n_tset[] = "tset";
static char	n_xorb[] = "xorb";
static char	n_xor[] = "xor";
static char	n_flags[] = "flags";
static struct optab	optab[] = {
	n_addb, 1,
	n_add, 2,
	n_subb, 1,
	n_sub, 2,
	n_orb, 1,
	n_or, 2,
	n_andb, 1,
	n_and, 2,
	n_xorb, 1,
	n_xor, 2,
	n_cpb, 1,
	n_cp, 2,
	n_none, 0,
	n_none, 0,
	n_none, 5,
	n_none, 5,
	n_cpl, 3,
	n_pushl, 3,
	n_subl, 3,
	n_push, 2,
	n_ldl, 3,
	n_popl, 3,
	n_addl, 3,
	n_pop, 2,
	n_multl, 4,
	n_mult, 3,
	n_divl, 4,
	n_div, 3,
	n_none, 2,
	n_ldl, 3,
	n_none, 0,
	n_call, 0,
	n_ldb, 1,
	n_ld, 2,
	n_resb, 1,
	n_res, 2,
	n_setb, 1,
	n_set, 2,
	n_bitb, 1,
	n_bit, 2,
	n_incb, 1,
	n_inc, 2,
	n_decb, 1,
	n_dec, 2,
	n_exb, 1,
	n_ex, 2,
	n_none, 1,
	n_none, 2,
	n_none, 1,
	n_none, 2,
	n_none, 1,
	n_none, 2,
	n_none, 2,
	n_none, 2,
	n_none, 2,
	n_none, 2,
	n_none, 1,
	n_ldps, 2,
	n_none, 1,
	n_none, 2,
	n_none, 1,
	n_none, 2,
	n_none, 1,
	n_none, 2,
};
static struct optab	op8d[] = {
	n_comb, 1,
	n_cpb, 1,
	n_negb, 1,
	n_none, 1,
	n_testb, 1,
	n_ldb, 1,
	n_tsetb, 1,
	n_none, 0,
	n_clrb, 1,
	n_ldctlb, 1,
	n_ldctlb, 1,
};
static struct optab	opcd[] = {
	n_com, 2,
	n_none, 2,
	n_neg, 2,
	n_resflg, 2,
	n_test, 2,
	n_ld, 2,
	n_tset, 2,
	n_nop, 0,
	n_clr, 2,
	n_push, 2,
	n_setflg, 2,
	n_comflg, 2,
};
static struct optab	opshift[] = {
	n_rlb, 1,
	n_sllb, 1,
	n_rlb, 1,
	n_sdlb, 1,
	n_rrb, 1,
	n_slll, 3,
	n_rrb, 1,
	n_sdll, 3,
	n_rlcb, 1,
	n_slab, 1,
	n_rlcb, 1,
	n_sdab, 1,
	n_rrcb, 1,
	n_slal, 3,
	n_rrcb, 1,
	n_sdal, 3,
};
static struct optab	optrans[] = {
	n_trib, 1,
	n_trtib, 1,
	n_trirb, 1,
	n_trtirb, 1,
	n_trdb, 1,
	n_trtdb, 1,
	n_trdrb, 1,
	n_trtdrb, 1,
};
static struct optab	fptab[] = {
	"fclr", 2,
	"fcpz", 2,
	"fcpzx", 2,
	"fexm", 2,
	"dum", 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	"fcpf", 2,
	"fcpfx", 2,
	"fcp", 2,
	"fcpx", 2,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	"fadd", 2,
	"fsub", 2,
	"fdiv", 2,
	"fmul", 2,
	"fremstep", 2,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	"fld", 2,
	"fldbcd", 2,
	"fldi", 0x8003,
	"fldm", 2,
	"fldt", 0x8003,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	"fabs", 2,
	"fneg", 2,
	n_none, 0,
	n_none, 0,
	"fsqr", 2,
	"fint", 2,
	"fnorm", 2,
	n_none, 0,
	"fldctl", 8195,
	"fldctlb", 0,
	"fresflg", 0,
	"fsetflg", 0,
	"fsetmode", 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
	n_none, 0,
};
static char	*flgnam[] = {
	"ouflg", "invflg", "intflg", "sysflg",
	"cmpflg",
};
static char	*ctlnam[] = {
	"scon", "pc1", "pc2", "traps",
	"fflags", "user", "fop1", "fop2",
};
static char	*sysnam[] = {
	"sysiret", "exit", "fork", "read",
	"write", "open", "close", "wait",
	"creat", "link", "unlink", "exec",
	"chdir", "time", "mknod", "chmod",
	"chown", "break", "stat", "seek",
	"getpid", "mount", "umount", "setuid",
	"getuid", "stime", "ptrace", "alarm",
	"fstat", "pause", "utime", "stty",
	"gtty", "access", "nice", "ftime",
	"sync", "kill", "switch", "setpgrp",
	"tell", "dup", "pipe", "times",
	"prof", "unused", "setgid", "getgid",
	"sig", "reserved", "reserved", "sysacct",
	"sysphys", "syslock", "ioctl", "unused",
	"mpxchan", "reserved", "reserved", "exece",
	"umask", "chroot", "unused", "reserved",
};
static char	*ccnam[] = {
	"f", "lt", "le", "ule",
	"ov/pe", "mi", "z/eq", "c/ult",
	"", "ge", "gt", "ugt",
	"nov/po", "pl", "nz/ne", "nc/uge",
};

static
imm(n, flag)
register int flag, n;
{
	if (n != 1 || flag == 0)
		sprintf(operand[1], "#%d", n);
	opp = operand[0];
	addrmode();
}

static
ccop()
{
	strcpy(operand[0], ccnam[word & 017]);
	opp = operand[1];
	addrmode();
}

static
regimm()
{
	opp = operand[0];
	reg(word);
	immed();
}

static
immed()
{
	opp = operand[1];
	if (mode != 0 || (word & 0360) != 0)
		addrmode();
	else {
		word2 = (*getfunc)(&addr, space);
		addr.a_offset += 2;
		switch (size) {
		case 1:
			sprintf(opp, fmtb, word2 & 0377);
			break;
		case 2:
			sprintf(opp, fmtw, word2);
			break;
		case 3:
			word3 = (*getfunc)(&addr, space);
			addr.a_offset += 2;
			sprintf(opp, fmtl, word2, word3);
			break;
		default:
			bad = 1;
		}
	}
}

static
addrreg()
{
	opp = operand[1];
	reg(word);
	opp = operand[0];
	addrmode();
}

static
regbx()
{
	opp = operand[0];
	reg(word);
	opp = operand[1];
	bx();
}

static
bx()
{
	if (segmode == 1)
		lreg(word >> 4);
	else
		wreg(word >> 4);
	word2 = (*getfunc)(&addr, space);
	addr.a_offset += 2;
	*opp++ = '(';
	wreg(word2 >> 8);
	*opp++ = ')';
}

static
regba()
{
	opp = operand[0];
	reg(word);
	opp = operand[1];
	ba();
}

static
ba()
{
	ADDR	a;

	word2 = (*getfunc)(&addr, space);
	addr.a_offset += 2;
	if ((word & 0360) == 0) {
		a.a_offset = (int)word2 + addr.a_offset;
		a.a_segno = addr.a_segno;
		(*symfunc)(&a, symkind, opp);
	} else {
		if (segmode == 1)
			lreg(word >> 4);
		else
			wreg(word >> 4);
		sprintf(opp, "(#%%%04x)", word2);
	}
}

static
addrimm()
{
	opp = operand[0];
	addrmode();
	imm16();
}

static
imm16()
{
	word2 = (*getfunc)(&addr, space);
	addr.a_offset += 2;
	if (size == 1)
		sprintf(operand[1], fmtb, word2 & 0377);
	else
		sprintf(operand[1], fmtw, word2);
}

static
ldm()
{
	word2 = (*getfunc)(&addr, space);
	addr.a_offset += 2;
	wreg(word2 >> 8);
	sprintf(operand[2], "#%d", (word2 & 017) + 1);
	opp = opp2;
	addrmode();
}

static
ireg2(r)
register unsigned r;
{
	ireg(r >> 4);
	opp = operand[2];
	wreg(word2 >> 8);
}

static
flags()
{
	opp = operand[0];
	if ((word & 0200) != 0) {
		*opp++ = 'c';
		*opp++ = ',';
	}
	if ((word & 0100) != 0) {
		*opp++ = 'z';
		*opp++ = ',';
	}
	if ((word & 040) != 0) {
		*opp++ = 's';
		*opp++ = ',';
	}
	if ((word & 020) != 0) {
		*opp++ = 'p';
		*opp++ = '/';
		*opp++ = 'v';
		opp++;
	}
	*--opp = '\0';
}

static
ldiw()
{
	register int t;

	ireg(word >> 4);
	opp = opp2;
	t = word;
	word <<= 4;
	word = word & 037777 ^ t & 0140000;
	addrmode();
}

static
reg(r)
register int r;
{
	switch (size) {
	case 1:
		breg(r);
		break;
	case 2:
		wreg(r);
		break;
	case 3:
		lreg(r);
		break;
	case 4:
		qreg(r);
		break;
	default:
		bad = 1;
	}
}

static
breg(r)
register int r;
{
	*opp++ = 'r';
	if ((r & 010) == 0)
		*opp++ = 'h';
	else
		*opp++ = 'l';
	*opp++ = (r & 07) + '0';
}

static
wreg(r)
register int r;
{
	register unsigned n;

	if ((n = r & 017) == 017) {
		*opp++ = 's';
		*opp++ = 'p';
	} else {
		*opp++ = 'r';
		if (n > 9) {
			*opp++ = '1';
			n -= 10;
		}
		*opp++ = n + '0';
	}
}

static
lreg(r)
register int r;
{
	if ((r & 1) != 0)
		bad = 1;
	else {
		*opp++ = 'r';
		wreg(r);
	}
}

static
qreg(r)
register int r;
{
	register unsigned n;

	if ((r & 3) != 0)
		bad = 1;
	else {
		*opp++ = 'r';
		*opp++ = 'q';
		if ((n = r & 017) > 9) {
			*opp++ = '1';
			n -= 10;
		}
		*opp++ = n + '0';
	}
}

static
addrmode()
{
	switch (mode) {
	case 0:
		ireg(word >> 4);
		break;
	case 1:
		da();
		if ((word & 0360) != 0) {
			while (*opp)
				opp++;
			*opp++ = '(';
			wreg(word >> 4);
			*opp++ = ')';
		}
		break;
	case 2:
		reg(word >> 4);
		break;
	default:
		bad = 1;
	}
}

static
ireg(r)
register int r;
{
	*opp++ = '@';
	if (segmode == 1)
		lreg(r);
	else
		wreg(r);
}

static
da()
{
	ADDR	a;

	word2 = (*getfunc)(&addr, space);
	addr.a_offset += 2;
	if (segmode == 0) {
		a.a_segno = 0;
		a.a_offset = (unsigned)word2;
		(*symfunc)(&a, symkind, opp);
	} else {
		a.a_segno = (word2 & 077400) >> 8;
		if ((word2 & 0100000) == 0) {
			a.a_offset = (unsigned)(word2 & 0377);
			*opp++ = '|';
			(*symfunc)(&a, symkind, opp);
			while (*opp)
				opp++;
			*opp++ = '|';
		} else {
			word3 = (*getfunc)(&addr, space);
			addr.a_offset += 2;
			a.a_offset = (unsigned)word3;
			(*symfunc)(&a, symkind, opp);
		}
	}
}

static
epuldctl()
{
	if ((word & 07400) == 07000) {
		switch ((word & 014) >> 2) {
		case 0:
			strcpy(opcode, "xldctl");
			strcpy(operand[0], n_flags);
			opp = operand[1];
			epu();
			opp = operand[2];
			epuimm();
			break;
		case 1:
			strcpy(opcode, "xctl");
			opp = operand[0];
			epu();
			opp = operand[1];
			epuimm();
			break;
		case 2:
			strcpy(opcode, "xldctl");
			opp = operand[0];
			epu();
			strcpy(operand[1], n_flags);
			opp = operand[2];
			epuimm();
			break;
		default:
			bad = 1;
		}
	} else {
		sprintf(operand[2], "#%d", (word2 & 017) + 1);
		opp = operand[3];
		epuimm();
		strcpy(opcode, "xldm");
		switch ((word & 014) >> 2) {
		case 0:
			opp = operand[0];
			wreg(word2 >> 8);
			opp = operand[1];
			epu();
			break;
		case 1:
			opp = operand[0];
			epu();
			opp = operand[1];
			addrmode();
			break;
		case 2:
			opp = operand[0];
			epu();
			opp = operand[1];
			wreg(word2 >> 8);
			break;
		case 3:
			opp = operand[1];
			epu();
			opp = operand[0];
			addrmode();
			break;
		}
	}
}

static
epu()
{
	*opp++ = 'e';
	*opp++ = 'p';
	*opp++ = 'u';
	*opp++ = (word & 03) + '0';
}

static
epuimm()
{
	sprintf(opp, "#%04x", (word2 & 0170000) >> 8 | (word2 & 0360) >> 4);
}

static
fpsize()
{
	register int n;

	opp = opcode;
	while (*opp)
		opp++;
	n = word2 & 017;
	switch (fpflags) {
	case 0:
		switch (n) {
		case 1:
			*opp++ = 's';
			break;
		case 3:
			*opp++ = 'd';
			break;
		case 4:
			if (fpop == 043) {
				strcpy(operand[2], "#1");
				break;
			}
			break;
		case 9:
			if (fpop == 043) {
				strcpy(operand[2], "#2");
				break;
			}
		default:
			fpbad = 1;
		}
		break;
	case 0100000:
	case 0140000:
		switch (n) {
		case 1:
			*opp++ = 'l';
			size = 3;
			break;
		case 3:
			*opp++ = 'q';
			size = 4;
			break;
		case 4:
			strcpy(opp, "bcd");
			opp += 3;
			break;
		default:
			fpbad = 1;
		}
		break;
	case 040000:
		switch (n) {
		case 3:
			*opp++ = 's';
			break;
		case 7:
			*opp++ = 'd';
			break;
		case 9:
			break;
		default:
			fpbad = 1;
		}
		break;
	case 020000:
		break;
	default:
		fpbad = 1;
	}
}

static
fpprec()
{
	switch ((word2 & 030000) >> 12) {
	case 0:
		break;
	case 1:
		strcpy(opp, "sgl ");
		opp += 4;
		break;
	case 3:
		strcpy(opp, "dbl ");
		opp += 4;
		break;
	default:
		fpbad = 1;
	}
}

static
freg(r)
register unsigned r;
{
	if (r & 0200)
		fpbad = 1;
	else {
		if (fpflags & 040000)
			if (r & 020)
				fpbad = 1;
			else
				*opp++ = 'f';
		*opp++ = 'f';
		*opp++ = ((r & 0360) >> 4) + '0';
	}
}

static
fctl(r)
register unsigned r;
{
	register unsigned n;

	if (fpop == 060)
		if ((n = (r & 0360) >> 4) > 7)
			fpbad = 1;
		else
			strcpy(opp, ctlnam[n]);
	else
		freg(r);
}

disinit(getf, symf, flag)
unsigned (*getf)();
int (*symf)();
register int flag;
{
	segmode = flag;
	getfunc = getf;
	symfunc = symf;
}

ADDR *
disasm(ap, sp, ip)
ADDR	*ap;
int	sp;
INSTR	*ip;
{
	register int	i, j;
	register unsigned	op, n;
	char	*p;
	int	t;
	ADDR	a;

	for (i = 0; i <= 16; i++)
		opcode[i] = 0;
	ip->d_opcode = opcode;
	for (i = 0; i <= 3; i++) {
		for (j = 0; j <= 29; j++)
			operand[i][j] = 0;
		ip->d_operand[i] = operand[i];
	}
	for (i = 0; i <= 19; i++)
		comment[i] = 0;
	ip->d_comment = comment;
	space = sp;
	symkind = 1;
	bad = 0;
	addr.a_segno = ap->a_segno;
	addr.a_offset = ap->a_offset;
	word = (*getfunc)(&addr, sp);
	addr.a_offset += 2;
	mode = word >> 14;
	if (mode != 3) {
		op = (word >> 8) & 077;
		p = optab[op].o_name;
		if (*p != 0)
			strcpy(opcode, p);
		size = optab[op].o_fmt;
		switch (op) {
		case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
		case 8: case 9: case 10: case 11: case 16: case 18: case 20: case 22:
		case 32: case 33: case 44: case 45:
			regimm();
			break;
		case 12:
			n = word & 017;
			p = op8d[n].o_name;
			if (*p != 0)
				strcpy(opcode, p);
			size = op8d[n].o_fmt;
			switch (n) {
			case 0: case 2: case 4: case 6: case 8:
				opp = operand[0];
				addrmode();
				break;
			case 1:
				switch (mode) {
				case 0:
				case 1:
					strcpy(opcode, n_cpb);
					opp = operand[0];
					addrmode();
					imm16();
					break;
				case 2:
					strcpy(opcode, n_ldctlb);
					opp = operand[0];
					breg(word >> 4);
					strcpy(operand[1], n_flags);
					break;
				default:
					bad = 1;
				}
				break;
			case 5:
				opp = operand[0];
				addrmode();
				imm16();
				break;
			case 9:
				opp = operand[1];
				breg(word >> 4);
				strcpy(operand[0], n_flags);
				break;
			default:
				bad = 1;
			}
			break;
		case 13:
			n = word & 017;
			p = opcd[n].o_name;
			if (*p != 0)
				strcpy(opcode, p);
			size = opcd[n].o_fmt;
			switch (n) {
			case 0: case 2: case 4: case 6: case 8:
				opp = operand[0];
				addrmode();
				break;
			case 1:
				switch (mode) {
				case 0:
				case 1:
					strcpy(opcode, n_cp);
					opp = operand[0];
					addrmode();
					imm16();
					break;
				case 2:
					strcpy(opcode, n_setflg);
					flags();
					break;
				default:
					bad = 1;
				}
				break;
			case 3:
				strcpy(opcode, n_resflg);
				flags();
				break;
			case 5:
				switch (mode) {
				case 2:
					strcpy(opcode, n_comflg);
					flags();
					break;
				case 0:
				case 1:
					strcpy(opcode, n_ld);
					addrimm();
					break;
				default:
					bad = 1;
				}
				break;
			case 7:
				strcpy(opcode, n_nop);
				break;
			case 9:
				strcpy(opcode, n_push);
				opp = operand[0];
				ireg(word >> 4);
				imm16();
				break;
			default:
				bad = 1;
			}
			break;
		case 14:
		case 15:
			fpbad = 0;
			word2 = (*getfunc)(&addr, space);
			addr.a_offset += 2;
			fpop = (word2 & 0360) >> 4 | (word2 & 0140000) >> 10;
			if (fpop == 021 && word & 014)
				fpop = 023;
			if (fpop == 022 && word & 014)
				fpop = 024;
			if (*fptab[fpop].o_name == 0)
				fpbad = 1;
			else {
				strcpy(opcode, fptab[fpop].o_name);
				size = fptab[fpop].o_fmt & 017777;
				fpflags = fptab[fpop].o_fmt & 0160000;
				if (word & 0400)
					switch ((word & 014) >> 2) {
					case 0:
						if (mode != 2)
							fpbad = 1;
						else {
							fpsize();
							opp = operand[0];
							reg(word2 >> 8);
							opp = operand[1];
							fctl(word);
						}
						break;
					case 1:
						if (word & 0100000)
							fpbad = 1;
						else {
							fpsize();
							opp = operand[0];
							fpprec();
							fctl(word2 >> 4);
							opp = operand[1];
							addrmode();
						}
						break;
					case 2:
						if (mode != 2)
							fpbad = 1;
						else {
							fpsize();
							opp = operand[0];
							fpprec();
							fctl(word);
							opp = operand[1];
							reg(word2 >> 8);
						}
						break;
					case 3:
						if (word & 0100000)
							fpbad = 1;
						else {
							fpsize();
							opp = operand[1];
							fctl(word2 >> 4);
							opp = operand[0];
							addrmode();
						}
						break;
					}
				else if (mode != 2)
					fpbad = 1;
				else
					switch ((word & 014) >> 2) {
					case 0:
						if (word2 & 017)
							fpbad = 1;
						else if (fpop == 034) {
							opp = operand[0];
							fpprec();
							freg(word);
							opp = operand[1];
							freg(word2 >> 4);
						} else if (fpop == 021 || fpop == 022) {
							opp = operand[0];
							freg(word);
							opp = operand[1];
							freg(word2 >> 4);
						} else if (word2 & 030000)
							fpbad = 1;
						else if (fpop != 061) {
							opp = operand[0];
							freg(word);
						} else if ((n = (word & 0360) >> 4) > 4)
							fpbad = 1;
						else
							strcpy(operand[0], flgnam[n]);
						break;
					case 1:
						if (fpop == 0) {
							opp = operand[0];
							freg(word);
						} else if (!(word2 & 017) && fpop != 064 && fpop != 062 && fpop != 063) {
							opp = operand[0];
							fpprec();
							freg(word);
							opp = operand[1];
							freg(word2 >> 4);
						} else if (((word2 & 0360) >> 4) ^ 4) {
							opp = opcode;
							if (fpop & 1)
								strcpy(opp, "fset");
							else
								strcpy(opp, "fres");
							opp += 4;
							switch ((word & 0360) >> 4) {
							case 0:
								strcpy(opp, "flg");
								break;
							case 2:
								strcpy(opp, "trap");
								break;
							default:
								fpbad = 1;
							}
							opp = operand[0];
							if ((n = word2 & 017) & 1) {
								strcpy(opp, "inv,");
								opp += 4;
							}
							if (n & 2) {
								strcpy(opp, "fov,");
								opp += 4;
							}
							if (n & 4) {
								strcpy(opp, "un,");
								opp += 3;
							}
							if (n & 8) {
								strcpy(opp, "dz,");
								opp += 3;
							}
							if ((n = (word2 & 07400) >> 8) & 1) {
								strcpy(opp, "inx,");
								opp += 4;
							}
							if (n & 2) {
								strcpy(opp, "de,");
								opp += 3;
							}
							if (n & 4) {
								strcpy(opp, "nan,");
								opp += 4;
							}
							if (n & 8) {
								strcpy(opp, "ix");
								opp += 3;
							}
							*--opp = '\0';
						} else if (word & 0360)
							fpbad = 1;
						else {
							strcpy(opcode, "fsetmode");
							opp = operand[0];
							if ((n = word2 & 07417) & 0400) {
								strcpy(opp, n & 1 ? "aff," : "proj,");
								opp += n & 1 ? 4 : 5;
							}
							if (n & 03000) {
								*opp++ = 'r';
								switch (n & 6) {
								case 0:
									*opp++ = 'n';
									break;
								case 2:
									*opp++ = 'z';
									break;
								case 4:
									*opp++ = 'p';
									break;
								case 6:
									*opp++ = 'm';
									break;
								}
								opp++;
							}
							*--opp = '\0';
						}
						break;
					default:
						fpbad = 1;
					}
			}
			if (fpbad)
				epuldctl();
			break;
		case 17:
		case 19:
			opp = operand[0];
			opp2 = operand[1];
			ldiw();
			break;
		case 21:
		case 23:
			opp = operand[1];
			opp2 = operand[0];
			ldiw();
			break;
		case 24: case 25: case 26: case 27:
			opp = operand[0];
			reg(word);
			if (size == 4)
				size = 3;
			else
				size = 2;
			immed();
			break;
		case 28:
			switch (word & 017) {
			case 8:
				strcpy(opcode, n_testl);
				size = 3;
				opp = operand[0];
				addrmode();
				break;
			case 1:
				strcpy(opcode, n_ldm);
				opp = operand[0];
				opp2 = operand[1];
				ldm();
				break;
			case 9:
				strcpy(opcode, n_ldm);
				opp = operand[1];
				opp2 = operand[0];
				ldm();
				break;
			default:
				bad = 1;
			}
			break;
		case 29:
			addrreg();
			break;
		case 30:
			switch (mode) {
			case 0:
			case 1:
				symkind = 0;
				strcpy(opcode, n_jp);
				ccop();
				break;
			case 2:
				strcpy(opcode, n_ret);
				strcpy(operand[0], ccnam[word & 017]);
				break;
			default:
				bad = 1;
			}
			break;
		case 31:
			symkind = 0;
			opp = operand[0];
			addrmode();
			break;
		case 34: case 35: case 36: case 37: case 38: case 39:
			if ((word & 0140360) == 0) {
				opp = operand[1];
				wreg(word);
				word2 = (*getfunc)(&addr, space);
				addr.a_offset += 2;
				opp = operand[0];
				reg(word2 >> 8);
			} else
				imm(word & 017, 0);
			break;
		case 40: case 41: case 42: case 43:
			imm((word & 017) + 1, 1);
			break;
		case 46:
			switch (mode) {
			case 2:
				strcpy(opcode, n_tccb);
				ccop();
				break;
			case 0:
			case 1:
				strcpy(opcode, n_ldb);
				addrreg();
				break;
			default:
				bad = 1;
			}
			break;
		case 47:
			switch (mode) {
			case 2:
				strcpy(opcode, n_tcc);
				ccop();
				break;
			case 0:
			case 1:
				strcpy(opcode, n_ld);
				addrreg();
				break;
			default:
				bad = 1;
			}
			break;
		case 48:
			switch (mode) {
			case 0:
				if ((word & 0360) != 0)
					strcpy(opcode, n_ldb);
				else {
					strcpy(opcode, n_ldrb);
					symkind = 0;
				}
				regba();
				break;
			case 1:
				strcpy(opcode, n_ldb);
				regbx();
				break;
			case 2:
				strcpy(opcode, n_dab);
				opp = operand[0];
				addrmode();
				break;
			default:
				bad = 1;
			}
			break;
		case 49:
			switch (mode) {
			case 0:
				if ((word & 0360) != 0)
					strcpy(opcode, n_ld);
				else {
					strcpy(opcode, n_ldr);
					symkind = 0;
				}
				regba();
				break;
			case 1:
				strcpy(opcode, n_ld);
				regbx();
				break;
			case 2:
				switch (word & 017) {
				case 0:
					strcpy(opcode, n_extsb);
					break;
				case 7:
					strcpy(opcode, n_extsl);
					size = 4;
					break;
				case 10:
					strcpy(opcode, n_exts);
					size = 3;
					break;
				default:
					bad = 1;
				}
				opp = operand[0];
				addrmode();
				break;
			default:
				bad = 1;
			}
			break;
		case 50:
		case 51:
			switch (mode) {
			case 0:
				if ((word & 0360) == 0) {
					strcpy(opcode, (word & 0400) == 0 ? n_ldrb : n_ldr);
					symkind = 0;
				} else
					strcpy(opcode, (word & 0400) == 0 ? n_ldb : n_ld);
				opp = operand[1];
				reg(word);
				opp = operand[0];
				ba();
				break;
			case 1:
				strcpy(opcode, (word & 0400) == 0 ? n_ldb : n_ld);
				opp = operand[1];
				reg(word);
				opp = operand[0];
				bx();
				break;
			case 2:
				n = word & 017;
				p = opshift[n].o_name;
				strcpy(opcode, p);
				if ((op & 1) == 0 && (n & 5) == 5)
					bad = 1;
				if ((op & 1) == 1) {
					p = opcode;
					while (*++p != 0)
						;
					if (*--p == 'b')
						*p = '\0';
					else
						size = 3;
				}
				switch (n) {
				case 0: case 2: case 4: case 6: case 8: case 10: case 12: case 14:
					if ((word & 2) != 0) {
						operand[1][0] = '#';
						operand[1][1] = '2';
					}
					opp = operand[0];
					addrmode();
					break;
				case 1: case 5: case 9: case 13:
					opp = operand[0];
					reg(word >> 4);
					word2 = (*getfunc)(&addr, space);
					addr.a_offset += 2;
					if ((int)word2 < 0) {
						word2 = -word2;
						opcode[1] = 'r';
					}
					if (word2 != 1)
						sprintf(operand[1], "#%d", word2);
					break;
				case 3: case 7: case 11: case 15:
					opp = operand[0];
					reg(word >> 4);
					word2 = (*getfunc)(&addr, space);
					addr.a_offset += 2;
					opp = operand[1];
					wreg(word2 >> 8);
					break;
				default:
					bad = 1;
				}
				break;
			default:
				bad = 1;
			}
			break;
		case 52:
			switch (mode) {
			case 0:
				if ((word & 0360) == 0) {
					strcpy(opcode, n_ldar);
					symkind = 0;
				} else
					strcpy(opcode, n_lda);
				if (segmode == 1)
					size = 3;
				regba();
				break;
			case 1:
				strcpy(opcode, n_lda);
				if (segmode == 1)
					size = 3;
				regbx();
				break;
			case 2:
				strcpy(opcode, n_adcb);
				size = 1;
				regimm();
				break;
			default:
				bad = 1;
			}
			break;
		case 53:
			switch (mode) {
			case 0:
				size = 3;
				if ((word & 0360) == 0) {
					strcpy(opcode, n_ldrl);
					symkind = 0;
				} else
					strcpy(opcode, n_ldl);
				regba();
				break;
			case 2:
				strcpy(opcode, n_adc);
				regimm();
				break;
			case 1:
				size = 3;
				strcpy(opcode, n_ldl);
				regbx();
				break;
			default:
				bad = 1;
			}
			break;
		case 54:
			switch (mode) {
			case 1:
				strcpy(opcode, n_lda);
				if (segmode == 1)
					size = 3;
				regimm();
				break;
			case 2:
				strcpy(opcode, n_sbcb);
				size = 1;
				regimm();
				break;
			default:
				bad = 1;
			}
			break;
		case 55:
			switch (mode) {
			case 0:
				size = 3;
				if ((word & 0360) == 0) {
					strcpy(opcode, n_ldrl);
					symkind = 0;
				} else
					strcpy(opcode, n_ldl);
				opp = operand[1];
				reg(word);
				opp = operand[0];
				ba();
				break;
			case 2:
				strcpy(opcode, n_sbc);
				regimm();
				break;
			case 1:
				size = 3;
				strcpy(opcode, n_ldl);
				opp = operand[1];
				reg(word);
				opp = operand[0];
				bx();
				break;
			default:
				bad = 1;
			}
			break;
		case 56:
			word2 = (*getfunc)(&addr, space);
			addr.a_offset += 2;
			n = (word >> 1) & 07;
			strcpy(opcode, optrans[n].o_name);
			opp = operand[0];
			ireg(word >> 4);
			opp = operand[1];
			ireg2(word2);
			break;
		case 57:
			opp = operand[0];
			addrmode();
			break;
		case 58:
		case 59:
			switch (mode) {
			case 1:
				switch (word & 0377) {
				case 0:
					if ((word & 0400) == 0)
						strcpy(opcode, n_halt);
					else
						strcpy(opcode, n_iret);
					break;
				case 8:
					strcpy(opcode, n_mset);
					break;
				case 9:
					strcpy(opcode, n_mres);
					break;
				case 10:
					strcpy(opcode, n_mbit);
					break;
				case 13:
					strcpy(opcode, n_mreq);
					opp = operand[0];
					reg(word >> 4);
					break;
				default:
					bad = 1;
				}
				break;
			case 0:
				word2 = (*getfunc)(&addr, space);
				addr.a_offset += 2;
				t = word2 & 010;
				switch ((word & 017) >> 1) {
				case 0:
					strcpy(opcode, t == 0 ? n_inir : n_ini);
					opp = operand[1];
					*opp++ = '@';
					wreg(word >> 4);
					opp = operand[0];
					ireg2(word2);
					break;
				case 1:
					strcpy(opcode, t == 0 ? n_otir : n_outi);
					opp = operand[0];
					*opp++ = '@';
					wreg(word2 >> 4);
					opp = operand[1];
					ireg2(word);
					break;
				case 2:
					strcpy(opcode, n_in);
					opp = operand[0];
					reg(word >> 4);
					sprintf(operand[1], fmta, word2);
					break;
				case 3:
					strcpy(opcode, n_out);
					opp = operand[1];
					reg(word >> 4);
					sprintf(operand[0], fmta, word2);
					break;
				case 4:
					strcpy(opcode, t == 0 ? n_indr : n_ind);
					opp = operand[1];
					*opp++ = '@';
					wreg(word >> 4);
					opp = operand[0];
					ireg2(word2);
					break;
				case 5:
					strcpy(opcode, t == 0 ? n_otdr : n_outd);
					opp = operand[0];
					*opp++ = '@';
					wreg(word2 >> 4);
					opp = operand[1];
					ireg2(word);
					break;
				default:
					bad = 1;
				}
				if ((word & 1) != 0) {
					opp = opcode;
					while (*++opp != 0)
						;
					while (opp != opcode) {
						*opp = opp[-1];
						opp--;
					}
					opcode[0] = 's';
				}
				break;
			case 2:
				word2 = (*getfunc)(&addr, space);
				addr.a_offset += 2;
				t = word2 & 1;
				switch ((word >> 1) & 07) {
				case 0:
					strcpy(opcode, t == 0 ? ((word2 & 010) == 0 ? n_ldir : n_ldi) : n_cpi);
					break;
				case 1:
					strcpy(opcode, n_cpsi);
					break;
				case 2:
					strcpy(opcode, n_cpir);
					break;
				case 3:
					strcpy(opcode, n_cpsir);
					break;
				case 4:
					strcpy(opcode, t == 0 ? ((word2 & 010) == 0 ? n_lddr : n_ldd) : n_cpd);
					break;
				case 5:
					strcpy(opcode, n_cpsd);
					break;
				case 6:
					strcpy(opcode, n_cpdr);
					break;
				case 7:
					strcpy(opcode, n_cpsdr);
					break;
				default:
					bad = 1;
				}
				if ((word & 1) != 0) {
					opp = operand[1];
					ireg(word >> 4);
					opp = operand[0];
					ireg2(word2);
				} else if ((word & 2) == 0) {
					opp = operand[0];
					reg(word2 >> 4);
					opp = operand[1];
					ireg2(word);
					strcpy(operand[3], ccnam[word2 & 017]);
				} else {
					opp = operand[1];
					ireg(word >> 4);
					opp = operand[0];
					ireg2(word2);
					strcpy(operand[3], ccnam[word2 & 017]);
				}
				break;
			default:
				bad = 1;
			}
			if (mode != 1 && (word & 0400) == 0) {
				p = opcode;
				while (*++p != 0)
					;
				*p++ = 'b';
				*p = '\0';
			}
			break;
		case 60:
			switch (mode) {
			case 0:
				strcpy(opcode, n_inb);
				opp = operand[0];
				reg(word);
				opp = operand[1];
				*opp++ = '@';
				wreg(word >> 4);
				break;
			case 1:
				strcpy(opcode, (word & 4) == 0 ? n_di : n_ei);
				strcpy(operand[0], (word & 1) == 0 ? ((word & 2) == 0 ? "vi,nvi" : "nvi") : ((word & 2) == 0 ? "vi" : ""));
				break;
			case 2:
				strcpy(opcode, n_rrdb);
				regimm();
				break;
			}
			break;
		case 61:
			switch (mode) {
			case 0:
				strcpy(opcode, n_in);
				opp = operand[0];
				reg(word);
				opp = operand[1];
				*opp++ = '@';
				wreg(word >> 4);
				break;
			case 1:
				strcpy(opcode, n_ldctl);
				if ((word & 010) == 0) {
					opp = operand[0];
					opp2 = operand[1];
				} else {
					opp = operand[1];
					opp2 = operand[0];
				}
				wreg(word >> 4);
				switch (n = word & 07) {
				case 2:
					strcpy(opp2, "fcw");
					break;
				case 3:
					strcpy(opp2, "refresh");
					break;
				case 4:
					strcpy(opp2, "psapseg");
					break;
				case 5:
					strcpy(opp2, segmode == 1 ? "psapoff" : "psap");
					break;
				case 6:
					strcpy(opp2, "nspseg");
					break;
				case 7:
					strcpy(opp2, segmode == 1 ? "nspoff" : "nsp");
					break;
				}
				if (segmode != 1 && (n == 4 || n == 6))
					bad = 1;
				break;
			case 2:
				strcpy(opcode, n_ldk);
				imm(word & 017, 0);
				break;
			}
			break;
		case 62:
			switch (mode) {
			case 0:
				strcpy(opcode, n_outb);
				opp = operand[1];
				reg(word);
				opp = operand[0];
				*opp++ = '@';
				wreg(word >> 4);
				break;
			case 2:
				strcpy(opcode, n_rldb);
				regimm();
				break;
			}
			break;
		case 63:
			switch (mode) {
			case 0:
				strcpy(opcode, n_out);
				opp = operand[1];
				reg(word);
				opp = operand[0];
				*opp++ = '@';
				wreg(word >> 4);
				break;
			case 1:
				strcpy(opcode, n_sc);
				n = word & 0377;
				comment[0] = '!';
				strcpy(&comment[1], sysnam[n]);
				i = 0;
				while (comment[++i] != 0)
					;
				comment[i++] = '!';
				comment[i] = '\0';
				sprintf(operand[0], "#%d", n);
				break;
			default:
				bad = 1;
			}
			break;
		default:
			bad = 1;
		}
	} else
		switch ((word >> 12) & 03) {
		case 0:
			strcpy(opcode, n_ldb);
			opp = operand[0];
			breg(word >> 8);
			sprintf(operand[1], "#%%%02x", word & 0377);
			break;
		case 1:
			symkind = 0;
			strcpy(opcode, n_calr);
			if ((word & 04000) == 0)
				n = word & 07777;
			else
				n = word | 0170000;
			a.a_offset = addr.a_offset - (int)(n << 1);
			a.a_segno = addr.a_segno;
			(*symfunc)(&a, symkind, operand[0]);
			break;
		case 2:
			symkind = 0;
			strcpy(opcode, n_jr);
			strcpy(operand[0], ccnam[(word >> 8) & 017]);
			a.a_offset = (char)word * 2 + addr.a_offset;
			a.a_segno = addr.a_segno;
			(*symfunc)(&a, symkind, operand[1]);
			break;
		case 3:
			symkind = 0;
			opp = operand[0];
			if ((word & 0200) == 0) {
				strcpy(opcode, n_djnzb);
				breg(word >> 8);
			} else {
				strcpy(opcode, n_djnz);
				wreg(word >> 8);
			}
			a.a_offset = addr.a_offset - (int)(word << 1 & 0377);
			a.a_segno = addr.a_segno;
			(*symfunc)(&a, symkind, operand[1]);
			break;
		}
	if (bad) {
		sprintf(opcode, fmta, word);
		for (i = 0; i <= 3; i++)
			operand[i][0] = 0;
		return(0);
	}
	return(&addr);
}
