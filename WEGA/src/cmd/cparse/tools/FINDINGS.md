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
| D only (pattern-verified) | 176 |
| other differences | 3 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| addcase | 0174-01e0 | 0x0174 | confirmed | strict | D only (pattern-verified) |  |  |
| adddef | 01e0-0230 | 0x01e0 | confirmed | strict | D only (pattern-verified) |  |  |
| aobeg | 9fe8-9fea | 0x9f04 | confirmed | strict | D only (pattern-verified) |  |  |
| argalloc | 2e02-2f42 | 0x2db2 | confirmed | open | D only (pattern-verified) | D10, D13, D2, D7 |  |
| argregs | 5584-5662 | 0x54e2 | confirmed | open | D only (pattern-verified) | D6 D19 | Source: argregs(p->right) + argregs(p->left), so that the shipped cc calls the left one first like the original (D19); with the old cc the natural order argregs(p->left) + argregs(p->right) is expected. |
| argsize | 7fbc-801a | 0x7eb8 | confirmed | open | D only (pattern-verified) | D1, D2 |  |
| bccode | 6b6e-6ba8 | 0x6a78 | confirmed | open | D only (pattern-verified) | D10, D13 |  |
| bcon | 47fa-482e | 0x4790 | confirmed | open | D only (pattern-verified) | D1 |  |
| bdty | 004e-00b4 | 0x004e | confirmed | strict | D only (pattern-verified) |  |  |
| beginit | 8210-8308 | 0x8122 | confirmed | open | D only (pattern-verified) | D1, D10, D3 |  |
| bend | 67ac-6810 | 0x66c2 | confirmed | open | D only (pattern-verified) |  |  |
| bfcode | 69a8-6b6e | 0x68b0 | confirmed | open | D only (pattern-verified) | D1, D2 |  |
| bfend | 6ba8-6bb6 | 0x6ab6 | confirmed | open | D only (pattern-verified) |  |  |
| block | 4f0e-4f5e | 0x4e9a | confirmed | open | D only (pattern-verified) |  |  |
| bpsize | 482e-4856 | 0x47c6 | confirmed | open | D only (pattern-verified) | D1 |  |
| branch | 807e-809c | 0x7f7c | confirmed | open | D only (pattern-verified) | D1 |  |
| bsize | 7f40-7f8c | 0x7e3a | confirmed | open | D only (pattern-verified) | D1 |  |
| bstart | 670c-67ac | 0x6620 | confirmed | open | D only (pattern-verified) |  |  |
| bstruct | 3408-34d0 | 0x33b2 | confirmed | open | D only (pattern-verified) | D1 |  |
| buildtree | 39dc-4398 | 0x397a | confirmed | open | D only (pattern-verified) | D13, D2, D3, D6, D7, D8, D8/D2 |  |
| cast | 7136-717c | 0x704c | confirmed | open | D only (pattern-verified) | D1 |  |
| cerror | 7c90-7d0e | 0x7b8a | confirmed | strict | D only (pattern-verified) |  |  |
| chkpun | 449a-465a | 0x441e | confirmed | open | D only (pattern-verified) | D1 |  |
| chkstr | 4406-449a | 0x438a | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| cinit | 776a-786a | 0x7684 | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| cisreg | 75c8-7620 | 0x74f2 | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| clearst | 9bd0-9cbc | 0x9ae6 | confirmed | strict | D only (pattern-verified) |  |  |
| clocal | 717c-75c8 | 0x7094 | confirmed | open | D only (pattern-verified) | D2 D3 D6 D7 D19 | Source: tbytes(l->type) > tbytes(p->type) for the call order of the original (D19); the rest are register and layout differences. |
| commdec | 7b18-7b7a | 0x7a1e | confirmed | open | D only (pattern-verified) | D1 |  |
| conval | 5d98-6002 | 0x5cb8 | confirmed | open | D only (pattern-verified) | D1, D13, D2, D3, D6, D8/D2 |  |
| convert | 48a6-4930 | 0x483e | confirmed | open | D only (pattern-verified) | D1 |  |
| copysym | 97d4-994c | 0x96e2 | confirmed | open | D only (pattern-verified) | D1, D7 |  |
| cprint | 786a-79d0 | 0x777c | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| ctype | 7ad0-7b10 | 0x79d8 | confirmed | open | D only (pattern-verified) | D1, D8 |  |
| dclargs | 2f42-3190 | 0x2efc | confirmed | open | D only (pattern-verified) | D13, D2, D5, D7 |  |
| dclstruct | 34d0-3720 | 0x347c | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D3, D5 |  |
| defalign | 6810-6876 | 0x6726 | confirmed | open | D only (pattern-verified) | D1, D10, D3 |  |
| defid | 249a-2c38 | 0x2448 | confirmed | open | other differences | D15 D2 D3 | Remaining: the original places the member/structure-name block (the WEGA name space check, label mismatch) between the then and the else part of the dimension test inside the loop over the dimensions; our source has it at the PCC place behind the switch (same control flow). Tried: the PCC layout. Open: a source layout that gives the original block order (goto into the loop body like in optim?). |
| deflab | 68e4-6902 | 0x67ec | confirmed | open | D only (pattern-verified) | D1 |  |
| defnam | 6bb6-6c1e | 0x6ac4 | confirmed | open | D only (pattern-verified) | D1 |  |
| dexit | 8066-807e | 0x7f62 | confirmed | strict | D only (pattern-verified) |  |  |
| dimtinit | 1540-15b2 | 0x153e | confirmed | strict | D only (pattern-verified) |  |  |
| doinit | 8838-89fe | 0x874a | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2 |  |
| doszof | 5242-5294 | 0x51d0 | confirmed | open | D only (pattern-verified) | D1 |  |
| dstash | 00b4-00e2 | 0x00b4 | confirmed | strict | D only (pattern-verified) |  |  |
| ecode | 7b90-7c32 | 0x7a9e | confirmed | open | D only (pattern-verified) | D2, D3, D7, D8/D2 |  |
| ecomp | 5344-538c | 0x52d0 | confirmed | open | D only (pattern-verified) | D1 |  |
| econvert | 4930-49d6 | 0x48ca | confirmed | open | D only (pattern-verified) | D1 |  |
| efcode | 6902-69a8 | 0x680a | confirmed | open | D only (pattern-verified) | D1 |  |
| endinit | 86d2-8838 | 0x85f0 | confirmed | open | D only (pattern-verified) | D10, D13, D2, D3, D6, D7, D8/D2 |  |
| exname | 7a52-7ad0 | 0x7952 | confirmed | open | D only (pattern-verified) | D1, D13, D16, D3, D8 |  |
| f0000 | - | 0x0000 | open | open |  |  |  |
| f3970 | 39c2-39dc | 0x3970 | confirmed | open | D only (pattern-verified) | D1 | Used twice by the parser (yyparse); returns its argument; purpose and name open. |
| falloc | 8d84-8f46 | 0x8c8e | confirmed | open | D only (pattern-verified) | D2, D3, D7, D8/D2 |  |
| fhashv | 6580-65b8 | 0x6484 | confirmed | open | D only (pattern-verified) | D1, D3 |  |
| fincode | 76f8-776a | 0x7614 | confirmed | open | D only (pattern-verified) | D1, D2 |  |
| fixclass | 9466-959a | 0x9372 | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| fixtype | 92e8-9440 | 0x91f2 | confirmed | open | D only (pattern-verified) | D1 |  |
| fldal | 6c74-6c88 | 0x6b86 | confirmed | open | D only (pattern-verified) | D1 |  |
| fltlab | 64b2-6580 | 0x63b2 | confirmed | open | D only (pattern-verified) | D1, D13, D2 |  |
| fnfile | 644e-64b2 | 0x6348 | confirmed | open | D only (pattern-verified) | D3 |  |
| fortarg | 56ce-571c | 0x560e | confirmed | open | D only (pattern-verified) | D1 |  |
| ftnarg | 3732-3768 | 0x36ea | confirmed | open | D only (pattern-verified) | D2, D3 |  |
| ftnend | 2c94-2e02 | 0x2c44 | confirmed | strict | D only (pattern-verified) |  |  |
| fwalk | 7e42-7eba | 0x7d36 | confirmed | open | D only (pattern-verified) | D1, D2 |  |
| genswitch | 6cdc-7136 | 0x6bf0 | confirmed | open | other differences | D1 D2 D3 | Remaining: the size of the search table (n*SZLONG/SZCHAR as an unsigned long): the original divides with div and zero-extends, our cc shifts (srl #3) for (unsigned)(...); a new compiler difference candidate (the old cc does not turn an unsigned division by a power of 2 into a shift). Tried: (unsigned)(n*SZLONG/SZCHAR). |
| getlab | 6876-6888 | 0x6786 | confirmed | open | D only (pattern-verified) | D1, D3 |  |
| getmem | 8170-81f2 | 0x8084 | confirmed | open | D only (pattern-verified) | D1, D2, D5, D8 |  |
| getpreg | 324c-3322 | 0x3200 | confirmed | open | D only (pattern-verified) | D1, D13, D3, D7, D8 |  |
| getstr | 84bc-85c4 | 0x83e0 | confirmed | open | D only (pattern-verified) | D1, D10, D2 |  |
| gotscal | 89fe-8b2a | 0x890e | confirmed | open | D only (pattern-verified) | D1, D13, D2, D8/D2 |  |
| hash | 8bcc-8c08 | 0x8ae0 | confirmed | open | D only (pattern-verified) | D1 |  |
| hide | 994c-9a50 | 0x985a | confirmed | open | D only (pattern-verified) | D1, D7 |  |
| icons | 4f96-4fe0 | 0x4f24 | confirmed | open | D only (pattern-verified) | D1 |  |
| ilbrace | 8b2a-8b98 | 0x8a3c | confirmed | open | D only (pattern-verified) | D13, D2 |  |
| incode | 7658-76f8 | 0x7574 | confirmed | open | D only (pattern-verified) | D1 |  |
| inforce | 38c2-397a | 0x3870 | confirmed | open | D only (pattern-verified) | D10, D13 |  |
| initpreg | 322c-324c | 0x31e0 | confirmed | strict | D only (pattern-verified) |  |  |
| instk | 8308-84bc | 0x821a | confirmed | open | D only (pattern-verified) | D10, D13, D2, D3 |  |
| irbrace | 8b98-8bcc | 0x8aac | confirmed | strict | D only (pattern-verified) |  |  |
| isitfloat | 7b7a-7b90 | 0x7a80 | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| ispow2 | 8104-814e | 0x8014 | confirmed | open | D only (pattern-verified) | D1, D10, D13, D6 |  |
| lcons | 4f5e-4f96 | 0x4eec | confirmed | open | D only (pattern-verified) | D1 |  |
| lineid | 6c88-6cdc | 0x6b9c | confirmed | open | D only (pattern-verified) |  |  |
| locctr | 6888-68e4 | 0x6790 | confirmed | open | D only (pattern-verified) | D1 |  |
| lookup | 95dc-97d4 | 0x94e8 | confirmed | open | D only (pattern-verified) | D7 |  |
| lxclass | 1436-146c | 0x1434 | confirmed | strict | D only (pattern-verified) |  |  |
| lxcom | 1aa0-1b3a | 0x1a96 | confirmed | open | D only (pattern-verified) | D13, D2, D3, D6, D7, D8, D8/D2 |  |
| lxenter | 146c-1496 | 0x146a | confirmed | open | D only (pattern-verified) | D1 |  |
| lxinit | 1496-1516 | 0x1494 | confirmed | strict | D only (pattern-verified) |  |  |
| lxmore | 1716-179a | 0x1710 | confirmed | open | D only (pattern-verified) | D1, D2 |  |
| lxres | 234e-249a | 0x2314 | confirmed | open | D only (pattern-verified) | D1 |  |
| lxstart | 1430-1436 | 0x142e | confirmed | strict | D only (pattern-verified) |  |  |
| lxstr | 1866-1aa0 | 0x1862 | confirmed | open | other differences | D2 D6 D15 | Remaining: the lastcon \|= val<<8 arm uses rr0 in the original, rr4 in ours (D2), and it is placed in front of the case bodies in the original (layout); the outer switch is a compare chain (D6). |
| lxtitle | 15b2-1716 | 0x15b0 | confirmed | open | D only (pattern-verified) | D2 |  |
| main | 61d4-62d2 | 0x60d8 | confirmed | open | D only (pattern-verified) | D1, D2, D3, D6 |  |
| mainp1 | 13b8-1430 | 0x13b6 | confirmed | open | D only (pattern-verified) | D1 |  |
| makety | 4de8-4f0e | 0x4d76 | confirmed | open | D only (pattern-verified) | D1, D13 |  |
| mkdope | 7f10-7f40 | 0x7e06 | confirmed | open | D only (pattern-verified) |  |  |
| mkty | 002a-004e | 0x002a | confirmed | strict | D only (pattern-verified) |  |  |
| moditype | 520a-5242 | 0x5196 | confirmed | open | D only (pattern-verified) | D1 |  |
| moedef | 33ce-3408 | 0x3376 | confirmed | strict | D only (pattern-verified) |  |  |
| ncopy | 809c-80b2 | 0x7f9a | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| newsect | 65b8-66b8 | 0x64c2 | confirmed | open | D only (pattern-verified) | D16, D2, D6 |  |
| nidcl | 8f46-905c | 0x8e4a | confirmed | open | D only (pattern-verified) | D10, D2 |  |
| nncon | 814e-8170 | 0x8062 | confirmed | open | D only (pattern-verified) | D1 |  |
| noinit | 7b10-7b18 | 0x7a1a | confirmed | open | D only (pattern-verified) | D1 |  |
| notflt | 61a6-61d4 | 0x60b6 | confirmed | open | D only (pattern-verified) | D1 |  |
| notlval | 47ae-47fa | 0x4748 | confirmed | open | D only (pattern-verified) | D1, D6 |  |
| oalloc | 8c58-8d84 | 0x8b6e | confirmed | open | D only (pattern-verified) | D10, D13, D2, D3, D5, D7 |  |
| oconvert | 4a30-4af0 | 0x49ca | confirmed | open | D only (pattern-verified) | D1, D13, D2, D6 |  |
| offcon | 7620-7658 | 0x7540 | confirmed | open | D only (pattern-verified) | D1, D13 |  |
| opact | 4fe0-520a | 0x4f6e | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| openfiles | 62d2-644e | 0x61d0 | confirmed | open | D only (pattern-verified) | D2 |  |
| optim | 571c-5d98 | 0x565c | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8/D2 |  |
| pconvert | 49d6-4a30 | 0x4970 | confirmed | open | D only (pattern-verified) | D1 |  |
| prtdcon | 5294-5344 | 0x5222 | confirmed | open | D only (pattern-verified) | D1, D10, D2, D3 |  |
| prtree | 538c-5584 | 0x5318 | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8 |  |
| psave | 2c38-2c62 | 0x2be2 | confirmed | strict | D only (pattern-verified) |  |  |
| psavel | 2c62-2c94 | 0x2c0e | confirmed | open | D only (pattern-verified) | D10, D13, D3 |  |
| psize | 4856-48a6 | 0x47ee | confirmed | open | D only (pattern-verified) | D1 |  |
| ptmatch | 4af0-4c22 | 0x4a86 | confirmed | open | D only (pattern-verified) | D6 |  |
| putbyte | 85c4-85ec | 0x84e4 | confirmed | open | D only (pattern-verified) | D2, D3 |  |
| putnames | 9eea-9f06 | 0x9e04 | confirmed | strict | D only (pattern-verified) | D1, D10, D11, D13, D3, D7, D8, D8/D2 |  |
| putrefs | 9f88-9fb0 | 0x9ea6 | confirmed | strict | D only (pattern-verified) |  |  |
| putrefsyms | 9d20-9d4a | 0x9c38 | confirmed | strict | D only (pattern-verified) |  |  |
| putstrs | 85ec-86d2 | 0x8510 | confirmed | open | D only (pattern-verified) | D2 |  |
| putsym | 9d4a-9eea | 0x9c62 | confirmed | open | D only (pattern-verified) | D13, D2, D3 |  |
| puttags | 9f62-9f88 | 0x9e80 | confirmed | strict | D only (pattern-verified) |  |  |
| refid | 9abe-9bd0 | 0x99ca | confirmed | open | D only (pattern-verified) | D2, D3 |  |
| regfree | 959a-95dc | 0x94a6 | confirmed | open | D only (pattern-verified) | D1 |  |
| resetbc | 0130-0174 | 0x0130 | confirmed | strict | D only (pattern-verified) |  |  |
| resetmem | 81f2-8210 | 0x8100 | confirmed | open | D only (pattern-verified) | D2, D3 |  |
| rmconv | 6078-61a6 | 0x5f90 | confirmed | open | D only (pattern-verified) | D3 D19 | Source: bsize(q->type) == bsize(q->left->type) and notflt in the same order, for the call order of the original (D19). |
| rstruct | 3322-33ce | 0x32ca | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| savebc | 00e2-0130 | 0x00e2 | confirmed | strict | D only (pattern-verified) |  |  |
| savestr | 179a-1866 | 0x1792 | confirmed | open | D only (pattern-verified) | D13, D2 |  |
| scnames | 1374-13b8 | 0x1374 | confirmed | open | D only (pattern-verified) | D1, D2 |  |
| sectadd | 66da-670c | 0x65e8 | confirmed | open | D only (pattern-verified) | D10, D2, D3 |  |
| sectfull | 66b8-66da | 0x65c4 | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| setreg | 3190-322c | 0x3144 | confirmed | open | D only (pattern-verified) | D1 |  |
| sfinit | 1516-1540 | 0x1514 | confirmed | strict | D only (pattern-verified) |  |  |
| sheader | 9cf4-9d20 | 0x9c0c | confirmed | strict | D only (pattern-verified) |  |  |
| sinit | 9cbc-9cf4 | 0x9bd4 | confirmed | strict | D only (pattern-verified) |  |  |
| sputc | 9fb0-9fe8 | 0x9ece | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| strargs | 4398-4406 | 0x431a | confirmed | open | D only (pattern-verified) | D1, D7 |  |
| stref | 465a-47ae | 0x45de | confirmed | open | D only (pattern-verified) | D1, D13, D2, D3 |  |
| swend | 0250-0312 | 0x0250 | confirmed | strict | D only (pattern-verified) |  |  |
| swstart | 0230-0250 | 0x0230 | confirmed | strict | D only (pattern-verified) |  |  |
| syserr | 7c32-7c54 | 0x7b28 | confirmed | strict | D only (pattern-verified) | D1 |  |
| talign | 3768-381a | 0x3722 | confirmed | open | D only (pattern-verified) | D1, D2, D6 |  |
| talloc | 7d72-7dd0 | 0x7c70 | confirmed | open | D only (pattern-verified) | D1, D13, D3, D7 |  |
| tbytes | 8036-8066 | 0x7f30 | confirmed | open | D only (pattern-verified) | D1 |  |
| tcheck | 7dd0-7e0c | 0x7cc4 | confirmed | open | D only (pattern-verified) | D2 |  |
| tcopy | 80b2-8104 | 0x7fbc | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| tempnode | 5662-56ce | 0x55b8 | confirmed | open | D only (pattern-verified) | D1 |  |
| tfree | 7e0c-7e26 | 0x7cfe | confirmed | open | D only (pattern-verified) | D2, D3 |  |
| tfree1 | 7e26-7e42 | 0x7d1a | confirmed | open | D only (pattern-verified) | D2, D3 |  |
| tinit | 7d4a-7d72 | 0x7c48 | confirmed | strict | D only (pattern-verified) |  |  |
| treeeq | 6002-6078 | 0x5f1a | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D6, D7, D8/D2 |  |
| tsize | 381a-38c2 | 0x37cc | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D6 |  |
| twords | 7f8c-7fbc | 0x7e86 | confirmed | open | D only (pattern-verified) | D1 |  |
| tymatch | 4c22-4de8 | 0x4bb4 | confirmed | open | D only (pattern-verified) | D2 |  |
| tymerge | 9168-9252 | 0x906e | confirmed | open | D only (pattern-verified) | D1, D2, D3 |  |
| types | 905c-9168 | 0x8f60 | confirmed | open | D only (pattern-verified) | D13, D5 |  |
| tyreduce | 9252-92e8 | 0x9156 | confirmed | open | D only (pattern-verified) | D1, D10, D3 |  |
| uclass | 9440-9466 | 0x934c | confirmed | open | D only (pattern-verified) | D1 |  |
| uerror | 7c54-7c90 | 0x7b4c | confirmed | strict | D only (pattern-verified) |  |  |
| unhide | 9a50-9abe | 0x995c | confirmed | open | D only (pattern-verified) | D1 |  |
| upoff | 8c08-8c58 | 0x8b1e | confirmed | open | D only (pattern-verified) | D10 |  |
| vfdalign | 397a-39c2 | 0x392c | confirmed | open | D only (pattern-verified) | D1, D13 |  |
| vfdzero | 79d0-7a52 | 0x78cc | confirmed | open | D only (pattern-verified) | D16, D2, D3, D6 |  |
| walkf | 7eba-7f10 | 0x7dae | confirmed | open | D only (pattern-verified) | D1 |  |
| werror | 7d0e-7d4a | 0x7c0a | confirmed | strict | D only (pattern-verified) |  |  |
| where | 801a-8036 | 0x7f12 | confirmed | strict | D only (pattern-verified) |  |  |
| yyaccpt | 372e-3732 | 0x36e6 | confirmed | strict | D only (pattern-verified) |  |  |
| yyerror | 3720-372e | 0x36d4 | confirmed | open | D only (pattern-verified) | D2, D3 |  |
| yylex | 1b3a-234e | 0x1b2a | confirmed | open | D only (pattern-verified) | D1, D10, D13, D2, D5, D6, D7 |  |
| yyparse | 0312-1374 | 0x0312 | confirmed | strict | D only (pattern-verified) |  |  |
| zecode | 6c1e-6c74 | 0x6b2c | confirmed | open | D only (pattern-verified) | D10, D13, D3 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
