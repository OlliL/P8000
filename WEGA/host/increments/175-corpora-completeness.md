# 175 Completeness of all corpora, and the features the emulator still lacks

## Status

planned (owner 2026-11-25); not started. Runs at the end of the restoration, before the tag `WEGA_3.1`.

## Goal

The emulator was extended step by step (devices, signals, the console state, ...). Corpora written before such a step
left out cases "because the emulator does not support it", and these cases were in places never added once the
emulator could do it. This increment goes through every corpus (`compare/CORPORA`) once more:

- each case left out because of the emulator is added if the emulator can do it now;
- what the emulator still cannot do is collected in one list of missing features (like increment 162 for the kernel
  memory), with the programs and cases waiting for each feature;
- the features of that list are built in the emulator (one step per feature, `host/z8k-emu/README.md` kept current),
  and the waiting cases are added.

## Steps

1. Inventory: for every corpus, the cases left out or cut short because of the emulator. Sources: the comments in
   `corpus.py` and `tests.txt`, the notes in `findings.json`, `SYSIII.md`, the man page of the program against its
   cases (an option or a message without a case is suspect). A first grep (2026-11-25) for remarks about the emulator
   in `compare/` hits only a few corpora (e.g. df: "the emulator has no /dev/kmem (increment 162)"); most gaps are
   therefore not marked and have to be found from the man page and the source.
2. For each gap: can the emulator do it today (measure with a case), or which feature is missing.
3. The list of missing features, in this file: feature, programs and cases waiting for it, effort, order. Features
   already planned elsewhere (162: /dev/kmem, /dev/mem, /dev/swap) are referenced, not repeated.
4. Add every case that is possible now; then build the features in the order of step 3 and add their cases.
5. checkall ALL OK; `compare/CORPORA` and the corpora's `findings.json` current.

## Done when

- Every corpus has the cases its man page and source call for, or a recorded reason why not (in its `findings.json`).
- No case is left out for an emulator limit that no longer exists.
- The list of the emulator's missing features is in this file, each either built or recorded as not to be built, with
  the reason.
