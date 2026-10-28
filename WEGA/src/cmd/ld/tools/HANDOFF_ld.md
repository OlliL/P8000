# Handoff: /bin/ld (Phase A, not finished)

Status 2026-10-28. Program: `/bin/ld` (WEGA 3.1, 21732 bytes, nonsegmented E711, stripped, SCCS `ld.c 1.18`).
Predecessor of `src/cmd/sld`. Not committed. Process: `host/HOWTO_reconstruct.md`, Phase A steps 0 to 6.

## Facts (step 0)

- Header: magic E711, imsize 21692 = code 18944 (0x4a00) + data 2748 (0x0abc), bss 27082 (0x69ca), no symbols.
- Data strings (`host/tools/sstrings.py -n 4`): crt0.s 3.2, ld.c 1.18, copyname.c 1.3, printname.c 1.5, symbols.c 1.8.
  Messages match sld (`Premature EOF`, `Multiply defined`, `Bad output file`, ...). `sld` strings are a guide for names.
- Modules: crt0.s, ld.c, copyname.c, printname.c, symbols.c (from the SCCS tags, order of the data).

## Step 1: function starts (done, inventory)

- `src/cmd/ld/tools/inventory.py` (nonsegmented copy of the sld inventory): 115 proven starts, 23 candidates
  after ret/jump, 0 decode failures, 6130 linear instructions. Output: `/mnt/share/cvs/P8000/tmp/tmp/ld_inv.json`.
- `own_table.py` writes `src/cmd/ld/tools/own_functions.txt`: 62 own functions below 0x2c4c, 60 proven,
  2 candidates (0x0010, 0x0018). Status of all 62: `open`. Names: `f<entry>` (none understood yet).

## Step 2: libc and shipped members (done, identification)

- `host/tools/libcver.py` on `/bin/ld`: libc part from 0x2c4c. 27 members identical to `/lib/libc.a`:
  scanf ffltioc ungetc cuexit decimal isatty stty swab tolower strncmp access brk chmod close creat fstat getpid
  ioctl link lseek open read signal umask unlink write cerror.
- Not identical (older revision, as in sld): 0x2d28..0x4826, 17 pieces, best fingerprint matches are guesses
  (flsbuf, fltioc, rdwr, printf, doprnt, atoi, cnvt, length, system, swap, mktemp, rindex, strcat, strncpy, setbuf).
  Not decided: these need a cut like `src/lib/oldslibc` (sld) before they can be linked.

## Steps 3 to 6: not started

- Step 3 (own code as one block): not done.
- Step 4 (data and bss): not done; sizes known (data 2748, bss 27082), addresses not checked yet.
- Step 5 (C functions, 3 attempts each): no C draft written yet. 0 of 62 functions done.
- Step 6 (corpus harness): not written. The sld harness (`host/ld/check.py` with `SLD_NEW`) is the model.
- Makefile, module split: not started.

## Open questions

- The unidentified libc pieces 0x2d28..0x4826: cut as oldlibc (`host/tools/oldslibc_cut.py` style) or as own code?
- Module boundaries inside 0x0000..0x2c4c: open (sld had the same problem; use the SCCS order of the data).

## Files of this step

- `tools/inventory.py`, `tools/own_table.py`, `tools/progcfg.py`, `tools/own_functions.txt`, `tools/HANDOFF_ld.md`.
