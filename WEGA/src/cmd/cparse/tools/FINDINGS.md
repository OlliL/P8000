# cparse findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `cparse` compared with `/lib/cparse`; pending findings are the open items of the analysis.

## Summary

**Functions:** 181

| Pairing | Count |
|---|---|
| open | 181 |

| Difference class | Count |
|---|---|
| - | 181 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| addcase | - | 0x0174 | open | open |  |  |  |
| adddef | - | 0x01e0 | open | open |  |  |  |
| argsize | - | 0x7eb8 | open | open |  |  |  |
| bccode | - | 0x6a78 | open | open |  |  |  |
| bcon | - | 0x4790 | open | open |  |  |  |
| bdty | - | 0x004e | open | open |  |  |  |
| beginit | - | 0x8122 | open | open |  |  |  |
| bfcode | - | 0x68b0 | open | open |  |  |  |
| block | - | 0x4e9a | open | open |  |  |  |
| bpsize | - | 0x47c6 | open | open |  |  |  |
| branch | - | 0x7f7c | open | open |  |  |  |
| bsize | - | 0x7e3a | open | open |  |  |  |
| bstruct | - | 0x33b2 | open | open |  |  |  |
| buildtree | - | 0x397a | open | open |  |  |  |
| cast | - | 0x704c | open | open |  |  |  |
| cerror | - | 0x7b8a | open | open |  |  |  |
| chkpun | - | 0x441e | open | open |  |  |  |
| chkstr | - | 0x438a | open | open |  |  |  |
| cisreg | - | 0x74f2 | open | open |  |  |  |
| clearst | - | 0x9ae6 | open | open |  |  |  |
| clocal | - | 0x7094 | open | open |  |  |  |
| conval | - | 0x5cb8 | open | open |  |  |  |
| convert | - | 0x483e | open | open |  |  |  |
| ctype | - | 0x79d8 | open | open |  |  |  |
| dclargs | - | 0x2db2 | open | open |  |  |  |
| dclstruct | - | 0x347c | open | open |  |  |  |
| defid | - | 0x2448 | open | open |  |  |  |
| defnam | - | 0x6ac4 | open | open |  |  |  |
| dexit | - | 0x7f62 | open | open |  |  |  |
| doinit | - | 0x874a | open | open |  |  |  |
| dstash | - | 0x00b4 | open | open |  |  |  |
| ecomp | - | 0x52d0 | open | open |  |  |  |
| econvert | - | 0x48ca | open | open |  |  |  |
| efcode | - | 0x680a | open | open |  |  |  |
| endinit | - | 0x85f0 | open | open |  |  |  |
| f0000 | - | 0x0000 | open | open |  |  |  |
| f142e | - | 0x142e | open | open |  |  |  |
| f1434 | - | 0x1434 | open | open |  |  |  |
| f1514 | - | 0x1514 | open | open |  |  |  |
| f153e | - | 0x153e | open | open |  |  |  |
| f1792 | - | 0x1792 | open | open |  |  |  |
| f2be2 | - | 0x2be2 | open | open |  |  |  |
| f2c0e | - | 0x2c0e | open | open |  |  |  |
| f2efc | - | 0x2efc | open | open |  |  |  |
| f3144 | - | 0x3144 | open | open |  |  |  |
| f31e0 | - | 0x31e0 | open | open |  |  |  |
| f3200 | - | 0x3200 | open | open |  |  |  |
| f36d4 | - | 0x36d4 | open | open |  |  |  |
| f3970 | - | 0x3970 | open | open |  |  |  |
| f4eec | - | 0x4eec | open | open |  |  |  |
| f51d0 | - | 0x51d0 | open | open |  |  |  |
| f54e2 | - | 0x54e2 | open | open |  |  |  |
| f55b8 | - | 0x55b8 | open | open |  |  |  |
| f5f1a | - | 0x5f1a | open | open |  |  |  |
| f5f90 | - | 0x5f90 | open | open |  |  |  |
| f60b6 | - | 0x60b6 | open | open |  |  |  |
| f61d0 | - | 0x61d0 | open | open |  |  |  |
| f6348 | - | 0x6348 | open | open |  |  |  |
| f63b2 | - | 0x63b2 | open | open |  |  |  |
| f6484 | - | 0x6484 | open | open |  |  |  |
| f64c2 | - | 0x64c2 | open | open |  |  |  |
| f65c4 | - | 0x65c4 | open | open |  |  |  |
| f65e8 | - | 0x65e8 | open | open |  |  |  |
| f6620 | - | 0x6620 | open | open |  |  |  |
| f66c2 | - | 0x66c2 | open | open |  |  |  |
| f6726 | - | 0x6726 | open | open |  |  |  |
| f6786 | - | 0x6786 | open | open |  |  |  |
| f6790 | - | 0x6790 | open | open |  |  |  |
| f67ec | - | 0x67ec | open | open |  |  |  |
| f6ab6 | - | 0x6ab6 | open | open |  |  |  |
| f6b2c | - | 0x6b2c | open | open |  |  |  |
| f6b9c | - | 0x6b9c | open | open |  |  |  |
| f7574 | - | 0x7574 | open | open |  |  |  |
| f7684 | - | 0x7684 | open | open |  |  |  |
| f777c | - | 0x777c | open | open |  |  |  |
| f7952 | - | 0x7952 | open | open |  |  |  |
| f7a1a | - | 0x7a1a | open | open |  |  |  |
| f7a1e | - | 0x7a1e | open | open |  |  |  |
| f7a9e | - | 0x7a9e | open | open |  |  |  |
| f7b28 | - | 0x7b28 | open | open |  |  |  |
| f8100 | - | 0x8100 | open | open |  |  |  |
| f8510 | - | 0x8510 | open | open |  |  |  |
| f8aac | - | 0x8aac | open | open |  |  |  |
| f8ac4 | - | 0x8ac4 | open | open |  |  |  |
| f8ae0 | - | 0x8ae0 | open | open |  |  |  |
| f8b1e | - | 0x8b1e | open | open |  |  |  |
| f94a6 | - | 0x94a6 | open | open |  |  |  |
| f94e8 | - | 0x94e8 | open | open |  |  |  |
| f96e2 | - | 0x96e2 | open | open |  |  |  |
| f985a | - | 0x985a | open | open |  |  |  |
| f99ca | - | 0x99ca | open | open |  |  |  |
| f9bd4 | - | 0x9bd4 | open | open |  |  |  |
| f9c0c | - | 0x9c0c | open | open |  |  |  |
| f9c38 | - | 0x9c38 | open | open |  |  |  |
| f9c62 | - | 0x9c62 | open | open |  |  |  |
| f9e04 | - | 0x9e04 | open | open |  |  |  |
| f9e80 | - | 0x9e80 | open | open |  |  |  |
| f9ea6 | - | 0x9ea6 | open | open |  |  |  |
| f9ece | - | 0x9ece | open | open |  |  |  |
| f9f04 | - | 0x9f04 | open | open |  |  |  |
| falloc | - | 0x8c8e | open | open |  |  |  |
| fincode | - | 0x7614 | open | open |  |  |  |
| fixclass | - | 0x9372 | open | open |  |  |  |
| fixtype | - | 0x91f2 | open | open |  |  |  |
| fldal | - | 0x6b86 | open | open |  |  |  |
| fortarg | - | 0x560e | open | open |  |  |  |
| ftnarg | - | 0x36ea | open | open |  |  |  |
| ftnend | - | 0x2c44 | open | open |  |  |  |
| fwalk | - | 0x7d36 | open | open |  |  |  |
| genswitch | - | 0x6bf0 | open | open |  |  |  |
| getmem | - | 0x8084 | open | open |  |  |  |
| getstr | - | 0x83e0 | open | open |  |  |  |
| gotscal | - | 0x890e | open | open |  |  |  |
| icons | - | 0x4f24 | open | open |  |  |  |
| ilbrace | - | 0x8a3c | open | open |  |  |  |
| inforce | - | 0x3870 | open | open |  |  |  |
| instk | - | 0x821a | open | open |  |  |  |
| isitfloat | - | 0x7a80 | open | open |  |  |  |
| ispow2 | - | 0x8014 | open | open |  |  |  |
| lxcom | - | 0x1a96 | open | open |  |  |  |
| lxenter | - | 0x146a | open | open |  |  |  |
| lxinit | - | 0x1494 | open | open |  |  |  |
| lxmore | - | 0x1710 | open | open |  |  |  |
| lxres | - | 0x2314 | open | open |  |  |  |
| lxstr | - | 0x1862 | open | open |  |  |  |
| lxtitle | - | 0x15b0 | open | open |  |  |  |
| main | - | 0x60d8 | open | open |  |  |  |
| mainp1 | - | 0x13b6 | open | open |  |  |  |
| makety | - | 0x4d76 | open | open |  |  |  |
| mkdope | - | 0x7e06 | open | open |  |  |  |
| mkty | - | 0x002a | open | open |  |  |  |
| moditype | - | 0x5196 | open | open |  |  |  |
| moedef | - | 0x3376 | open | open |  |  |  |
| ncopy | - | 0x7f9a | open | open |  |  |  |
| nidcl | - | 0x8e4a | open | open |  |  |  |
| nncon | - | 0x8062 | open | open |  |  |  |
| notlval | - | 0x4748 | open | open |  |  |  |
| oalloc | - | 0x8b6e | open | open |  |  |  |
| oconvert | - | 0x49ca | open | open |  |  |  |
| offcon | - | 0x7540 | open | open |  |  |  |
| opact | - | 0x4f6e | open | open |  |  |  |
| optim | - | 0x565c | open | open |  |  |  |
| pconvert | - | 0x4970 | open | open |  |  |  |
| prtdcon | - | 0x5222 | open | open |  |  |  |
| prtree | - | 0x5318 | open | open |  |  |  |
| psize | - | 0x47ee | open | open |  |  |  |
| ptmatch | - | 0x4a86 | open | open |  |  |  |
| putbyte | - | 0x84e4 | open | open |  |  |  |
| resetbc | - | 0x0130 | open | open |  |  |  |
| rstruct | - | 0x32ca | open | open |  |  |  |
| savebc | - | 0x00e2 | open | open |  |  |  |
| scnames | - | 0x1374 | open | open |  |  |  |
| strargs | - | 0x431a | open | open |  |  |  |
| stref | - | 0x45de | open | open |  |  |  |
| swend | - | 0x0250 | open | open |  |  |  |
| swstart | - | 0x0230 | open | open |  |  |  |
| talign | - | 0x3722 | open | open |  |  |  |
| talloc | - | 0x7c70 | open | open |  |  |  |
| tbytes | - | 0x7f30 | open | open |  |  |  |
| tcheck | - | 0x7cc4 | open | open |  |  |  |
| tcopy | - | 0x7fbc | open | open |  |  |  |
| tfree | - | 0x7cfe | open | open |  |  |  |
| tfree1 | - | 0x7d1a | open | open |  |  |  |
| tinit | - | 0x7c48 | open | open |  |  |  |
| tsize | - | 0x37cc | open | open |  |  |  |
| twords | - | 0x7e86 | open | open |  |  |  |
| tymatch | - | 0x4bb4 | open | open |  |  |  |
| tymerge | - | 0x906e | open | open |  |  |  |
| types | - | 0x8f60 | open | open |  |  |  |
| tyreduce | - | 0x9156 | open | open |  |  |  |
| uclass | - | 0x934c | open | open |  |  |  |
| uerror | - | 0x7b4c | open | open |  |  |  |
| unhide | - | 0x995c | open | open |  |  |  |
| vfdalign | - | 0x392c | open | open |  |  |  |
| vfdzero | - | 0x78cc | open | open |  |  |  |
| walkf | - | 0x7dae | open | open |  |  |  |
| werror | - | 0x7c0a | open | open |  |  |  |
| where | - | 0x7f12 | open | open |  |  |  |
| yyaccpt | - | 0x36e6 | open | open |  |  |  |
| yylex | - | 0x1b2a | open | open |  |  |  |
| yyparse | - | 0x0312 | open | open |  |  |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
