# sum against System III

`/bin/sum` of WEGA against `cmd/sum.c` of System III (SCCS 2.1 07/28/82). Measured 2026-11-17.

## WEGA's own

- The SCCS line `static char sccsid[] = "@[$]sum.c	2.1  07/28/82 13:16:30 - 87wega3.2";` stands in the data; it is not in System III's text.

## Forms that follow the bytes, not the System III text

- The unions are declared `} suma, tempa;` (System III: `} tempa, suma;`): the order of the two in the bss.
