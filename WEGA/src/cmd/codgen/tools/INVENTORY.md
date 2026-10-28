# codgen inventory (status of the analysis)

Generated from findings.json by inventory.py. Method and limits: see classification_method and the notes below.

## Pairings

- functions of our codgen: 206
- confirmed with a counterpart in /lib/codgen: 206
- without counterpart on our side: 
- original functions without counterpart on our side: `0x4868` (forwarder of main, main-wrapper-01), `0x6fca` and `0x854c` (dead code, quirk table)
- code coverage: every instruction of our codgen belongs to a function; in the original only the start code (0x0-0x28) lies outside the pairings, and the continuation of rdsym (0x1aae) belongs to rdsym

## Identical

Identical under metric strict-1 (the same instruction sequence, only addresses masked): 8 functions: `endrun`, `errexit`, `hdrout`, `rcount_reset`, `regreset`, `regtabinit`, `rnnullall`, `rnnullcall`

## Only D1-D14 (pattern-verified)

200 functions: every differing instruction matches a D pattern (D1, D2, D3, D5, D6, D7, D8, D10, D11, D13). The order of the instructions is not part of this check. D4, D9, D12 and D14 are not separable by these patterns.

## D1-D14 and real differences

6 functions with real differences (the D part is not separated from them): `clocal`, `expand`, `freereg`, `sucomp`, `tshape`, `zzzcode`

## Real differences (findings)

- `allo3-rcookie-dup` (fixed (allo3.c: rcookie removed, allocreg calls rallkind)): the original allocreg calls 0x89a2 (rallkind in comm3.c); we have a second copy of the same function in allo3.c
- `allo3-tcopy-copy-01` (fixed (allo3.c: tcopy calls ncopy(q, p))): original 0x8a24 calls ncopy (0x8a02) for the copy of the node; the rebuild copies inline (ldir)
- `ttype-shift-01` (merged into shift-type-01): the original shifts with srl (unsigned), ours with sra (signed)
- `fixfld-shift-01` (merged into shift-type-01): the original shifts the field value with srl, ours with sra
- `shift-type-01` (fixed for ttype; the rest is shift-type-02): three functions shift a field value with srl (unsigned) in the original and with sra (signed) in ours
- `mapnm-real-01` (analysed): pairing confirmed by strings; only 33 % of the instructions match; the statement order and the expressions differ (sdal / slal, the test order)
- `buildstar-order-01` (explained: layout): only 50 % match; the if chains come in another order
- `expand-loop-01` (explained): the original reads *cp into a byte and dispatches through a table; ours tests *cp and then dispatches
- `options-loop-01` (fixed (main3.c: register int i, err; 76 %, the rest D)): the loop variable is a register in the original and in memory in ours; the layout of the switch differs
- `main-char-01` (checked): the original reads the input character as a byte; ours as int; the switch is a compare chain there (D6)
- `tshape-order-01` (checked: case order (no further gain); SZERO value form open): the call order special / shtemp / shumul / ltypeop differs; a long test is written differently (cpl and tcc in the original)
- `efcode-shift-01` (checked): sdal (original) against slal (ours) for the same scaling of sz; the sequence of the additions differs
- `sucomp-size-01` (partly fixed (locals nf, fl, t without register)): sucomp: 756 instructions against 684; the difference is in the body (not yet analysed)
- `rsinsert-size-01` (analysed): rsinsert: 157 against 110; first tests equal, the difference is in the body (not yet analysed)
- `rsmerge-size-01` (analysed): rsmerge: 225 against 159; not yet analysed
- `tcmd-calls-01` (analysed): tcmd: the calls memname and getreg are arranged differently (not yet analysed)
- `clocal-size-01` (open (D15)): clocal: 230 against 264; block order of the NAME and SCONV branches (swap tried, rejected: alignment 47 -> 17 %)
- `header-rdsym-01` (closed): the header comment of rdr3.c gives rdsym at 0x1a86; the body at 0x1aae matches (strings and calls); the comment must be corrected in the source (kind source)
- `acon-tail-01` (explained): the original shares the printf and the off: tail between the three L%d branches; ours has the call in each branch
- `main-wrapper-01` (analysed): the original crt0 calls 0x4868, which passes argc and argv to the body at 0x023c; ours calls the body directly
- `lineout-type-01` (fixed (efcode3.c: long line -> int line; 100 % identical)): our lineout declares long line; the original takes int line (r7). main passes ifline (int) in r7 and ifname in r6. Ours reads the file from r5, so the file name
- `shift-type-02` (fixed except the order of zzzcode): the sign of a shift with a constant count differs (srl in the original, sra in ours, or the other way round): the variable shifted is unsigned in the original; 
- `zzzcode-order-01` (partly fixed): block order: the block if (notoff(...)) order(r, STAREG) lies at another place (808 ours, 920 original); 96 differing blocks, 86 % aligned
- `printf-wrapper-01` (fixed): the cgprintf wrapper (clears 400 bytes of stack before printf) was a workaround for the stray minus bug of the shipped printf
- `expand-layout-01` (explained: layout (D7)): order: 15 instructions at another place (ld rS,rS; call; ld F,rS; calr; inc rS,#1 moved before the loop body); length equal (169)
- `freereg-layout-01` (explained: layout (D7 goto/loop placement)): order: 6 instructions (ldb rlS,F; extsb; ld; calr; cp; jr le) at another place (ours 40, original 10); length 89 against 91
- `conval-layout-01` (fixed (source: case order)): order: 10 instructions (ldl rrS,F; cpl; ldk rS,#0; tcc eq,rS; jr) at another place; length 198 against 197

## Limits

- The D classes are assigned by patterns over the instruction forms, not by a separate test with the old cc. Layout and order are not compared.
- Pairings with header or string evidence were checked on the body only partly (see audit and the review notes in findings.json).
- The original is stripped; its function starts come from call targets and the instruction after a ret.
