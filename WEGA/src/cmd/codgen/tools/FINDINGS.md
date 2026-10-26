# codgen findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `codgen` compared with `/lib/codgen`; pending findings are the open items of the analysis.

## Summary

**Functions:** 206

| Pairing | Count |
|---|---|
| confirmed | 206 |

| Difference class | Count |
|---|---|
| D only (pattern-verified) | 202 |
| other differences | 4 |

**Pending findings:** 24

| Status | Count |
|---|---|
| analysed | 5 |
| checked | 2 |
| closed | 1 |
| explained | 2 |
| explained: layout | 1 |
| fixed | 1 |
| fixed (allo3.c: rcookie removed, allocreg calls rallkind) | 1 |
| fixed (allo3.c: tcopy calls ncopy(q, p)) | 1 |
| fixed (efcode3.c: long line -> int line; 100 % identical) | 1 |
| fixed (main3.c: register int i, err; 76 %, the rest D) | 1 |
| fixed except the order of zzzcode | 1 |
| fixed for ttype; the rest is shift-type-02 | 1 |
| merged into shift-type-01 | 2 |
| open | 1 |
| partly fixed | 3 |

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| acon | 0x2e0a-0x2fdc | 0x437e | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| adrput | 0x320e-0x33ba | 0x41d0 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8 |  |
| allo | 0x05a2-0x0950 | 0x67e8 | confirmed | differs | D only (pattern-verified) | D10, D13, D2, D6, D7, D8/D2 |  |
| alloc | 0x878c-0x8808 | 0x8b6e | confirmed | differs | D only (pattern-verified) | D1 |  |
| allocinit | 0x8808-0x8826 | 0x8bea | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| allocreg | 0x0494-0x05a2 | 0x6c4e | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D7, D8/D2 |  |
| argplace | 0x2c36-0x2db6 | 0x5d2c | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D5, D6, D7, D8/D2 |  |
| argprep | 0x71e0-0x72d0 | 0x18be | confirmed | differs | D only (pattern-verified) | D2, D3, D7 |  |
| argscan | 0x293a-0x2aba | 0x5a4c | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D7, D8/D2 |  |
| argsize | 0x130c-0x1366 | 0x88f6 | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| argsort | 0x2aba-0x2b1a | 0x5bcc | confirmed | differs | D only (pattern-verified) | D3 |  |
| argstrip | 0x2b84-0x2c36 | 0x5c7a | confirmed | differs | D only (pattern-verified) | D1 |  |
| bcon | 0xb660-0xb692 | 0xb0a0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| branch | 0x8762-0x8776 | 0x89e8 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| bsize | 0x0eae-0x0efc | 0x8878 | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D6, D7 |  |
| buildstar | 0xbb0a-0xbbbc | 0xafe4 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| canon | 0x71a6-0x71e0 | 0x1884 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| canon2 | 0x778e-0x77aa | 0x46fe | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| cbgen | 0x63a8-0x6412 | 0x460e | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D6, D8/D2 |  |
| cbranch | 0x6c20-0x6eec | 0x12bc | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8/D2 |  |
| cerror | 0x8428-0x84aa | 0x85ae | confirmed | differs | D only (pattern-verified) | D10, D13, D7, D8 |  |
| chk | 0xb692-0xb69a | 0xab3e | confirmed | differs | D only (pattern-verified) |  |  |
| clocal | 0xb69a-0xb9a0 | 0xab48 | confirmed | differs | other differences | D15 | clocal-size-01 (D15): 80 instructions at another place (block order of the branches) |
| codgen | 0x6412-0x645c | 0x0aaa | confirmed | differs | D only (pattern-verified) | D1, D7 |  |
| conput | 0x3016-0x3102 | 0x3fda | confirmed | differs | D only (pattern-verified) | D13, D2, D3, D6, D7, D8, D8/D2 |  |
| constore | 0x6b2c-0x6b68 | 0x11da | confirmed | differs | D only (pattern-verified) | D1, D6 |  |
| conval | 0x7b3c-0x7dba | 0xb774 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8/D2 |  |
| convelim | 0x796a-0x7a98 | 0xba7c | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D8, D8/D2 |  |
| deflab | 0x2926-0x293a | 0x59de | confirmed | differs | D only (pattern-verified) |  |  |
| delay | 0x7430-0x753c | 0x0838 | confirmed | differs | D only (pattern-verified) | D13, D7 |  |
| delay1 | 0x73a8-0x7430 | 0x094e | confirmed | differs | D only (pattern-verified) | D1, D6 |  |
| delay2 | 0x7304-0x73a8 | 0x0a0a | confirmed | differs | D only (pattern-verified) | D1, D6 |  |
| delstrip | 0x72d0-0x7304 | 0x09d2 | confirmed | differs | D only (pattern-verified) | D2, D3, D8/D2 |  |
| dopeinit | 0x86be-0x86ee | 0x8844 | confirmed | differs | D only (pattern-verified) |  |  |
| efcode | 0xa10e-0xa1c4 | 0x28f6 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3 |  |
| endline | 0xa1d6-0xa224 | 0x8c22 | confirmed | differs | D only (pattern-verified) |  |  |
| endrun | 0xa224-0xa228 | 0x8c1e | confirmed | exact | D only (pattern-verified) |  |  |
| epilog | 0x9e8a-0x9edc | 0x2990 | confirmed | differs | D only (pattern-verified) | D1, D13, D3 |  |
| eread | 0x9346-0x9542 | 0x2480 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| errexit | 0x874a-0x8762 | 0x89ce | confirmed | exact | D only (pattern-verified) |  |  |
| expand | 0x343a-0x362e | 0x6444 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| fbegin | 0xa550-0xa684 | 0x8fda | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3 |  |
| fcmd | 0xacb2-0xad46 | 0xa1ba | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| ffld | 0x6eec-0x7082 | 0x15d0 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| findtemp | 0xac80-0xacb2 | 0xa7ac | confirmed | differs | D only (pattern-verified) | D1 |  |
| fixfld | 0xa878-0xa936 | 0x944a | confirmed | differs | D only (pattern-verified) | D1, D3, D7, D8, D8/D2 |  |
| fixname | 0xab64-0xabca | 0xa0c6 | confirmed | differs | D only (pattern-verified) | D1, D10 |  |
| fixstarg | 0x2b1a-0x2b84 | 0x5c1c | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| fixtemp | 0xa6fe-0xa878 | 0x918e | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D7, D8, D8/D2 |  |
| foldadd | 0xba3a-0xbb0a | 0xaf14 | confirmed | differs | D only (pattern-verified) | D1 |  |
| fpdec | 0x9cd8-0x9cf8 | 0x2b20 | confirmed | differs | D only (pattern-verified) | D2, D3, D7 |  |
| fpinc | 0x9cb8-0x9cd8 | 0x2aa2 | confirmed | differs | D only (pattern-verified) | D2, D3, D7 |  |
| free1 | 0x85b8-0x85d4 | 0x8758 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| freeparm | 0x025a-0x02ac | 0x6792 | confirmed | differs | D only (pattern-verified) | D1, D13 |  |
| freereg | 0x037c-0x0494 | 0x6d5a | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| freetemp | 0x02ac-0x02fc | 0x6ba6 | confirmed | differs | D only (pattern-verified) | D1, D2 |  |
| funcend | 0x9c5c-0x9cb8 | 0x289a | confirmed | differs | D only (pattern-verified) | D1 |  |
| fwalk | 0x8668-0x86be | 0x87ec | confirmed | differs | D only (pattern-verified) | D1 |  |
| fwalkd | 0x85ee-0x8668 | 0x8774 | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D7 |  |
| genargs | 0x2db6-0x2e0a | 0x59f4 | confirmed | differs | D only (pattern-verified) | D1, D7 |  |
| gencall | 0x7620-0x76fa | 0x4534 | confirmed | differs | D only (pattern-verified) | D10 |  |
| geninc | 0x5592-0x562a | 0x2e4c | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D6 |  |
| genscall | 0x76fa-0x7708 | 0x4520 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| getlab | 0x2914-0x2926 | 0x59d4 | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| getlr | 0x1658-0x16e8 | 0x6636 | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D6, D7, D8 |  |
| getreg | 0x4008-0x436e | 0x7c10 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| gswitch | 0xb2d8-0xb600 | 0xa7e0 | confirmed | differs | D only (pattern-verified) | D2, D3, D7, D8/D2 |  |
| hdrout | 0xa1c4-0xa1d6 | 0x8c0c | confirmed | exact | D only (pattern-verified) |  |  |
| hopcode | 0x33ba-0x3426 | 0x2de0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| icright | 0x7766-0x778e | 0x46d0 | confirmed | differs | D only (pattern-verified) | D1, D3, D7, D8/D2 |  |
| init | 0x96b6-0x9748 | 0x019e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| insput | 0x3102-0x3112 | 0x4088 | confirmed | differs | D only (pattern-verified) |  |  |
| isint | 0x7950-0x796a | 0xbba2 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| ispow2 | 0x0e5e-0x0eae | 0x8af6 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6 |  |
| lastchance | 0x7742-0x7766 | 0x46ae | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| lineout | 0x9c44-0x9c5c | 0x2880 | confirmed | differs | D only (pattern-verified) |  |  |
| lmuldiv | 0x25fa-0x263c | 0x576e | confirmed | differs | D only (pattern-verified) | D1, D3, D7, D8/D2 |  |
| ltypeok | 0x118c-0x11d4 | 0x3e1c | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D7 |  |
| ltypeop | 0x0e32-0x0e5e | 0x3e66 | confirmed | differs | D only (pattern-verified) | D3, D7, D8/D2 |  |
| main | 0x9748-0x9c44 | 0x023c | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| makeblock | 0xb600-0xb660 | 0xaece | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3 |  |
| mapn | 0x8fa0-0x8fd0 | 0x2038 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| mapnm | 0x9020-0x9346 | 0x20c0 | confirmed | differs | D only (pattern-verified) | D10, D13, D2, D3, D7, D8 |  |
| mapregs | 0xa936-0xaa36 | 0x9512 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| mapsym | 0x8fd0-0x9020 | 0x206a | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| markcall | 0x6b68-0x6bd2 | 0x1212 | confirmed | differs | D only (pattern-verified) | D1, D3, D6, D7, D8/D2 |  |
| match | 0x16e8-0x1962 | 0x6196 | confirmed | differs | D only (pattern-verified) | D13, D2, D3, D6, D7, D8, D8/D2 |  |
| member | 0xb9a0-0xba3a | 0xb0d4 | confirmed | differs | D only (pattern-verified) | D10, D2, D3, D7 |  |
| memname | 0xaf94-0xb00a | 0xa734 | confirmed | differs | D only (pattern-verified) | D1, D13, D7, D8 |  |
| mkadrs | 0x782c-0x7950 | 0x48a0 | confirmed | differs | D only (pattern-verified) | D1, D3, D7, D8/D2 |  |
| mkhash | 0x8944-0x8980 | 0x2406 | confirmed | differs | D only (pattern-verified) | D13, D2 |  |
| mkrall | 0x20ec-0x214e | 0x51c0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| mycanon | 0x77aa-0x780e | 0x4804 | confirmed | differs | D only (pattern-verified) | D1 |  |
| ncopy | 0x8776-0x878c | 0x8a02 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| newnode | 0x384a-0x38b8 | 0x75e0 | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D6, D7, D8 |  |
| nextcook | 0x7708-0x7742 | 0x4674 | confirmed | differs | D only (pattern-verified) | D1 |  |
| niceuty | 0x23ac-0x23f6 | 0x547a | confirmed | differs | D only (pattern-verified) | D1, D10, D2 |  |
| nlcheck | 0x890c-0x8944 | 0x244a | confirmed | differs | D only (pattern-verified) | D2, D8 |  |
| notoff | 0x1962-0x1992 | 0x49c0 | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D8 |  |
| nullsyms | 0x452c-0x4570 | 0x811e | confirmed | differs | D only (pattern-verified) | D13, D3, D7, D8, D8/D2 |  |
| nxwork | 0x4cfc-0x542a | 0x98e4 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8/D2 |  |
| offstar | 0x232c-0x23ac | 0x53fa | confirmed | differs | D only (pattern-verified) | D1 |  |
| optim | 0x7dba-0x83d0 | 0xb178 | confirmed | differs | D only (pattern-verified) | D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| options | 0x9542-0x96b6 | 0x002a | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7 |  |
| order | 0x6506-0x69d2 | 0x0ba2 | confirmed | differs | D only (pattern-verified) | D10, D13, D2, D3, D6, D7, D8/D2 |  |
| orderadd | 0x226a-0x232c | 0x5338 | confirmed | differs | D only (pattern-verified) | D1 |  |
| oreg2 | 0x7082-0x71a6 | 0x175e | confirmed | differs | D only (pattern-verified) | D10, D3, D7 |  |
| oregfix | 0xabca-0xac80 | 0xa12a | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| oregput | 0x2fdc-0x3016 | 0x409a | confirmed | differs | D only (pattern-verified) | D1 |  |
| pairmate | 0x4570-0x4588 | 0x73f0 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| parminit | 0xa228-0xa4f4 | 0x8c74 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| plainicon | 0x0f2c-0x0f56 | 0x8b44 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| prcook | 0x645c-0x6506 | 0x0af6 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D7, D8 |  |
| prline | 0x83d0-0x83ec | 0x8952 | confirmed | differs | D only (pattern-verified) |  |  |
| prlong | 0x3426-0x343a | 0x3fc4 | confirmed | differs | D only (pattern-verified) |  |  |
| prolog | 0xa0b4-0xa10e | 0x2aca | confirmed | differs | D only (pattern-verified) | D1, D13, D3 |  |
| psuffix | 0x5580-0x5592 | 0x2780 | confirmed | differs | D only (pattern-verified) | D3, D7 |  |
| rallkind | 0x871e-0x874a | 0x89a2 | confirmed | differs | D only (pattern-verified) | D1 |  |
| rallo | 0x214e-0x226a | 0x5222 | confirmed | differs | D only (pattern-verified) | D1, D13, D3, D6, D7 |  |
| rbusy | 0x00b4-0x018a | 0x66bc | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| rclass | 0x002a-0x007a | 0x6bf4 | confirmed | differs | D only (pattern-verified) | D10, D2, D3, D7, D8/D2 |  |
| rcmd | 0xade6-0xaf94 | 0xa2f4 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D7, D8/D2 |  |
| rcount | 0x0e1a-0x0e30 | 0x15ba | confirmed | differs | D only (pattern-verified) |  |  |
| rcount_reset | 0x0e30-0x0e32 | 0x778e | confirmed | exact | D only (pattern-verified) |  |  |
| rddata | 0x8980-0x8a46 | 0x19ac | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D3, D5, D7, D8 |  |
| rdin | 0x88fe-0x890c | 0x269a | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| rdin2 | 0x8826-0x88fe | 0x26ae | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D8 |  |
| rdsym | 0x8d68-0x8e4a | 0x1a86 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D7, D8/D2 |  |
| readmems | 0x8bb8-0x8d68 | 0x1d0e | confirmed | differs | D only (pattern-verified) | D8/D2 |  |
| readnums | 0x8b1e-0x8bb8 | 0x1c6c | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8/D2 |  |
| readsyms | 0x8a46-0x8af0 | 0x1b64 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D5, D7, D8/D2 |  |
| readtab | 0x8af0-0x8b1e | 0x1c30 | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| reclaim | 0x0a18-0x0dc8 | 0x7058 | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D7, D8/D2 |  |
| regargs | 0x9edc-0xa0b4 | 0x2c0e | confirmed | differs | D only (pattern-verified) | D10, D13, D2, D3 |  |
| regforsym | 0xaad8-0xab64 | 0xa03a | confirmed | differs | D only (pattern-verified) | D1, D13, D2 |  |
| regjam | 0x3c48-0x4008 | 0x7916 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| regreset | 0xad46-0xade6 | 0xa252 | confirmed | exact | D only (pattern-verified) |  |  |
| regrestore | 0x9cf8-0x9dc0 | 0x29de | confirmed | differs | D only (pattern-verified) | D1, D13, D3 |  |
| regsave | 0x9dc0-0x9e8a | 0x2b48 | confirmed | differs | D only (pattern-verified) | D1, D13, D3 |  |
| regtabinit | 0xa684-0xa68c | 0x910e | confirmed | exact | D only (pattern-verified) |  |  |
| rfind | 0x007a-0x00b4 | 0x96e6 | confirmed | differs | D only (pattern-verified) | D1 |  |
| rfree | 0x018a-0x01de | 0x6f76 | confirmed | differs | D only (pattern-verified) | D1 |  |
| rfreeall | 0x0dc8-0x0e1a | 0x9696 | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D8/D2 |  |
| rfreenode | 0x01de-0x025a | 0x6efa | confirmed | differs | D only (pattern-verified) | D1 |  |
| rmove | 0x11d4-0x130c | 0x3cf2 | confirmed | differs | D only (pattern-verified) | D1, D3, D6, D7, D8/D2 |  |
| rnassign | 0x4b28-0x4be0 | 0x9722 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D6, D7 |  |
| rnnull | 0x4caa-0x4cfc | 0x989e | confirmed | differs | D only (pattern-verified) | D2, D3, D7 |  |
| rnnullall | 0x4c52-0x4c7c | 0x983c | confirmed | exact | D only (pattern-verified) |  |  |
| rnnullcall | 0x4c2c-0x4c52 | 0x9816 | confirmed | exact | D only (pattern-verified) |  |  |
| rnnulll | 0x4c7c-0x4c9e | 0x986a | confirmed | differs | D only (pattern-verified) | D1 |  |
| rnnullp | 0x4be0-0x4c2c | 0x97cc | confirmed | differs | D only (pattern-verified) | D1, D7 |  |
| rnnullw | 0x4c9e-0x4caa | 0x988c | confirmed | differs | D only (pattern-verified) | D3 |  |
| roverlap | 0x3ac2-0x3c48 | 0x7790 | confirmed | differs | D only (pattern-verified) | D1, D3, D6 |  |
| rsbuild | 0x379e-0x384a | 0x7458 | confirmed | differs | D only (pattern-verified) | D2, D7 |  |
| rsdump | 0x4588-0x465c | 0x750c | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D7, D8 |  |
| rsfreeall | 0x465c-0x46c2 | 0x815e | confirmed | differs | D only (pattern-verified) | D13, D2, D3, D6, D7, D8/D2 |  |
| rsinit | 0x3714-0x3762 | 0x740c | confirmed | differs | D only (pattern-verified) | D1, D13, D5 |  |
| rsinsert | 0x38b8-0x3ac2 | 0x7650 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| rsmatch | 0x4a08-0x4a96 | 0x83ec | confirmed | differs | D only (pattern-verified) | D1, D7 |  |
| rsmerge | 0x46f6-0x4a08 | 0x81fe | confirmed | differs | D only (pattern-verified) | D1, D2, D3, D7, D8, D8/D2 |  |
| rspush | 0x46c2-0x46f6 | 0x81c4 | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| rsrelall | 0x4a96-0x4ae0 | 0x84b6 | confirmed | differs | D only (pattern-verified) | D7 |  |
| rsrelease | 0x3762-0x379e | 0x847a | confirmed | differs | D only (pattern-verified) | D1 |  |
| rsrestart | 0x4ae0-0x4b28 | 0x8502 | confirmed | differs | D only (pattern-verified) | D7 |  |
| rsreturn | 0x436e-0x452c | 0x7f50 | confirmed | differs | D only (pattern-verified) | D13, D2, D3, D6, D7, D8, D8/D2 |  |
| scmd | 0xb00a-0xb140 | 0xa4aa | confirmed | differs | D only (pattern-verified) | D10, D13, D2, D3, D6, D7, D8/D2 |  |
| setasg | 0x286a-0x2914 | 0x56c6 | confirmed | differs | D only (pattern-verified) | D1, D2 |  |
| setasop | 0x263c-0x286a | 0x57b0 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3 |  |
| setbin | 0x23f6-0x25a0 | 0x54c4 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| setrew | 0x15a6-0x1658 | 0x60e2 | confirmed | differs | D only (pattern-verified) | D13, D2, D3, D6, D7, D8, D8/D2 |  |
| setstr | 0x25a0-0x25fa | 0x5678 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| shincr | 0x1092-0x1112 | 0x3ed6 | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| shtemp | 0x1034-0x1092 | 0x3e8e | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D6, D7, D8, D8/D2 |  |
| shumul | 0x1112-0x118c | 0x3f4e | confirmed | differs | D only (pattern-verified) | D1, D10, D2 |  |
| sortsyms | 0x8e4a-0x8ee4 | 0x1ec2 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D8/D2 |  |
| special | 0x0f56-0x1034 | 0x471a | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D7, D8/D2 |  |
| stmt | 0x753c-0x7620 | 0x075a | confirmed | differs | D only (pattern-verified) | D1, D2 |  |
| stoarg | 0x6bd2-0x6c20 | 0x126a | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| stoasg | 0x780e-0x782c | 0x487e | confirmed | differs | D only (pattern-verified) | D1, D2, D3 |  |
| store | 0x69d2-0x6b2c | 0x1084 | confirmed | differs | D only (pattern-verified) | D1, D6 |  |
| structretcode | 0xa4f4-0xa550 | 0x8f7e | confirmed | differs | D only (pattern-verified) |  |  |
| sucomp | 0x1992-0x20ec | 0x49f0 | confirmed | differs | other differences |  | sucomp-size-01: 55 add rS,F forms (a frame slot as the second operand; the original loads the value into a register first); locals now register variables (source change) |
| symreg | 0xaa80-0xaad8 | 0x9fdc | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8/D2 |  |
| symuse | 0xaa36-0xaa80 | 0x964c | confirmed | differs | D only (pattern-verified) | D1 |  |
| szty | 0x0efc-0x0f2c | 0x88c4 | confirmed | differs | D only (pattern-verified) | D1, D3, D6 |  |
| tabclear | 0xa6c6-0xa6fe | 0x9152 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| tabfind | 0xa68c-0xa6c6 | 0x9116 | confirmed | differs | D only (pattern-verified) | D1 |  |
| tagfind | 0x8ee4-0x8fa0 | 0x1f82 | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| talloc | 0x8516-0x8574 | 0x869c | confirmed | differs | D only (pattern-verified) | D13, D3, D7 |  |
| tbytes | 0x86ee-0x871e | 0x8970 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D6, D7 |  |
| tcheck | 0x8574-0x85b8 | 0x86fa | confirmed | differs | D only (pattern-verified) | D2 |  |
| tcmd | 0xb140-0xb258 | 0xa5de | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| tcopy | 0x0950-0x0a18 | 0x8a24 | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D7, D8/D2 |  |
| tfree | 0x85d4-0x85ee | 0x873c | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| tinit | 0x84e6-0x8516 | 0x866c | confirmed | differs | D only (pattern-verified) |  |  |
| tmpcleanup | 0x542a-0x552c | 0x9308 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| tmpdump | 0x552c-0x5580 | 0x93f4 | confirmed | differs | D only (pattern-verified) | D2 |  |
| treeeq | 0x7a98-0x7b3c | 0xb9ea | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8/D2 |  |
| tshape | 0x1366-0x14ba | 0x5e92 | confirmed | differs | other differences |  | tshape-order-01: SZERO value form only (cpl + tcc in the original, a branch in ours); the call order is fixed by the case order (FLD before STAR) |
| tsuffix | 0x362e-0x3714 | 0x2796 | confirmed | differs | D only (pattern-verified) | D13, D2, D3, D7, D8 |  |
| ttype | 0x14ba-0x15a6 | 0x5ff6 | confirmed | differs | D only (pattern-verified) | D1, D3, D6 |  |
| uerror | 0x83ec-0x8428 | 0x8570 | confirmed | differs | D only (pattern-verified) |  |  |
| upput | 0x3112-0x320e | 0x40d4 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8 |  |
| usable | 0x02fc-0x037c | 0x6e78 | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D7 |  |
| werror | 0x84aa-0x84e6 | 0x862e | confirmed | differs | D only (pattern-verified) |  |  |
| zzzcode | 0x562a-0x63a8 | 0x2edc | confirmed | differs | other differences | D15 | zzzcode-order-01 (D15): block layout of the structure move; the n <= 4 / else branches were swapped (95 -> 80 differing blocks); the if (!tshape(r, STAREG)) block of the big branch still lies elsewhere (orig 920, ours 809 before the swap) |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
| allo3-rcookie-dup | source | rcookie | fixed (allo3.c: rcookie removed, allocreg calls rallkind) |  | delete rcookie in allo3.c, declare rallkind as extern in allo3.c and call it from allocreg | The original allocreg calls 0x89a2 (rallkind). The change itself is invisible to the pattern check, because call targets are masked; the fix follows from the call target. |
| allo3-tcopy-copy-01 | source | tcopy | fixed (allo3.c: tcopy calls ncopy(q, p)) |  | call ncopy(q, p) instead of the struct assignment in tcopy | Experiment: *q = *p -> ncopy(q, p): tcopy 69 -> 67 instructions, the rest is D (0 unexplained). |
| ttype-shift-01 | source | ttype | merged into shift-type-01 |  | make the shifted value unsigned, check which variable's type gives srl |  |
| fixfld-shift-01 | source | fixfld | merged into shift-type-01 |  | same type check as ttype-shift-01 |  |
| shift-type-01 | source | ttype, fixfld, buildstar | fixed for ttype; the rest is shift-type-02 |  | find the variable whose type is unsigned in the original (a bit field or an unsigned member); make the shift unsigned in all three | Experiment (see before): unsigned operand in DECREF/INCREF and ttype gives srl as in the original; ttype pattern-explained after it. fixfld and buildstar: per site, not yet changed. |
| mapnm-real-01 | source | mapnm | analysed | D-classes (mostly) | classify each block against the original, then fix the source | reclassified: after removing the known D-classes (frame and spill offsets, registers, shift forms, data addresses) mapnm aligns to 87 %. The remaining differences are D-classes too, not a source difference found so far (13 % open). |
| buildstar-order-01 | source | buildstar | explained: layout | D7-like layout (else branch out of line, jumps back to the shared store) | none: layout | Both branches reach the same store block; the original places the UAND branch after the store and jumps back, ours falls through into it. Same instructions, only the layout. Swap of the two tests tried earlier: 59 -> 62 % only. Source unchanged; the shift is fixed (srl #2 in both). |
| expand-loop-01 | source | expand | explained | D7 + D2 | none: compiler differences D7 and D2 | D7 (loop test at the bottom in the original: entry jp 661A into the test; ours tests at the top) and D2 (the switch character is loaded into rl2 in the original, into rl3 and copied to r2 in ours). Same length 169 / 169; the moved blocks follow from the loop layout. Source unchanged. |
| options-loop-01 | source | options | fixed (main3.c: register int i, err; 76 %, the rest D) | real (source) | declare i and err as register variables in options() (main3.c); verify the byte identity of options afterwards | Experiment: register int i, err in main3.c: 69 -> 76 %, the differences left are D classes (0 unexplained). i alone is not enough (75 %). |
| main-char-01 | source | main | checked | D6 + D8 | none in the source; check with the old cc reconstruction | not a type difference: the getc expansion occurs 10 times in both (10 / 10), calls 66 / 66; the byte loads (10 / 18) and masks (7 / 8) differ, which is D8; the switch as a compare chain is D6. |
| tshape-order-01 | source | tshape | partly fixed | fixed: case order (source); open: SZERO value form | verify the form of the SZERO return (value via tcc) in the original | Case order: FLD case placed before STAR in match3.c (the original's bodies are in this order: call order special, shtemp, ltypeop, shumul now equal). Alignment 56 -> 74 %, pattern diff 102 -> 60. Corpus 101/101 equal. Open: the last operand of the SZERO return is a value (cpl rr2,#0; ldk r2,#0; tcc eq,r2) in the original and a branch in ours; the if-form had no gain. Also tried: `!p->lval` and parenthesised && (117 instructions, 0.776), `? 1 : 0` (113, 0.754), local variable (117, 0.759); no tcc/cpl in any variant. |
| efcode-shift-01 | source | efcode | checked | D10 | none in the source; D10 in the old cc reconstruction | the same divisions (divl by 8 and by 2) in both; the multiplication by 2 is sdal with a register count in the original and slal #1 in ours (long shift form, D10); register and spill differ (D2, D4). |
| sucomp-size-01 | other | sucomp | partly fixed | fixed: locals register (source); open: add with frame operand | find the expression that makes the old compiler add a frame slot directly (49 places); the register declaration is only an approximation | Source change in order3.c: the locals of sucomp (o, arity, nr, nf, sul, sur, fl, fr, t, l, r) are register variables, as in other functions of the module. Memory loads 226 -> 54, unexplained add forms 91 -> 55, alignment 36 -> 46 %, length 756 -> 731 (original 684). Corpus 101/101 equal. Earlier experiment (temporaries for l->su, l->fsu) had no effect. Open: add rS,F with a frame slot as the second operand (the original uses a register); operand order not checked. Add-operand kinds (sucomp, reg,reg / reg,mem): original 52 / 49, now 80 / 22: the original adds a frame slot directly in 49 places, ours loads it into a register first. Variants (scratch, not kept): operand order swapped in the sums (unexplained 55 -> 49, alignment 46 -> 45, diff 739 -> 756: no gain); only the pointers register (742 instructions, alignment 34, unexplained 85); only the integers register (= current source); only o/arity/t register (757, alignment 35); only the sums register (731, alignment 34, unexplained 49). The current source is kept (best alignment and length of the variants); the real cause is not found yet. |
| rsinsert-size-01 | other | rsinsert | analysed |  | analyse the difference (later, in the correction phase) | D8 and D3 mostly (byte compares of char locals, reloads); one block of the original (a table lookup followed by the byte test) is missing at that place in ours, the order of the tests differs (D6 / D7 candidate). Class: D-classes, to verify; one block to check in the source. |
| rsmerge-size-01 | other | rsmerge | analysed |  | analyse the difference (later, in the correction phase) | D3 and D8: 59 differing blocks, mostly spill and reload of the register variables (D3); byte compares of the char variables gone1, gone2, drop1, drop2 are written as load and compare in ours, as a memory compare in the original (D8). Size difference 225 / 159 is spill code. No source difference found. Class: D-classes, to verify. |
| tcmd-calls-01 | other | tcmd | analysed |  | analyse the difference (later, in the correction phase) | D6 and D14: the switch is a cpir table in ours and a compare chain in the original (D6); the call sites of the varargs printf wrapper differ in the frame setup (cgprintf with its five arguments, D14). The calls memname and getreg are the same blocks, shifted. Class: D-classes, to verify. |
| clocal-size-01 | other | clocal | open | D15 block layout (open) | analyse the block order (later, in the correction phase); swap of NAME/SCONV rejected | 80 instructions at another place (block order). Experiment (copy, reverted): NAME and SCONV branches swapped (exclusive conditions, corpus 101/101 equal). Block count 26 -> 10, but alignment 47 -> 17 % and pattern diff 246 -> 406: rejected. Not analysed further. |
| header-rdsym-01 | source | header | closed |  | analyse the difference (later, in the correction phase) | not a defect: the header is right (0x1a86). The earlier finding came from an extent that was cut after a ret at 0x1aae, which is the continuation of the same function. Alignment 92.8 %. |
| acon-tail-01 | source | acon | explained |  | none: compiler difference D11 | D11 (old popt tail merging): the original shares the call of printf (jp 44D6 / jr 44D6) between the three L%d branches; the shipped popt does not merge tails. Source unchanged. Pattern check: 0 unexplained instruction forms. |
| main-wrapper-01 | source | main | analysed | source (missing function in gen3.c) | add to gen3.c, between mycanon and stoasg, a function main(argc, argv) that returns the body; rename the body in main3.c (not main); crt0 then calls the forwarder. Check the bytes of gen3 after the change (the return value needs no extra code if it is returned directly). | 0x4868 is a separate function (after a ret at 0x4866, no jumps into it). Its only reference is the call from the start code at 0x1e. Body: spill argc/argv as one long, load them into r7 and r6, call the body at 0x023c, return. Position: in the original it lies between mycanon (0x4804) and stoasg (0x487e), i.e. at the place of a function of gen3.c; our gen3.c has no function there (mycanon is followed by stoasg). Conclusion: the original gen3.c contains a two-argument function that forwards to the body of main (main3.c, 0x023c), and this forwarder is the linked symbol main (crt0 calls it). The body in main3.c must then have another name (a static or differently named function); the name does not appear in the bytes, the link does. |
| lineout-type-01 | source | lineout | fixed (efcode3.c: long line -> int line; 100 % identical) |  | declare int line in lineout() (the cast (int)line then is not needed); check with codgen -l on a file that prints .line | Experiment: long line -> int line in efcode3.c gives lineout 100 % identical (8 / 8 instructions, 0 differences). |
| shift-type-02 | source | fixfld, buildstar, shincr, rsdump, tmpcleanup, fbegin, fixtemp, zzzcode | fixed except the order of zzzcode |  | find the variables of these shifts and their declarations (same type question as shift-type-01); check each site separately | fixed: rsdump, tmpcleanup, fixtemp (struct tmpent and struct rs flags are int in the original); shincr (t >> 2 unsigned, local3.c); fbegin (DECREF in lcl3.c signed); buildstar (the two sflags tests in tr3.c unsigned). rejected: sflags unsigned in struct sym for all uses (six other functions became unexplained: mapnm, mapregs, memname, oreg2, oregfix, readsyms). open: fixfld (one shift of the original not reproduced, 5 unexplained forms), zzzcode (shift counts equal, 2 unexplained forms sra #1, to analyse). \| fixfld: fixed: INCREF(DECREF((unsigned)m->type)) without the temporary t (the original expands the macro twice; the second srl is the second use of the expression); zzzcode: the shift counts equal the original; the two sra #1 forms are the same in context (sra r2,#1 after the load of the frame slot; offsets differ, D1), so not a sign difference. The pattern check still counts them as unexplained because the block is moved. |
| zzzcode-order-01 | other | zzzcode | partly fixed | D15 block layout (partly fixed) | find the case or branch of the original that holds this block; decide between source order and layout (D6/D7) before changing the source | The two sra #1 forms are the same in context (shift counts equal). Done: the n <= 4 / else branches of the ZS case were swapped (n > 4 first; layout change, source semantics unchanged): differing blocks 95 -> 80, instructions 1045 -> 1044; corpus codgen 101/101, scodgen 101/101. Open: the if (!tshape(r, STAREG)) block of the big branch (orig 920, ours 809 before the swap) and about 20 blocks in the same function. |
| printf-wrapper-01 | source | cgprintf, cgwipe, rcookie, start | fixed | source change, Makefile links the old printf and doprnt (oldlibc 0_3) | none | The original codgen and scodgen link printf and doprnt of the older libc (frame 0x16, doprnt frame 0x68 = oldlibc 0_3, the same version ranlib uses). With them the stray minus does not occur and the wrapper is gone: codgen 100/100 and 1/1, scodgen 100/100 and 1/1 (corpus). rcookie was removed as a duplicate of rallkind, start is the crt0 entry and not codgen code. |
