# oldlibc - the libraries of the time of the old programs

The shipped library (`/lib/libc.a`, sources in `../libc`) is not the only libc inside the original programs: each was linked from a
libc of its time. `make` builds them as `../libc-vN.a`; the Makefile names per library its members in the order of the original
programs, and for each module how it is made:

    <module>/0_<n>.c     a source for this version; the Makefile also uses sources of ../libc with another compiler
    <module>/<module>.c  a System III source (a module without its own 0_n.c)
    <module>/0_<n>.s     assembler (cas syntax), where a compiler we do not have made the module, or it was assembler in the original;
                         a 0_<n>.c beside it is a draft

The version numbers are invented (`0_1`, `0_2`, ...); the same bytes have the same number. What each version holds and why: `host/KNOWLEDGE.md`, "oldlibc".
A program finds its library with `host/tools/whichlibc.py`. `host/tools/libctable.py` measures which source and compiler give which of the
.s, `host/tools/libcfit.py` tries one source. How a library was found: the members of programs whose versions and order do not conflict.

## State

91 of 104 modules from C. Which program uses which library: `host/KNOWLEDGE.md`, "The libraries of the old programs".

## Open

The 13 `.s`, waiting for a compiler or a source form (`host/tools/libcfit.py`, `libctable.py`; what was tried:
`host/OLDCC_LOG.md`):

- With v1q (size): `doprnt` 0_1.
- With v0: `fseek` 0_2 (one statement: the registers at the label of an else, `OLD_D3J` fits it and not dc), `doprnt` 0_3.
- Libc of the passes: `malloc` 0_3 (a parameter reloaded at a label, the order of `q+temp-1`), `cnvt` 0_2 (`-val` of a long
  parameter from memory, one witness), `doprnt` 0_5, 0_7 (`_align` spills r5, `ap = argp` reloads, the order of a double
  argument), `fltioc` 0_1.
- Others: `swap` 0_1, `malloc` 0_4, `doprnt` 0_8 (0_1 with a smaller frame in `_strout`, libc-v10).
- Assembler in the original, these stay `.s`: `open` 0_1, `ecvt` 0_1.