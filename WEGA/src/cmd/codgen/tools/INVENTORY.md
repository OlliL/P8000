# codgen inventory (status of the analysis)

Generated from findings.json by inventory.py. Method and limits: see classification_method and the notes below.

## Pairings

- functions of our codgen: 210
- confirmed with a counterpart in /lib/codgen: 206
- without counterpart on our side: `cgprintf`, `cgwipe`, `rcookie`, `start`
- original functions without counterpart on our side: `0x4868` (forwarder of main, main-wrapper-01), `0x6fca` and `0x854c` (dead code, quirk table)
- code coverage: every instruction of our codgen belongs to a function; in the original only the start code (0x0-0x28) lies outside the pairings, and the continuation of rdsym (0x1aae) belongs to rdsym

## Identical

Identical under metric strict-1 (the same instruction sequence, only addresses masked): 8 functions: `endrun`, `errexit`, `hdrout`, `rcount_reset`, `regreset`, `regtabinit`, `rnnullall`, `rnnullcall`

## Only D1-D14 (pattern-verified)

194 functions: every differing instruction matches a D pattern (D1, D2, D3, D5, D6, D7, D8, D10, D11, D13). The order of the instructions is not part of this check. D4, D9, D12 and D14 are not separable by these patterns.

## D1-D14 and real differences

12 functions with real differences (the D part is not separated from them): `acon`, `allocreg`, `buildstar`, `clocal`, `expand`, `fixfld`, `lineout`, `options`, `sucomp`, `tcopy`, `tshape`, `ttype`

## Real differences (findings)

- `allo3-rcookie-dup` (analysed): the original allocreg calls 0x89a2 (rallkind in comm3.c); we have a second copy of the same function in allo3.c
- `allo3-tcopy-copy-01` (analysed): original 0x8a24 calls ncopy (0x8a02) for the copy of the node; the rebuild copies inline (ldir)
- `ttype-shift-01` (merged into shift-type-01): the original shifts with srl (unsigned), ours with sra (signed)
- `fixfld-shift-01` (merged into shift-type-01): the original shifts the field value with srl, ours with sra
- `shift-type-01` (analysed-partly): three functions shift a field value with srl (unsigned) in the original and with sra (signed) in ours
- `mapnm-real-01` (analysed): pairing confirmed by strings; only 33 % of the instructions match; the statement order and the expressions differ (sdal / slal, the test order)
- `buildstar-order-01` (analysed): only 50 % match; the if chains come in another order
- `expand-loop-01` (checked): the original reads *cp into a byte and dispatches through a table; ours tests *cp and then dispatches
- `options-loop-01` (checked): the loop variable is a register in the original and in memory in ours; the layout of the switch differs
- `main-char-01` (checked): the original reads the input character as a byte; ours as int; the switch is a compare chain there (D6)
- `tshape-order-01` (checked): the call order special / shtemp / shumul / ltypeop differs; a long test is written differently (cpl and tcc in the original)
- `efcode-shift-01` (checked): sdal (original) against slal (ours) for the same scaling of sz; the sequence of the additions differs
- `sucomp-size-01` (analysed): sucomp: 756 instructions against 684; the difference is in the body (not yet analysed)
- `rsinsert-size-01` (analysed): rsinsert: 157 against 110; first tests equal, the difference is in the body (not yet analysed)
- `rsmerge-size-01` (analysed): rsmerge: 225 against 159; not yet analysed
- `tcmd-calls-01` (analysed): tcmd: the calls memname and getreg are arranged differently (not yet analysed)
- `clocal-size-01` (analysed): clocal: 230 against 264; the difference is in the body (not yet analysed)
- `header-rdsym-01` (closed): the header comment of rdr3.c gives rdsym at 0x1a86; the body at 0x1aae matches (strings and calls); the comment must be corrected in the source (kind source)
- `acon-tail-01` (analysed): the original shares the printf and the off: tail between the three L%d branches; ours has the call in each branch
- `main-wrapper-01` (analysed): the original crt0 calls 0x4868, which passes argc and argv to the body at 0x023c; ours calls the body directly
- `lineout-type-01` (analysed): our lineout declares long line; the original takes int line (r7). main passes ifline (int) in r7 and ifname in r6. Ours reads the file from r5, so the file name

## Limits

- The D classes are assigned by patterns over the instruction forms, not by a separate test with the old cc. Layout and order are not compared.
- Pairings with header or string evidence were checked on the body only partly (see audit and the review notes in findings.json).
- The original is stripped; its function starts come from call targets and the instruction after a ret.
