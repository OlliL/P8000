# 162 Kernel memory in the emulator: /dev/kmem, /dev/mem, /dev/swap for the corpora

## Status

planned (owner 2026-11-24); not started.

## Goal

The programs that read the kernel's memory can be tested by their corpora on more than their start: today every such
step ends with "/dev/kmem: Cannot open" (df: `compare/df`, all cases but the options). The emulator gets a device model
for the kernel memory that serves, at the addresses of the kernel's symbols, data a case describes. So df can be run on a
mount table and a file system with a broken free list (`alloc`: a block under `s_isize` and one at or over `s_fsize`, the
two halves of its test, in the build of the originals and in the shipped one), and ps, pstat ... on a process table.

## Who reads the kernel (the WEGA root, scanned 2026-11-24)

| File | /dev/kmem | /dev/mem | /dev/swap | nlist of /wega |
|---|---|---|---|---|
| bin/df | x | | | x |
| bin/ps | x | x | x | x |
| bin/pstat | x | x | | x |
| bin/adb | x | x | | x |
| usr/bin/quot | x | | | x |
| usr/bin/timex | x | | | x |
| etc/kdb | x | | | x |
| etc/killall | | | | x |
| etc/sysgen | | | | x |

Each of them gets corpus cases once the model is there (step 4); which tables each one reads is found from its source or
its code then (df: `Nmount`, the mount table; ps: proc, user areas through /dev/mem and /dev/swap; pstat: inode, text,
proc, file tables; quot: none beyond the name list? to be read).

## Done when

- `Z8K_DEVTAB` has a model for kernel memory (name and form to be fixed in step 1) and `host/z8k-emu/README.md` describes it.
- A case can set kernel data by a readable description (symbol, type or bytes, value), not by hand-made binary files;
  the bytes are made by a host script at the start of the case.
- df has cases on a mount table and file system images: the free counts, the bad free block (both halves), the
  options with real data; they pass against the original, the build and the shipped build in checkall.
- The other programs of the table have their cases or a recorded reason why not yet.

## Read first

- `host/z8k-emu/README.md` (`Z8K_DEVTAB`, the models image, host, proc, console; "Users").
- `host/recon/corpusrun.py` (the case fields `device:`, `host:`), `compare/df`, `compare/fse` (a device model in use).
- The kernel headers of WEGA: `src/head/sys` (mount, proc, user, inode, filsys), `src/uts`; the name list of `/wega`.

## Steps

1. How the programs address the kernel memory: which symbols (from `nlist("/wega")`), which offsets they `lseek` to on
   `/dev/kmem` and `/dev/mem` (segmented addresses of the Z8001 kernel or offsets), what they read there and in what size.
   Measured with `Z8K_SC_TRACE` on the originals (df first). Fixes the model.
2. The model in the emulator: reads at an address are served from an image of the kernel memory that the case gives
   (sparse: unset bytes read as 0, or as an error, to be decided by step 1); writes refused (no program of the table
   writes, to be checked: adb, kdb can).
3. The description of a case and its host script: kernel symbols from the name list of the `/wega` the program reads,
   values by type from the headers (a mount entry, a proc entry), the result the image file of the model.
4. df: the cases above; then ps, pstat, quot, timex, killall, kdb, adb as far as a corpus makes sense; sysgen separately.
