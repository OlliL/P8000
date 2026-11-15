# oldslibc - the segmented libraries of the time of the old programs

The shipped library (`/lib/slibc.a`, sources in `../slibc`) is not the only slibc inside the original segmented programs: `/bin/sld`
and `/bin/cas` were linked from an slibc of their time. `make` builds them as `../slibc-vN.a`; the Makefile names per library its
members in the order of the original program, and for each module how it is made:

    <module>/0_<n>.s     assembler (cas syntax), cut from the original program, where a compiler or a source we do not have made it
    <module>/0_<n>.c     a source for this version
    tmp/<module>.o       the member of /lib/slibc.a (extracted by the Makefile), where the program holds the shipped one

The version numbers are invented (`0_1`: sld, `0_2`: cas); the same bytes have the same number. Built by the shipped `cas`
(bit 31 of an address constant clear, as the members of the time; not by the assembler of sccvc).

| Library | Program | Own modules |
|---|---|---|
| `slibc-v9.a` | sld (07/83) | printf 0_1, doprnt 0_1 (another revision than the shipped member) |
| `slibc-v11.a` | cas | fseek rdwr printf doprnt filbuf flsbuf endopen findiop perror cnvt length malloc mktemp strcpy strncpy strlen (0_2), errlst 0_2 |

How the modules were found and cut: `host/tools/oldslibc_cut.py` (sld; the data of printf and doprnt:
`host/tools/oldslibc_doprnt_data.py`), `host/tools/oldslibc_cas.py` (cas, text and data verified against `/bin/cas`); every other
module of the two programs is equal to the member of `/lib/slibc.a`. The libc part of sld and its link order:
`host/KNOWLEDGE.md`, "sld: cut of the original"; the library of cas: `host/OLDCC.md`, "The old scc of cas".

## State

1 of 19 modules from C: `errlst` 0_2 (`errlst/0_2.c`, by sccvc: the strings behind both tables, the data order of the old
compiler; assembled by the shipped `cas`). The two programs are byte-identical (checkall).

## Open

The 18 `.s`: not yet tried from C. printf 0_1, doprnt 0_1 (sld, 07/83, with EPU instructions) and the 0_2 of cas; candidates are
the C sources of `../slibc` and `../libc` with the old compilers (vl, vc, v1), measured as for `../oldlibc`.
