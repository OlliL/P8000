/*
 * regs3.c - the register names and the status of the registers (codgen / scodgen)
 *
 * From /lib/codgen: rnames 0x3fca, rhnames 0x27ba, rlnames 0x27ca, rstatus 0x27da.
 * Registers 0-15 are the word registers (15 is fp), 16-23 the float registers f0-f7; r4-r7 and f0-f3 hold values
 * across expressions (STAREG / STBREG), the others serve variables.
 */
#include "cg.h"

char *rnames[] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "fp",
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
};

char *rhnames[] = { "rh0", "rh1", "rh2", "rh3", "rh4", "rh5", "rh6", "rh7" };

char *rlnames[] = { "rl0", "rl1", "rl2", "rl3", "rl4", "rl5", "rl6", "rl7" };

int rstatus[] = {
	SAREG | STAREG, SAREG | STAREG, SAREG | STAREG, SAREG | STAREG,
	SAREG | STAREG, SAREG | STAREG, SAREG | STAREG, SAREG | STAREG,
	SAREG, SAREG, SAREG, SAREG, SAREG, SAREG, SAREG, SAREG,
	SBREG | STBREG, SBREG | STBREG, SBREG | STBREG, SBREG | STBREG,
	SBREG, SBREG, SBREG, SBREG,
};
