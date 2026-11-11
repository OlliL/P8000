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
