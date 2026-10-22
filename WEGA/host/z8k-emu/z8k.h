/*
 * z8k.h - user-mode Z8001/Z8002 CPU core used as an oracle for the WEGA tool
 * chain reconstruction.  No MMU, no interrupts, no system mode: only what a
 * WEGA user program can execute.  System calls (SC) are handed to a callback.
 */
#ifndef Z8K_H
#define Z8K_H
#include <stdint.h>
#include <stdio.h>

#define F_C 0x80
#define F_Z 0x40
#define F_S 0x20
#define F_V 0x10
#define F_D 0x08
#define F_H 0x04

typedef struct Z8K Z8K;

struct Z8K {
	uint16_t r[16];
	uint8_t  fl;		/* flags C Z S P/V D H in bits 7..2 */
	int      segmode;	/* Z8001 (segmented) if non-zero */
	uint8_t  pcseg;		/* code segment (segmented mode) */
	uint16_t pc;
	uint8_t *imem[128];	/* instruction space, one 64K block per segment */
	uint8_t *dmem[128];	/* data space; same blocks as imem if combined I&D */
	int      sepid;		/* separate instruction and data space */
	uint8_t  fps[0x90];	/* u.u_fpstate of the EPU emulation (sys/user.h) */
	int      halted;
	int      trace;
	int      verbose;
	uint64_t icount;
	uint16_t cur_pc;	/* address of the instruction being executed */
	uint8_t  cur_seg;
	void    *user;
	/* returns 0 normally; non-zero stops the interpreter loop */
	int    (*syscall)(Z8K *c, unsigned n);
};

uint8_t *z8k_block(Z8K *c, int space, unsigned seg); /* space 0 = I, 1 = D */
int  z8k_step(Z8K *c);		/* execute one instruction, 0 = ok */
void z8k_dump(Z8K *c, FILE *f);
int  z8k_epu(Z8K *c, uint16_t w0);	/* EPU instruction (epu.c) */
void z8k_epu_reset(Z8K *c);		/* EPU registers after exec */

#endif
