# poptveto: which single decisions of popt differ from the original

Scratch tools of the cas work (2026-11-11), kept as they were used; the paths inside name a scratch directory and must be
set before use.

- `z21_veto.patch`: popt with `-DVETO`. Every block move of codemove (M), loop inversion (I) and turn of a conditional jump
  over a jump (S) gets a number per function; `/tmp/veto.txt` of the guest root (`FUNCINDEX n n ...`) names the ones not to
  do, `/tmp/veto.log` lists what was offered (`function event type label`).
- `veto.py MODULE FUNC [FUNCINDEX [n ...]]`: the model with one module compiled again (`D12_ONLY`), strictcmp of FUNC;
  greedy search, `search2()` all pairs. Shows which decisions lead to the original: data for a rule or a source form.
- `par.py`, `jgen_jpr.py`: eight parallel work copies of the model, generated source forms of jpr.
- `bv.sh`, `cgb.sh`, `cpb.sh`: spopt, scodgen, scparse of vc with other switches into a private copy of OLDROOT.
- `realcas.sh`: the real build of src/cmd/cas with such a root, stripped, compared with /bin/cas.
- `pb.sh NAME "DEFINES"`: spopt of vc with the defines of the segvc rule replaced by DEFINES (so a switch can also be left
  out, which `bv.sh` cannot), into `o_NAME`; `PSED` (a sed script) or `PPY` (a python script taking the file) change z21.c
  of the copy first. `par.py` takes the base model from `$S/$PBASE` (default `t`).
- `pveto.py`: the veto search on eight slots in parallel (`setup(files, base, root)`, `bfs(module, func, fidx, depth)`); each slot
  has its own copy of the root, the model and the sources. The `tmp` of a root made by `make oldroot` is a symlink to the
  `tmp` of the WEGA root, shared by every copy: `setup` replaces it by a directory of its own, else all slots read the same
  `/tmp/veto.txt` (the earlier parallel results were wrong for that reason; sequential runs were right).
- `vpatch.py`, `vpatchF.py`: `PPY` scripts for `pb.sh` that apply `z21_veto.patch`; `vpatchF.py` also offers a forward loop
  inversion (the conditional jump stands behind the jump) as event `F`, done only when listed in `/tmp/veto.txt`.
- `pb.sh` replaces whatever defines the segvc rule has (`-DSPOPT ...`).
