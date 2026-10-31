# cparse findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `cparse` compared with `/lib/cparse`; pending findings are the open items of the analysis.

## Summary

**Functions:** 180

| Pairing | Count |
|---|---|
| confirmed | 179 |
| open | 1 |

| Difference class | Count |
|---|---|
| - | 1 |
| D only (pattern-verified) | 154 |
| other differences | 25 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Strict layers | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|---|
| addcase | 016a-01d6 | 0x0174 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D11, D13, D2, D3, D7, D8/D2 |  |
| adddef | 01d6-0226 | 0x01e0 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D2, D3, D7 |  |
| aobeg | a006-a008 | 0x9f04 | confirmed | strict | D only (pattern-verified) | exact | D11 |  |
| argalloc | 2e20-2f60 | 0x2db2 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| argdebug | 39e0-39fa | 0x3970 | confirmed |  | D only (pattern-verified) | named shapes only | D1 | Used twice by the parser (elist: every argument of a call); returns its argument. Name and purpose assumed: probably a debug hook whose body was left out by the preprocessor (unused register variable, 4 bytes of frame), like the unused values in bfcode and clocal. |
| argregs | 55a2-5680 | 0x54e2 | confirmed |  | D only (pattern-verified) | open shapes | D6 D19 | Source: argregs(p->right) + argregs(p->left), so that the shipped cc calls the left one first like the original (D19); with the old cc the natural order argregs(p->left) + argregs(p->right) is expected. |
| argsize | 7fda-8038 | 0x7eb8 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D2, D3, D7, D8/D2 |  |
| bccode | 6b8c-6bc6 | 0x6a78 | confirmed |  | D only (pattern-verified) | named shapes only | D10, D11, D13, D7 |  |
| bcon | 4818-484c | 0x4790 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D2, D3, D7 |  |
| bdty | 004c-00ac | 0x004e | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7 |  |
| beginit | 822e-8326 | 0x8122 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| bend | 67ca-682e | 0x66c2 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D3, D7, D8, D8/D2 |  |
| bfcode | 69c6-6b8c | 0x68b0 | confirmed |  | other differences | named shapes only |  | unexplained instruction forms: sra	rS,#2 |
| bfend | 6bc6-6bd4 | 0x6ab6 | confirmed | strict | D only (pattern-verified) | exact | D11, D13, D7 |  |
| block | 4f2c-4f7c | 0x4e9a | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D11, D13, D2, D3, D7 |  |
| bpsize | 484c-4874 | 0x47c6 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D2, D3, D7 |  |
| branch | 809c-80ba | 0x7f7c | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D2, D3, D7 |  |
| bsize | 7f5e-7faa | 0x7e3a | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| bstart | 672a-67ca | 0x6620 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D3, D7, D8, D8/D2 |  |
| bstruct | 3426-34ee | 0x33b2 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| buildtree | 39fa-43b6 | 0x397a | confirmed |  | other differences | open shapes |  | unexplained instruction forms: srl	rS,#2 |
| cast | 7154-719a | 0x704c | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D7 |  |
| cerror | 7cae-7d2c | 0x7b8a | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D3, D7, D8, D8/D2 |  |
| chkpun | 44b8-4678 | 0x441e | confirmed |  | other differences | frame and registers only |  | unexplained instruction forms: sra	rS,#2 |
| chkstr | 4424-44b8 | 0x438a | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| cinit | 7788-7888 | 0x7684 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| cisreg | 75e6-763e | 0x74f2 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D2, D7, D8, D8/D2 |  |
| clearst | 9bee-9cda | 0x9ae6 | confirmed | strict | other differences | exact |  | unexplained instruction forms: xor	rS,rS |
| clocal | 719a-75e6 | 0x7094 | confirmed |  | D only (pattern-verified) | open shapes | D2 D3 D6 D7 D19 | Source: tbytes(l->type) > tbytes(p->type) for the call order of the original (D19); the rest are register and layout differences. |
| commdec | 7b36-7b98 | 0x7a1e | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D7 |  |
| conval | 5db6-6020 | 0x5cb8 | confirmed |  | other differences | open shapes |  | unexplained instruction forms: multl	rqS,F |
| convert | 48c4-494e | 0x483e | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| copysym | 97f2-996a | 0x96e2 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| cprint | 7888-79ee | 0x777c | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| ctype | 7aee-7b2e | 0x79d8 | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D2, D3, D7, D8, D8/D2 |  |
| dclargs | 2f60-31ae | 0x2efc | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D16, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| dclstruct | 34ee-373e | 0x347c | confirmed |  | other differences | open shapes |  | unexplained instruction forms: multl	rqS,rrS |
| defalign | 682e-6894 | 0x6726 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| defid | 24b8-2c56 | 0x2448 | confirmed |  | other differences | open shapes | D15 D2 D3 | Remaining: the original places the member/structure-name block (the WEGA name space check, label mismatch) between the then and the else part of the dimension test inside the loop over the dimensions; our source has it at the PCC place behind the switch (same control flow). Tried: the PCC layout. Open: a source layout that gives the original block order (goto into the loop body like in optim?). |
| deflab | 6902-6920 | 0x67ec | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D2, D3, D7 |  |
| defnam | 6bd4-6c3c | 0x6ac4 | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D2, D3, D7, D8 |  |
| dexit | 8084-809c | 0x7f62 | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D7 |  |
| dimtinit | 155e-15d0 | 0x153e | confirmed | strict | D only (pattern-verified) | exact | D10, D11, D13 |  |
| doinit | 8856-8a1c | 0x874a | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| doszof | 5260-52b2 | 0x51d0 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| dstash | 00ac-00d8 | 0x00b4 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D3, D7 |  |
| ecode | 7bae-7c50 | 0x7a9e | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| ecomp | 5362-53aa | 0x52d0 | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D2, D3, D7, D8/D2 |  |
| econvert | 494e-49f4 | 0x48ca | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| efcode | 6920-69c6 | 0x680a | confirmed |  | other differences | frame and registers only |  | unexplained instruction forms: srl	rS,#2 |
| endinit | 86f0-8856 | 0x85f0 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| exname | 7a70-7aee | 0x7952 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D16, D2, D3, D7, D8 |  |
| f0000 | - | 0x0000 | open | open |  |  |  |  |
| falloc | 8da2-8f64 | 0x8c8e | confirmed |  | other differences | open shapes |  | unexplained instruction forms: multl	rqS,rrS |
| fhashv | 659e-65d6 | 0x6484 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D2, D3, D8 |  |
| fincode | 7716-7788 | 0x7614 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D5, D7, D8/D2 |  |
| fixclass | 9484-95b8 | 0x9372 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| fixtype | 9306-945e | 0x91f2 | confirmed |  | other differences | frame only |  | unexplained instruction forms: srl	rS,#2 |
| fldal | 6c92-6ca6 | 0x6b86 | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D3, D7 |  |
| fltlab | 64d0-659e | 0x63b2 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8/D2 |  |
| fnfile | 646c-64d0 | 0x6348 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D3, D7, D8, D8/D2 |  |
| fortarg | 56ec-573a | 0x560e | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D2, D3, D7, D8, D8/D2 |  |
| ftnarg | 3750-3786 | 0x36ea | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| ftnend | 2cb2-2e20 | 0x2c44 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| fwalk | 7e60-7ed8 | 0x7d36 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| genswitch | 6cfa-7154 | 0x6bf0 | confirmed |  | D only (pattern-verified) | open shapes | D1 D2 D3 | Remaining: the size of the search table (n*SZLONG/SZCHAR as an unsigned long): the original divides with div and zero-extends, our cc shifts (srl #3) for (unsigned)(...); a new compiler difference candidate (the old cc does not turn an unsigned division by a power of 2 into a shift). Tried: (unsigned)(n*SZLONG/SZCHAR). |
| getlab | 6894-68a6 | 0x6786 | confirmed |  | D only (pattern-verified) | named shapes only | D11, D13 |  |
| getmem | 818e-8210 | 0x8084 | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| getpreg | 326a-3340 | 0x3200 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| getstr | 84da-85e2 | 0x83e0 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| gotscal | 8a1c-8b48 | 0x890e | confirmed |  | other differences | open shapes |  | unexplained instruction forms: multl	rqS,rrS; srl	rS,#2 |
| hash | 8bea-8c26 | 0x8ae0 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D16, D3, D7, D8 |  |
| hide | 996a-9a6e | 0x985a | confirmed |  | other differences | frame and registers only |  | unexplained instruction forms: xor	rS,rS |
| icons | 4fb4-4ffe | 0x4f24 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| ilbrace | 8b48-8bb6 | 0x8a3c | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| incode | 7676-7716 | 0x7574 | confirmed |  | other differences | frame and registers only |  | unexplained instruction forms: or	rS,A |
| inforce | 38e0-3998 | 0x3870 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D7 |  |
| initpreg | 324a-326a | 0x31e0 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D7 |  |
| instk | 8326-84da | 0x821a | confirmed |  | other differences | open shapes |  | unexplained instruction forms: srl	rS,#2 |
| irbrace | 8bb6-8bea | 0x8aac | confirmed | strict | D only (pattern-verified) | exact | D11, D13, D3, D7, D8/D2 |  |
| isitfloat | 7b98-7bae | 0x7a80 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D3, D7 |  |
| ispow2 | 8122-816c | 0x8014 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D6, D7, D8 |  |
| lcons | 4f7c-4fb4 | 0x4eec | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| lineid | 6ca6-6cfa | 0x6b9c | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| locctr | 68a6-6902 | 0x6790 | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D2, D7, D8/D2 |  |
| lookup | 95fa-97f2 | 0x94e8 | confirmed |  | other differences | frame and registers only |  | unexplained instruction forms: xor	rS,rS |
| lxclass | 1454-148a | 0x1434 | confirmed | strict | D only (pattern-verified) | exact | D10, D11, D13, D7 |  |
| lxcom | 1abe-1b58 | 0x1a96 | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| lxenter | 148a-14b4 | 0x146a | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| lxinit | 14b4-1534 | 0x1494 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| lxmore | 1734-17b8 | 0x1710 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| lxres | 236c-24b8 | 0x2314 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| lxstart | 144e-1454 | 0x142e | confirmed | strict | D only (pattern-verified) | exact | D11, D7 |  |
| lxstr | 1884-1abe | 0x1862 | confirmed |  | other differences | open shapes | D2 D6 D15 | Remaining: the lastcon \|= val<<8 arm uses rr0 in the original, rr4 in ours (D2), and it is placed in front of the case bodies in the original (layout); the outer switch is a compare chain (D6). |
| lxtitle | 15d0-1734 | 0x15b0 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| main | 61f2-62f0 | 0x60d8 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| mainp1 | 13d6-144e | 0x13b6 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D5, D7, D8/D2 |  |
| makety | 4e06-4f2c | 0x4d76 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| mkdope | 7f2e-7f5e | 0x7e06 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D11, D13, D3, D5, D6, D7, D8/D2 |  |
| mkty | 002a-004c | 0x002a | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D2, D3, D7 |  |
| moditype | 5228-5260 | 0x5196 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| moedef | 33ec-3426 | 0x3376 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| ncopy | 80ba-80d0 | 0x7f9a | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3 |  |
| newsect | 65d6-66d6 | 0x64c2 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D16, D3, D5, D7, D8, D8/D2 |  |
| nidcl | 8f64-907a | 0x8e4a | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| nncon | 816c-818e | 0x8062 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D2, D3, D7, D8/D2 |  |
| noinit | 7b2e-7b36 | 0x7a1a | confirmed |  | D only (pattern-verified) | named shapes only | D10, D11 |  |
| notflt | 61c4-61f2 | 0x60b6 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D2, D3, D7, D8/D2 |  |
| notlval | 47cc-4818 | 0x4748 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| oalloc | 8c76-8da2 | 0x8b6e | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| oconvert | 4a4e-4b0e | 0x49ca | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| offcon | 763e-7676 | 0x7540 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D2, D3, D7 |  |
| opact | 4ffe-5228 | 0x4f6e | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| openfiles | 62f0-646c | 0x61d0 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| optim | 573a-5db6 | 0x565c | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| pconvert | 49f4-4a4e | 0x4970 | confirmed |  | other differences | frame only |  | unexplained instruction forms: srl	rS,#2 |
| prtdcon | 52b2-5362 | 0x5222 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| prtree | 53aa-55a2 | 0x5318 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| psave | 2c56-2c80 | 0x2be2 | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D6, D7 |  |
| psavel | 2c80-2cb2 | 0x2c0e | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D13, D3, D6, D7 |  |
| psize | 4874-48c4 | 0x47ee | confirmed |  | other differences | frame and registers only |  | unexplained instruction forms: srl	rS,#2 |
| ptmatch | 4b0e-4c40 | 0x4a86 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| putbyte | 85e2-860a | 0x84e4 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D2, D3, D7 |  |
| putnames | 9f08-9f80 | 0x9e04 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D3, D7, D8, D8/D2 |  |
| putrefs | 9fa6-9fce | 0x9ea6 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D7 |  |
| putrefsyms | 9d3e-9d68 | 0x9c38 | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D6, D7 |  |
| putstrs | 860a-86f0 | 0x8510 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D3, D7, D8, D8/D2 |  |
| putsym | 9d68-9f08 | 0x9c62 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D16, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| puttags | 9f80-9fa6 | 0x9e80 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D6, D7 |  |
| refid | 9adc-9bee | 0x99ca | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| regfree | 95b8-95fa | 0x94a6 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| resetbc | 0126-016a | 0x0130 | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D13, D16, D3, D8 |  |
| resetmem | 8210-822e | 0x8100 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D3, D7 |  |
| rmconv | 6096-61c4 | 0x5f90 | confirmed |  | D only (pattern-verified) | open shapes | D3 D19 | Source: bsize(q->type) == bsize(q->left->type) and notflt in the same order, for the call order of the original (D19). |
| rstruct | 3340-33ec | 0x32ca | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| savebc | 00d8-0126 | 0x00e2 | confirmed | strict | D only (pattern-verified) | exact | D11, D13, D3, D7 |  |
| savestr | 17b8-1884 | 0x1792 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D3, D7, D8/D2 |  |
| scnames | 1392-13d6 | 0x1374 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| sectadd | 66f8-672a | 0x65e8 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D3, D7, D8/D2 |  |
| sectfull | 66d6-66f8 | 0x65c4 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D2, D3, D7 |  |
| setreg | 31ae-324a | 0x3144 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| sfinit | 1534-155e | 0x1514 | confirmed | strict | D only (pattern-verified) | exact | D1, D10, D11, D13, D3, D5, D7, D8/D2 |  |
| sheader | 9d12-9d3e | 0x9c0c | confirmed | strict | D only (pattern-verified) | exact | D10, D11, D13, D2, D7 |  |
| sinit | 9cda-9d12 | 0x9bd4 | confirmed | strict | D only (pattern-verified) | exact | D10, D11, D13, D7 |  |
| sputc | 9fce-a006 | 0x9ece | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| strargs | 43b6-4424 | 0x431a | confirmed |  | D only (pattern-verified) | frame only | D1, D11, D13, D2, D3, D7, D8/D2 |  |
| stref | 4678-47cc | 0x45de | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| swend | 0246-0308 | 0x0250 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D13, D2, D3, D5, D7, D8/D2 |  |
| swstart | 0226-0246 | 0x0230 | confirmed | strict | D only (pattern-verified) | exact | D11, D13, D7 |  |
| syserr | 7c50-7c72 | 0x7b28 | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D7 |  |
| talign | 3786-3838 | 0x3722 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| talloc | 7d90-7dee | 0x7c70 | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D13, D3, D7, D8/D2 |  |
| tbytes | 8054-8084 | 0x7f30 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D2, D3, D6, D7, D8, D8/D2 |  |
| tcheck | 7dee-7e2a | 0x7cc4 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D7, D8/D2 |  |
| tcopy | 80d0-8122 | 0x7fbc | confirmed |  | D only (pattern-verified) | open shapes | D1, D11, D2, D3, D6, D7, D8, D8/D2 |  |
| tempnode | 5680-56ec | 0x55b8 | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| tfree | 7e2a-7e44 | 0x7cfe | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D7, D8/D2 |  |
| tfree1 | 7e44-7e60 | 0x7d1a | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D13, D2, D3, D7, D8/D2 |  |
| tinit | 7d68-7d90 | 0x7c48 | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D7, D8/D2 |  |
| treeeq | 6020-6096 | 0x5f1a | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D2, D3, D6, D7, D8, D8/D2 |  |
| tsize | 3838-38e0 | 0x37cc | confirmed |  | other differences | named shapes only |  | unexplained instruction forms: multl	rqS,A(rS); multl	rqS,rrS |
| twords | 7faa-7fda | 0x7e86 | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D10, D11, D2, D3, D6, D7, D8, D8/D2 |  |
| tymatch | 4c40-4e06 | 0x4bb4 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8/D2 |  |
| tymerge | 9186-9270 | 0x906e | confirmed |  | other differences | named shapes only |  | unexplained instruction forms: srl	rS,#2 |
| types | 907a-9186 | 0x8f60 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| tyreduce | 9270-9306 | 0x9156 | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D7, D8, D8/D2 |  |
| uclass | 945e-9484 | 0x934c | confirmed |  | D only (pattern-verified) | frame only | D1, D10, D11, D13, D2, D3, D7, D8/D2 |  |
| uerror | 7c72-7cae | 0x7b4c | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D7 |  |
| unhide | 9a6e-9adc | 0x995c | confirmed |  | other differences | frame and registers only |  | unexplained instruction forms: xor	rS,rS |
| upoff | 8c26-8c76 | 0x8b1e | confirmed |  | other differences | named shapes only |  | unexplained instruction forms: multl	rqS,rrS |
| vfdalign | 3998-39e0 | 0x392c | confirmed |  | other differences | named shapes only |  | unexplained instruction forms: multl	rqS,rrS |
| vfdzero | 79ee-7a70 | 0x78cc | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D2, D3, D6, D7, D8/D2 |  |
| walkf | 7ed8-7f2e | 0x7dae | confirmed |  | D only (pattern-verified) | frame and registers only | D1, D11, D2, D3, D6, D7, D8, D8/D2 |  |
| werror | 7d2c-7d68 | 0x7c0a | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D7 |  |
| where | 8038-8054 | 0x7f12 | confirmed | strict | D only (pattern-verified) | exact | D1, D11, D13, D3, D7 |  |
| yyaccpt | 374c-3750 | 0x36e6 | confirmed | strict | D only (pattern-verified) | exact | D11, D7 |  |
| yyerror | 373e-374c | 0x36d4 | confirmed |  | D only (pattern-verified) | named shapes only | D1, D11, D3, D7 |  |
| yylex | 1b58-236c | 0x1b2a | confirmed |  | other differences | open shapes |  | unexplained instruction forms: multl	rqS,#A000A |
| yyparse | 0308-1392 | 0x0312 | confirmed |  | other differences | open shapes | D1 D2 D3 D6 D11 | cgram1.y is the V7/System III/Plexus grammar with one more nonterminal lcon_e (a long constant expression, lcons) for the three array dimensions: with it every yacc table (yyexca, yyact, yypact, yypgo, yyr1, yyr2, yychk, yydef) equals the original. tools/actcmp.py compares the actions rule by rule: 135 of 178 equal with frame and registers hidden; the rest are shared tails (D11), register and reload differences. |
| zecode | 6c3c-6c92 | 0x6b2c | confirmed |  | D only (pattern-verified) | open shapes | D1, D10, D11, D13, D3, D7 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
