/* disasm.h */
/* disasm/disinit  .... disassembler include file */

typedef struct address { char a_segno;
	      long a_offset;
	    } ADDR;

typedef struct _instr { char *d_opcode;
	       char *d_operand[4];
	       char *d_comment;
	     } INSTR;

#define NON_SEG 0
#define SEG     1

#define ISYMBOL 0
#define DSYMBOL 1

int	disinit();
ADDR	*disasm();
