# nm against System III

`/bin/nm` of WEGA (`nm.c 2.6 11/18/83`, ccvm, `/lib/libc.a`) against `cmd/nm.c` of System III (Plexus `cmd/nm.c` has the
same text). ZEUS 3.21 ships the same binary with the trailer `Zilog Inc`, without a source. WEGA's nm.c is a rewrite of the
System III one for s.out; the frame of System III is kept (the flags, the option cases, `compare`, `nextel`, the loop over the
files and archive members, the size computation of `-s`). The source was written from the binary. Measured 2026-11-25.

## WEGA's own

- The SCCS line `@[$]nm.c	2.6 11/18/83 13:50:10 - 87wega3.2`.
- s.out instead of a.out: `<s.out.h>`, `struct s_exec` and `struct s_nlist` (14 bytes); the symbols follow the image, its
  relocation (`SF_STRIP` clear: twice the image) and the segment table; `goodmagic` of the libc checks the magic number.
- `getopt` with `ngurposvl` instead of the loop over one option word; the usage `nm [-glnoprsuv] [file ...]`. Without a file:
  `s.out` when `swap_flg` is set, else `a.out` (see the quirks: always `a.out`).
- `-v`: the VDB types of s.out.h (`N_SIDENT` .. `N_GLOBAL`, letters `n h m x } { e + # p @ & $ * q ^ i l b d`) are printed,
  without `-v` they are skipped; `-v` also clears the sort flags and sets `-p`. Also new: `N_FN` `f`, `N_SN` `s`.
- `-l`: after the symbols the line table (`struct v_lines`: the address and the line number), or "no line table".
- Long names (the first byte `0x80 |` the length + 1, the characters run on into the following entries): read into the
  buffer behind `sym`, kept in a name table of 8000 bytes ("too many long names"), sorted through it by `compare`, printed
  by `putname` (more than 44 characters: 40 and `...`).
- `-s` refuses the segmented formats; for separate I&D (`S_MAGIC3`, `N_MAGIC3`, `X_MAGIC3`) the data size comes from the
  segment table (`datasize`).
- The values: `%08lx` when the high word is set, else `    %04x`; an undefined symbol gets a tab instead of blanks.
- Byte-swapped files: `swaphdr` for the header, `swab` for the symbols (dead, see the quirks table of KNOWLEDGE.md).
- `fread` with the size and the count the other way round (`fread(p, sizeof(x), 1, f)`), "Premature EOF" warnings.
