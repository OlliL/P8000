# 180 Whole system: self-hosted build, boot in MAME

## Status

open (last)

## Goal

Two tests of the whole: (1) the tree built a second time with the tools built from source (cc, cas, ld, ar, ...) gives the
same bytes; (2) a disk image of the install tree boots in the MAME P8000 machine and works (init, sh, login, commands).

## Done when

- A make target (or a checkall part) for the self-hosted build, byte-identical with the first build.
- A script that makes the disk image and a recorded boot in MAME with a list of checked commands.

## Read first

- `CLAUDE.md` (MAME fork, `mame/LOG.md` in the repository beside `WEGA/`).
- `host/tools/checkall.sh` (the install tree).
- `host/tools/mamerun.py` (MAME driven from Python: console on a socket, floppies in and out, NMI, exit), `mame/LOG.md`
  section 7 (the installation of WEGA in MAME), section 14 (tools, cautions).

## Known

- The install tree of checkall is byte-identical with the WEGA tree in every file `make install` puts there.
- How WEGA boots in MAME (measured 2026-11-16, increment 117; the owner's hint for the single-user mode):
  - MAME: `/mnt/share/git/mame/mame` (the fork with the P8000 machines), `floptool` beside it. Machine `p8000_16` (with
    the 16-bit board); ROMs in `/mnt/share/git/mame/roms/p8000_16`. The machine `p8000` (8-bit board only, UDOS) needs
    the `*_nur8bit` ROMs, which are in the owner's directory `~/Downloads/P8000/mame/roms` (flat; mamerun.py links them
    into `WORK/roms/p8000`).
  - A WEGA hard disk ready to boot: the owner's `~/Downloads/P8000/mame/harddisks/wega31_fresh_install_45MB.chd` (work on
    a copy, MAME writes into it). The WEGA start disk `w31start.cqm` (`~/Downloads/P8000/mame/floppies`) always in the
    first drive (`-flop1`).
  - Start without picture: `mame p8000_16 -rompath ... -video none -sound none -nothrottle -rs232 null_modem -bitb
    socket.127.0.0.1:PORT -flop1 w31start(.mfi) -hard wega.chd -autoboot_script ctl.lua` (mamerun.py does it; the
    console is a TCP connection to PORT).
  - The dialogue, from power on to the single-user shell: "U880-Softwaremonitor Version 3.1 - Press RETURN": RETURN;
    the 8-bit prompt `>`: RETURN (the start disk is booted); "U8000-Softwaremonitor Version 3.1 - Press NMI": no NMI
    (NMI would start the hardware test and the autoboot), RETURN instead: the 16-bit monitor answers `?` and its prompt
    `*`; `O U`: "BOOTING FROM UDOS FLOPPY", the prompt `> ` of the primary boot loader; `boot`: "Boot" and `: `;
    `md(0,16000)wega`: the kernel starts, prints the layout of the disk and "P8000 WEGA / Single-User Mode", the shell
    prompt is `#1 ` (a counter: csh).
  - Booted this way, the system was started from the start disk: the coupling software of the 8-bit part is loaded
    and `/dev/ud` works (getud, putud, format; fd(4), ud(4): not after a start by the monitor command `X`).
  - Input to WEGA (shell, login) needs 7E1, every character with even parity (`send(..., parity=True)`); the monitors
    and the boot loader take plain characters.
  - Multi-user mode (mame/LOG.md section 7, step 9): RETURN at the 8-bit monitor, then only NMI: after the hardware test
    the boot program starts the kernel from md(0,16000) by itself; login `wega`, password `root`.
  - A UDOS disk in the second drive at boot time makes UDOS stop (error C4, or C2 for an empty one): put it in after the
    boot (mamerun.py `insert`, the Lua command `swap`).
  - MAME writes the floppies back when it ends (MFI); `exit` through the Lua control lets it end properly. Never `pkill
    -f` (it kills the calling shell); stale MAME processes with a `ps | grep "[m]ame p8000"` loop in a script file.
  - The csh of the single-user shell: `$status`, not `$?`.

## Steps

1. Self-hosted build: the PATH of the build names the tools of the install tree instead of the originals.
2. Disk image from the install tree; boot in MAME; the list of commands.
3. Native build in MAME (owner 2026-11-25; the plan `future-project-structure.md` in the repository root, section
   "Native build in MAME"): a file system image that holds the git files of the tree (`wegafs.py`: a command that puts
   a whole directory tree is needed; where the image goes, a second disk or a partition, is to be measured in the MAME
   machine), boot single-user, mount it, run the complete build with the native make and tools, end MAME, take the
   install tree out of the image (`wegafs.py extract`), compare with the cross build. Not a must for this increment,
   but its tools (image, boot, mount) are made here and later move to `native/p8000-wega/` of ZEUS-WEGA.

## Decisions

## Not part of it

## Log
