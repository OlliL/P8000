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

## Missing features (the list of step 3)

### login (entered 2026-11-25, owner)

`compare/login` has only the cases that end before a user is logged in (10: unknown name, wrong password, name from
argv, `-`, NUL, blanks, long name, end of input; all on `terminal: console`). The rest of `src/cmd/login.c` needs:

| What login does | Emulator today (measured 2026-11-25 with the original, Z8K_UID=0, an overlay with passwd, utmp, wtmp) | Needed |
|---|---|---|
| `chroot(pw_dir)` for a shell field `*` (subsystem root), then `execl("/etc/login")` / `/bin/login` in it | sc 61 answers EPERM: "No Root Directory" | chroot (sc 61): a new guest root below ROOT for the process and its children (the paths, `..` at the new root, exec); EPERM for a user that is not root (sys4.c) |
| `open("/usr/adm/wtmp", O_WRONLY\|O_APPEND)`, write | the record overwrites the start of the file: wtmp stays 20 bytes after two logins | open with FAPPEND (the kernel's flag of O_APPEND): every write at the end |
| `/etc/utmp`: utscan finds the entry of `ttyname(0)` (console), writes the record over it; "No entry in utmp" otherwise | works (with the file in the case layer) | nothing; the corpus needs ROOT_COPY etc/utmp (and usr/adm/wtmp) and an utmp with a console entry |
| `getpass` on /dev/tty, `crypt`, a user with a password | works (console); the case needs an entry with a known crypt hash in a passwd of the case | a passwd of the corpus (ROOT_FILES or the layer) with a user and a known password |
| `chown(ttyn, uid, gid)`, `setgid`, `setuid`, `chdir(pw_dir)` | setgid/setuid/chown modelled with Z8K_UID; chown of a device of the devtab: to measure | to measure: chown and stat of /dev/console in the devtab model |
| no password: `execl("/bin/passwd", ...)` | works (passwd runs and asks) | nothing |
| the password age (`a64l` of pw_age, `time`): "Your password has expired", execl passwd | not measured | a passwd entry with an age field; Z8K_TIME |
| motd (`/etc/motd`, stopped by SIGINT through `catch`), "You have mail." (`stat` of /usr/spool/mail/NAME) | not measured | an interrupt to the console during the output (the console model has none yet) for the SIGINT case |
| `setenv`: HOME SHELL PATH LOGNAME TERM over the environment of getty; TERM from /etc/ttytype by the tty name | not measured | the environment of the step (`env:`); a shell that prints it |
| `execlp(pw_shell, "-csh", 0)`, the shell reads the console | not measured | the shell as the next step on the same console (csh with `-` in argv[0]: .login of HOME) |
| `alarm(60)` (argv[2]), SIGALRM: bailout, TCSETA with B0\|HUPCL, exit 1 | alarm and signals work; input that blocks is not possible (stdin is a file) | a console that waits (no EOF) for the timeout case |

When these are there: the cases of a successful login (shell and environment, utmp and wtmp records, the subsystem
root, the expired password, motd and mail, the timeout) go into `compare/login/tests.txt`.

## Done when

- Every corpus has the cases its man page and source call for, or a recorded reason why not (in its `findings.json`).
- No case is left out for an emulator limit that no longer exists.
- The list of the emulator's missing features is in this file, each either built or recorded as not to be built, with
  the reason.
