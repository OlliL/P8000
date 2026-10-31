/*
 * oldcc.h - the switches of the old code generators (host/OLDCC.md). A version Makefile names only its version
 * (OLD_V0, OLD_V0P): the native cc takes about 160 characters of -D options (more overwrite the name of its
 * temporary file: "Can't create /tmp/cc.../lib/cpp"). Without a version nothing is set: the shipped codgen.
 */
#ifdef OLD_V0P
/* v0p, the compiler of the passes: v0 and */
#define OLD_V0
#define OLD_D2Q		/* a long loaded through a pointer into the pair of its even address register, also when wanted */
#define OLD_D2X		/* a long result of a binary operator (not a shift) into the pair of its left operand first */
#define OLD_D3A		/* the registers of the parameters forgotten before the arguments of a call (gencall) */
#define OLD_D2A		/* the value of a long assignment copied into a new pair, which keeps the name (match) */
#endif

#ifdef OLD_V0
/* v0, the programs of 1982 */
#define OLD_D2R		/* a load through a pointer (STAR, OREG) into the register of its address first (allocreg) */
#define OLD_D18		/* the frame operand at offset 0 written as 0(fp), never @fp */
#define OLD_D1R		/* no temporary for the value of a return (r2 stays taken while a delayed x++ runs) */
#define OLD_D2W		/* the free registers: r0 early (rweight), the pairs rr2 rr0 rr4 rr6 at the start (rsbuild) */
#define OLD_D10		/* a long shifted by a constant through a register (ld A1,#n; sdal) */
#define OLD_D3P		/* a name in a register is taken from it also when another register is wanted (regforsym) */
#define OLD_D4O		/* the parameters taken in the order of their declaration (the copies of register parameters) */
#define OLD_D2C		/* a name (not a parameter) found in a scratch register is that register; a copy holds no name */
#define OLD_D2F		/* a free word register only when it weighs no more than the first free pair */
#define OLD_D2L		/* an idle scratch word that only holds a name counts as free when it weighs less (getreg) */
#define OLD_D2N		/* a name loaded into a word register (a global, an auto) is not remembered there */
#define OLD_D6T		/* a cpir switch: the values in front of the targets (record V), the target from the cpir pointer */
#define OLD_D2T		/* the register of a TEMP (the value of a switch) counts as free for OLD_D2L between its uses */
#define OLD_D6R		/* the count register of cpir in neither half of rr0 (gswitch) */
#define OLD_D3K		/* a parameter stays known in its register at a label a jump has named, and behind && and || */
#define OLD_D2V		/* OLD_D2R only when no lighter register is free (the address register competes by weight) */
#define OLD_D2M		/* a word multiplied through a pair: the pair of its even register (allocreg) */
#define OLD_D2P		/* OLD_D2L for pairs: an idle pair that holds a name is free when it weighs less */
#define OLD_D2U		/* the right operand of a long multiplication wants no register (rallo) */
#define OLD_D2G		/* OLD_D2L: an idle word of the same weight as the free one wins */
#endif

/* d2qfree (allo3.c) serves OLD_D2Q and OLD_D2M */
#ifdef OLD_D2Q
#define OLD_D2QF
#endif
#ifdef OLD_D2M
#ifndef OLD_D2QF
#define OLD_D2QF
#endif
#endif
