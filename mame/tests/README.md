# Tests on the real P8000

Every directory is one test with its own README (what is tested, how, what comes out, what
it means and which claim it supports) and a `photos/` directory for pictures of the screen.

| Directory | Claim | Status |
|---|---|---|
| `01-saved-pc-segment-bit15` | bit 15 of the saved PC segment word is saved and restored by IRET | done on the real machine |
| `02-z8010-set-all-inhibit-flags` | Z8010 %15/%16 set the flags for any data byte | done on the real machine, as the emulator |
| `03-z8010-sar-wrap` | Z8010 SAR wraps 63 -> 0 | done on the real machine, as the emulator |
| `04-z8010-reset-command-10` | Z8010 %10 resets MR, VTR, DSCR | program ready |

Generators: `../tools/gen.py` (test 01), `../tools/gen_z8010.py` (tests 02-04).
Expected values are the emulator's, in the READMEs. Fill in the real results and add the
photos, then tick the points in `../TODO.md` section 4.
