# gopt: the compiler vc

/lib/gopt (03/83) is byte-identical when the sources of `src/cmd/cc/gopt` are compiled with -O by **vc** (`sccvc` from
OLDROOT: `/lib/vc/scparse`, `/lib/vc/scodgen`, `/lib/vc/spopt`, `/lib/old/cas`) and linked by `sldo` with the start-up code
of the library (`/lib/scrt0.o`) and `slibc-v12.a` (src/cmd/cc/gopt/Makefile). vc is v0p compiled for segmented code with
switches of its own (host/OLDCC.md, "The old scc of cas"); /bin/cas is built by the same compiler. The switches below are
needed by gopt; every one is neutral on cas; the measured witnesses of every switch on cas and gopt: `host/OLDCC_SWITCHES_VC.md`. Their mechanism in full: host/OLDCC.md section
3; the measurements: host/OLDCC_LOG.md.

A witness is a function that is exact only with the switch. The source forms that the functions need (dead code, gotos for
the block order, declaration order, chains) are in `findings.json`; a switch was only taken where no source form gives
the code of the original.

## vc switches (only in vc)

| Switch | Pass | What it does | Witnesses |
|---|---|---|---|
| `OLD_D2K` | scodgen match | a store through a scratch register forgets its name | addallist addblk addhdrlist alloclpdef asgfold betainit betanew include initallist labquad mkleaf |
| `OLD_D2FA` | scodgen regforsym | an automatic pointer or long is found in the pair an assignment named with its frame place (only a name matched alone; in a condition only when `\|\|` is not at the top) | asgfold clearblocks linkreachdefs liveonexit marklpblocks solvelive solvereach srchdef tmpsearch |
| `OLD_D24L` | scodgen local3.c | lda of a frame place without the mask and `#%7f00` (struct copies and struct arguments) | fndparm insrtlpdef insrtopstmp insrtstmpmove savecolors tmpsearch |
| `OLD_D3JL` | scodgen | a pointer parameter that comes back at a label gets no frame home, only a long does | canopt denorm eqopnd islive |
| `OLD_D3P` | scodgen regforsym | a name in a register is taken from it also under a cast between two types of the size of a pointer (pointer, long, unsigned long), one of them a pointer | tabfree |
| `OLD_D2A` (v0p) | scodgen match | the value of an assignment wanted in a register for a test is copied into a new pair; segmented a pointer in rr0 | ceval orderbl |
| `OLD_D2XP` | scodgen allocreg | OLD_D2X not into the pair of a parameter, and only when the registers allo takes are the result | constfold fndparm nodeno |
| `OLD_D2PP` | scodgen getreg | no pair free: an idle odd word holding a parameter is given back for a pair before regjam | clrlbit setlbit |
| `OLD_D2TC` | scodgen rbusy | a free word register is taken instead of stopping ("not found on rslist") | operand |
| `OLD_D2L` (v0) | scodgen getreg | an idle register that holds a name is free when it comes before the first free one (floats: by number) | constfold |
| `OLD_D14T` | scodgen gencall | when the result of a call goes to a temporary, the temporaries of its arguments are free again | initopts |
| `OLD_DNR` | scodgen reader | the register records F R S T of gopt (-O3, -O5) are not read: room in the 64 KiB of the vc scodgen | (none: room) |
| `OLD_D12R` | scparse | the static return area of a function that returns a structure goes into the data behind the function | constfold (data 7:5904) |
| `OLD_D50` | spopt | the loop inversion goes on at the target of a jump; not with one instruction between loop head and jump | tprint |
| `OLD_D51` (d), (e) | spopt codemove | (d) not a loop test named by two jumps; (e) not to a straight block behind a conditional jump when a later jump behind an unconditional one can take the block | chklpdefs (d), almap (e) |
| `OLD_D64` | spopt memreg | the offset of `\|_stkseg+~L1+4\|(fp)` is read behind `~L1` (the shipped spopt reads 0 and drops all register copies of frame slots) | domspace |

## Switches of v0 and v0p, changed for gopt

Each change keeps the v0p models (cparse, scparse, codgen, scodgen, popt, spopt, cpp) strictly equal and the v0 bench.

| Switch | Change | Witnesses |
|---|---|---|
| `OLD_D2N` | also a global loaded into a pair is not remembered | compress dominators eval joinblk linkp srchdflist |
| `OLD_D2P` | for a word an idle lighter pair is split, but not a pair that holds a parameter (v0, vc) | pr |
| `OLD_D2P` | no merge of the words of a parameter in r4..r7 (v0, vc) | testbit |
| `OLD_D2P` | segmented: the words of a parameter merge into a pair only when no pair is free | mkconstopnd |
| `OLD_D2Q` | segmented: a value loaded through `rrN(rM)` with an odd index goes into the pair rM-1 | prcgraph |
| `OLD_D2MO` | not for a register that holds a parameter (it is copied first) | canshare setedges testbit |
| `OLD_D2M` | a word divided too, when it is a name found in its register | affsize |
| `OLD_D2R` | segmented: the half OLD_D2V refuses goes back to the free list | chkfamindvar |
| `OLD_D3AW` | an idle parameter in the register of an argument that the argument does not use goes back | setposition |

## Open (HANDOFF step 10)

The switches with one or two witnesses are to be checked once more against source forms; deledge has a `goto` into a
loop that may stand for a layout of the old popt.
