# libcisam: compiler switches outside the tree

Owner's decision (2026-11-18): libcisam is built from `src/lib/libcisam` with ccv1/sccv1 -O. Where no source form and no
existing switch (also narrowed) gives the original, a new v1 switch is worked out here as a diff with a description.
The diffs are NOT applied in the tree: v1 stays as it is and the libcisam members stay "open" in
`compare/libcisam/findings.json`. A later session tries to get rid of them (source forms, narrowing existing switches).

Each diff applies alone to the tree it was made on (git apply). They overlap in reader3.c, so together use `ALL.diff`
(all four switches, the Makefile lines included).

| Switch | File | Effect alone (libcisam lines nonseg/seg, label addresses masked) |
|---|---|---|
| `OLD_D3W` | reader3.c cbranch, QUEST | 38/44 > 16/44 (btree, open) |
| `OLD_D2PW` | allo3.c allocreg | 38/44 > 34/44 (item) |
| `OLD_D3Q` | reader3.c QUEST | 38/44 > 38/20 (item itunpack) |
| `OLD_D3H` | reader3.c cbranch, allo3.c allocreg | 38/44 > 34/34 (supp, item itpress) |
| all four (`ALL.diff`) | | 8/10: audit 4/4, read 4/6 |

`OLD_D63` (float compare) is in the tree already (checkall green).

## Measuring

Copy `tmp/oldroot` to a scratch root, build codgen/scodgen v1 with the switches into its `lib/v1` (as
`host/HANDOFF_libcisam.md` says: `make -f ../../Makefile codgen CFLAGS="-O -DOLD_D3 -DOLD_D61 -DOLD_D62 -DOLD_D63 ..."`
in `src/cmd/codgen/build/nonsegv1x`, scodgen with `-DSEG -DOLD_D24 -DOLD_D2CW` in `build/segv1x`), compile each member
with ccv1/sccv1 -O, disassemble with `host/tools/sdis.py` and compare with the members of `/usr/lib/libcisam.a` and
`slibcisam.a` with label addresses masked (`sed 's/L0[td]_[0-9a-fA-F]*/L/g'`).
