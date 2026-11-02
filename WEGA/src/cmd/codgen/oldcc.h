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
#define OLD_D5G		/* an element of a global array of arrays: (index + index) + address (fixfld) */
#define OLD_D3AW	/* OLD_D3A only for a parameter that is a whole argument or an address in it (argplace) */
#define OLD_D2A		/* the value of a long assignment copied into a new pair, which keeps the name (match) */
#define OLD_D21	/* an unsigned division or modulo by a power of two stays a division (optim) */
#define OLD_D2PT	/* OLD_D2P takes the registers of a TEMP (the value of a switch) too */
#define OLD_D2MO	/* OLD_D2M for an odd left register: the pair below it when its even half is free or idle */
#define OLD_D2QO	/* OLD_D2Q also when an odd word register is wanted (the low half: an argument) */
#define OLD_D2O		/* a long loaded through an odd address register into the pair below it (allocreg) */
#define OLD_D16B	/* an unsigned word in memory with an assignment operator: rewritten as a = a op b (match, order) */
#define OLD_D16A	/* a frame slot incremented by a constant, value to a temporary, not for a compare: load, increment, store (match, setbin) */
#define OLD_D2R0	/* a load through an address moved out of r0 goes back into r0 (freereg) */
#define OLD_D6S		/* an assignment of a register to itself prints nothing (match) */
#define OLD_D3V		/* the value of a word assignment painted by a cast may be the memory it stored (reclaim) */
#define OLD_D3JM	/* OLD_D3J: at a label behind a jump named by several jumps the parameters known at the jumps and in front of it */
#define OLD_D3N		/* the variable the statement before stored from a register, stored through a pointer: that register */
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
#define OLD_D5S		/* an element of a local array: (index + offset) + fp, the frame pointer added last (fixfld) */
#define OLD_D2PO	/* OLD_D2P: an idle odd word whose even half is free is given back for a pair */
#define OLD_D3J		/* at a label named by jumps only the parameters known at all of them; behind a jump those of its one jump */
#define OLD_D2W6	/* a pair asked for when rr2 and rr0 are taken: rr6 before rr4 (getreg) */
#define OLD_D5A		/* the address of a local array element whose value goes to a given register: no preference (rallo) */
#define OLD_D22		/* a double argument, parameter and result in rq4 (r4..r7); a floating compare without rounding */
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
