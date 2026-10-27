# sld findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `sld` compared with `/lib/sld`; pending findings are the open items of the analysis.

## Summary

**Functions:** 416

| Pairing | Count |
|---|---|
| confirmed | 42 |
| open | 374 |

| Difference class | Count |
|---|---|
| - | 374 |
| D only (manual reading) | 3 |
| D only (pattern-verified) | 39 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | 0x0780-0x07ae | 0x1664 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| c_0018 |  | 0x0018 | open | candidate: starts after ret, not called |  |  |  |
| c_00ee |  | 0x00ee | open | candidate: starts after ret, not called |  |  |  |
| c_0178 |  | 0x0178 | open | candidate: starts after ret, not called |  |  |  |
| c_01d2 |  | 0x01d2 | open | candidate: starts after ret, not called |  |  |  |
| c_01e8 |  | 0x01e8 | open | candidate: starts after ret, not called |  |  |  |
| c_020e |  | 0x020e | open | candidate: starts after ret, not called |  |  |  |
| c_0256 |  | 0x0256 | open | candidate: starts after ret, not called |  |  |  |
| c_026c |  | 0x026c | open | candidate: starts after ret, not called |  |  |  |
| c_02f4 |  | 0x02f4 | open | candidate: starts after ret, not called |  |  |  |
| c_0334 |  | 0x0334 | open | candidate: starts after ret, not called |  |  |  |
| c_034a |  | 0x034a | open | candidate: starts after ret, not called |  |  |  |
| c_036a |  | 0x036a | open | candidate: starts after ret, not called |  |  |  |
| c_0382 |  | 0x0382 | open | candidate: starts after ret, not called |  |  |  |
| c_039a |  | 0x039a | open | candidate: starts after ret, not called |  |  |  |
| c_03b2 |  | 0x03b2 | open | candidate: starts after ret, not called |  |  |  |
| c_03ce |  | 0x03ce | open | candidate: starts after ret, not called |  |  |  |
| c_03e6 |  | 0x03e6 | open | candidate: starts after ret, not called |  |  |  |
| c_03fc |  | 0x03fc | open | candidate: starts after ret, not called |  |  |  |
| c_0412 |  | 0x0412 | open | candidate: starts after ret, not called |  |  |  |
| c_043a |  | 0x043a | open | candidate: starts after ret, not called |  |  |  |
| c_0464 |  | 0x0464 | open | candidate: starts after ret, not called |  |  |  |
| c_047c |  | 0x047c | open | candidate: starts after ret, not called |  |  |  |
| c_0494 |  | 0x0494 | open | candidate: starts after ret, not called |  |  |  |
| c_04a0 |  | 0x04a0 | open | candidate: starts after ret, not called |  |  |  |
| c_04b4 |  | 0x04b4 | open | candidate: starts after ret, not called |  |  |  |
| c_04c8 |  | 0x04c8 | open | candidate: starts after ret, not called |  |  |  |
| c_04dc |  | 0x04dc | open | candidate: starts after ret, not called |  |  |  |
| c_04f0 |  | 0x04f0 | open | candidate: starts after ret, not called |  |  |  |
| c_0504 |  | 0x0504 | open | candidate: starts after ret, not called |  |  |  |
| c_0518 |  | 0x0518 | open | candidate: starts after ret, not called |  |  |  |
| c_052c |  | 0x052c | open | candidate: starts after ret, not called |  |  |  |
| c_0540 |  | 0x0540 | open | candidate: starts after ret, not called |  |  |  |
| c_054c |  | 0x054c | open | candidate: starts after ret, not called |  |  |  |
| c_0566 |  | 0x0566 | open | candidate: starts after ret, not called |  |  |  |
| c_0574 |  | 0x0574 | open | candidate: starts after ret, not called |  |  |  |
| c_05ac |  | 0x05ac | open | candidate: starts after ret, not called |  |  |  |
| c_05c4 |  | 0x05c4 | open | candidate: starts after ret, not called |  |  |  |
| c_05d6 |  | 0x05d6 | open | candidate: starts after ret, not called |  |  |  |
| c_05f8 |  | 0x05f8 | open | candidate: starts after ret, not called |  |  |  |
| c_0610 |  | 0x0610 | open | candidate: starts after ret, not called |  |  |  |
| c_0632 |  | 0x0632 | open | candidate: starts after ret, not called |  |  |  |
| c_064a |  | 0x064a | open | candidate: starts after ret, not called |  |  |  |
| c_066c |  | 0x066c | open | candidate: starts after ret, not called |  |  |  |
| c_0684 |  | 0x0684 | open | candidate: starts after ret, not called |  |  |  |
| c_06a6 |  | 0x06a6 | open | candidate: starts after ret, not called |  |  |  |
| c_06c6 |  | 0x06c6 | open | candidate: starts after ret, not called |  |  |  |
| c_0762 |  | 0x0762 | open | candidate: starts after ret, not called |  |  |  |
| c_078c |  | 0x078c | open | candidate: starts after ret, not called |  |  |  |
| c_0a44 |  | 0x0a44 | open | candidate: starts after ret, not called |  |  |  |
| c_0a5c |  | 0x0a5c | open | candidate: starts after ret, not called |  |  |  |
| c_0a6a |  | 0x0a6a | open | candidate: starts after ret, not called |  |  |  |
| c_0a92 |  | 0x0a92 | open | candidate: starts after ret, not called |  |  |  |
| c_0ac8 |  | 0x0ac8 | open | candidate: starts after ret, not called |  |  |  |
| c_0ade |  | 0x0ade | open | candidate: starts after ret, not called |  |  |  |
| c_0aec |  | 0x0aec | open | candidate: starts after ret, not called |  |  |  |
| c_0b02 |  | 0x0b02 | open | candidate: starts after ret, not called |  |  |  |
| c_0c06 |  | 0x0c06 | open | candidate: starts after ret, not called |  |  |  |
| c_0db0 |  | 0x0db0 | open | candidate: starts after ret, not called |  |  |  |
| c_0dfe |  | 0x0dfe | open | candidate: starts after ret, not called |  |  |  |
| c_0e22 |  | 0x0e22 | open | candidate: starts after ret, not called |  |  |  |
| c_0f26 |  | 0x0f26 | open | candidate: starts after ret, not called |  |  |  |
| c_0ff2 |  | 0x0ff2 | open | candidate: starts after ret, not called |  |  |  |
| c_1038 |  | 0x1038 | open | candidate: starts after ret, not called |  |  |  |
| c_11b2 |  | 0x11b2 | open | candidate: starts after ret, not called |  |  |  |
| c_1224 |  | 0x1224 | open | candidate: starts after ret, not called |  |  |  |
| c_1238 |  | 0x1238 | open | candidate: starts after ret, not called |  |  |  |
| c_13de |  | 0x13de | open | candidate: starts after ret, not called |  |  |  |
| c_1414 |  | 0x1414 | open | candidate: starts after ret, not called |  |  |  |
| c_1560 |  | 0x1560 | open | candidate: starts after ret, not called |  |  |  |
| c_16ec |  | 0x16ec | open | candidate: starts after ret, not called |  |  |  |
| c_172c |  | 0x172c | open | candidate: starts after ret, not called |  |  |  |
| c_173c |  | 0x173c | open | candidate: starts after ret, not called |  |  |  |
| c_1742 |  | 0x1742 | open | candidate: starts after ret, not called |  |  |  |
| c_1756 |  | 0x1756 | open | candidate: starts after ret, not called |  |  |  |
| c_17e4 |  | 0x17e4 | open | candidate: starts after ret, not called |  |  |  |
| c_181c |  | 0x181c | open | candidate: starts after ret, not called |  |  |  |
| c_18dc |  | 0x18dc | open | candidate: starts after ret, not called |  |  |  |
| c_18f2 |  | 0x18f2 | open | candidate: starts after ret, not called |  |  |  |
| c_1a3a |  | 0x1a3a | open | candidate: starts after ret, not called |  |  |  |
| c_1bba |  | 0x1bba | open | candidate: starts after ret, not called |  |  |  |
| c_1c8e |  | 0x1c8e | open | candidate: starts after ret, not called |  |  |  |
| c_1d20 |  | 0x1d20 | open | candidate: starts after ret, not called |  |  |  |
| c_1fc4 |  | 0x1fc4 | open | candidate: starts after ret, not called |  |  |  |
| c_1fca |  | 0x1fca | open | candidate: starts after ret, not called |  |  |  |
| c_1fd0 |  | 0x1fd0 | open | candidate: starts after ret, not called |  |  |  |
| c_200e |  | 0x200e | open | candidate: starts after ret, not called |  |  |  |
| c_2014 |  | 0x2014 | open | candidate: starts after ret, not called |  |  |  |
| c_201a |  | 0x201a | open | candidate: starts after ret, not called |  |  |  |
| c_2056 |  | 0x2056 | open | candidate: starts after ret, not called |  |  |  |
| c_2078 |  | 0x2078 | open | candidate: starts after ret, not called |  |  |  |
| c_2324 |  | 0x2324 | open | candidate: starts after ret, not called |  |  |  |
| c_23fe |  | 0x23fe | open | candidate: starts after ret, not called |  |  |  |
| c_24fc |  | 0x24fc | open | candidate: starts after ret, not called |  |  |  |
| c_2566 |  | 0x2566 | open | candidate: starts after ret, not called |  |  |  |
| c_25ea |  | 0x25ea | open | candidate: starts after ret, not called |  |  |  |
| c_25f2 |  | 0x25f2 | open | candidate: starts after ret, not called |  |  |  |
| c_26f2 |  | 0x26f2 | open | candidate: starts after ret, not called |  |  |  |
| c_28c4 |  | 0x28c4 | open | candidate: starts after ret, not called |  |  |  |
| c_29a0 |  | 0x29a0 | open | candidate: starts after ret, not called |  |  |  |
| c_29b8 |  | 0x29b8 | open | candidate: starts after ret, not called |  |  |  |
| c_2a2e |  | 0x2a2e | open | candidate: starts after ret, not called |  |  |  |
| c_2b1e |  | 0x2b1e | open | candidate: starts after ret, not called |  |  |  |
| c_2cbc |  | 0x2cbc | open | candidate: starts after ret, not called |  |  |  |
| c_2d18 |  | 0x2d18 | open | candidate: starts after ret, not called |  |  |  |
| c_2dc8 |  | 0x2dc8 | open | candidate: starts after ret, not called |  |  |  |
| c_2e30 |  | 0x2e30 | open | candidate: starts after ret, not called |  |  |  |
| c_3006 |  | 0x3006 | open | candidate: starts after ret, not called |  |  |  |
| c_313c |  | 0x313c | open | candidate: starts after ret, not called |  |  |  |
| c_319e |  | 0x319e | open | candidate: starts after ret, not called |  |  |  |
| c_31c4 |  | 0x31c4 | open | candidate: starts after ret, not called |  |  |  |
| c_31fe |  | 0x31fe | open | candidate: starts after ret, not called |  |  |  |
| c_321e |  | 0x321e | open | candidate: starts after ret, not called |  |  |  |
| c_325c |  | 0x325c | open | candidate: starts after ret, not called |  |  |  |
| c_32ac |  | 0x32ac | open | candidate: starts after ret, not called |  |  |  |
| c_342a |  | 0x342a | open | candidate: starts after ret, not called |  |  |  |
| c_3442 |  | 0x3442 | open | candidate: starts after ret, not called |  |  |  |
| c_345c |  | 0x345c | open | candidate: starts after ret, not called |  |  |  |
| c_3468 |  | 0x3468 | open | candidate: starts after ret, not called |  |  |  |
| c_3490 |  | 0x3490 | open | candidate: starts after ret, not called |  |  |  |
| c_349c |  | 0x349c | open | candidate: starts after ret, not called |  |  |  |
| c_34fe |  | 0x34fe | open | candidate: starts after ret, not called |  |  |  |
| c_3516 |  | 0x3516 | open | candidate: starts after ret, not called |  |  |  |
| c_377e |  | 0x377e | open | candidate: starts after ret, not called |  |  |  |
| c_3a22 |  | 0x3a22 | open | candidate: starts after ret, not called |  |  |  |
| c_3af2 |  | 0x3af2 | open | candidate: starts after ret, not called |  |  |  |
| c_3b1e |  | 0x3b1e | open | candidate: starts after ret, not called |  |  |  |
| c_3b58 |  | 0x3b58 | open | candidate: starts after ret, not called |  |  |  |
| c_3bf4 |  | 0x3bf4 | open | candidate: starts after ret, not called |  |  |  |
| c_3bfe |  | 0x3bfe | open | candidate: starts after ret, not called |  |  |  |
| c_3c16 |  | 0x3c16 | open | candidate: starts after ret, not called |  |  |  |
| c_3f4a |  | 0x3f4a | open | candidate: starts after ret, not called |  |  |  |
| c_3fd8 |  | 0x3fd8 | open | candidate: starts after ret, not called |  |  |  |
| c_4168 |  | 0x4168 | open | candidate: starts after ret, not called |  |  |  |
| c_4444 |  | 0x4444 | open | candidate: starts after ret, not called |  |  |  |
| c_44e6 |  | 0x44e6 | open | candidate: starts after ret, not called |  |  |  |
| c_4668 |  | 0x4668 | open | candidate: starts after ret, not called |  |  |  |
| c_4854 |  | 0x4854 | open | candidate: starts after ret, not called |  |  |  |
| c_4882 |  | 0x4882 | open | candidate: starts after ret, not called |  |  |  |
| c_489a |  | 0x489a | open | candidate: starts after ret, not called |  |  |  |
| c_48a2 |  | 0x48a2 | open | candidate: starts after ret, not called |  |  |  |
| c_4990 |  | 0x4990 | open | candidate: starts after ret, not called |  |  |  |
| c_4996 |  | 0x4996 | open | candidate: starts after ret, not called |  |  |  |
| c_49a0 |  | 0x49a0 | open | candidate: starts after ret, not called |  |  |  |
| c_49a6 |  | 0x49a6 | open | candidate: starts after ret, not called |  |  |  |
| c_4a2a |  | 0x4a2a | open | candidate: starts after ret, not called |  |  |  |
| c_4a32 |  | 0x4a32 | open | candidate: starts after ret, not called |  |  |  |
| c_4a3a |  | 0x4a3a | open | candidate: starts after ret, not called |  |  |  |
| c_4a50 |  | 0x4a50 | open | candidate: starts after ret, not called |  |  |  |
| c_501e |  | 0x501e | open | candidate: starts after ret, not called |  |  |  |
| c_5040 |  | 0x5040 | open | candidate: starts after ret, not called |  |  |  |
| c_521e |  | 0x521e | open | candidate: starts after ret, not called |  |  |  |
| c_522c |  | 0x522c | open | candidate: starts after ret, not called |  |  |  |
| c_53d8 |  | 0x53d8 | open | candidate: starts after ret, not called |  |  |  |
| c_5552 |  | 0x5552 | open | candidate: starts after ret, not called |  |  |  |
| c_555c |  | 0x555c | open | candidate: starts after ret, not called |  |  |  |
| c_56e6 |  | 0x56e6 | open | candidate: starts after ret, not called |  |  |  |
| c_5868 |  | 0x5868 | open | candidate: starts after ret, not called |  |  |  |
| c_595a |  | 0x595a | open | candidate: starts after ret, not called |  |  |  |
| c_5960 |  | 0x5960 | open | candidate: starts after ret, not called |  |  |  |
| c_596a |  | 0x596a | open | candidate: starts after ret, not called |  |  |  |
| c_5970 |  | 0x5970 | open | candidate: starts after ret, not called |  |  |  |
| c_5a4a |  | 0x5a4a | open | candidate: starts after ret, not called |  |  |  |
| c_5a56 |  | 0x5a56 | open | candidate: starts after ret, not called |  |  |  |
| c_5d0a |  | 0x5d0a | open | candidate: starts after ret, not called |  |  |  |
| c_5da2 |  | 0x5da2 | open | candidate: starts after ret, not called |  |  |  |
| c_5e60 |  | 0x5e60 | open | candidate: starts after ret, not called |  |  |  |
| c_5e66 |  | 0x5e66 | open | candidate: starts after ret, not called |  |  |  |
| c_5e6c |  | 0x5e6c | open | candidate: starts after ret, not called |  |  |  |
| c_6134 |  | 0x6134 | open | candidate: starts after ret, not called |  |  |  |
| c_6244 |  | 0x6244 | open | candidate: starts after ret, not called |  |  |  |
| c_632e |  | 0x632e | open | candidate: starts after ret, not called |  |  |  |
| c_6354 |  | 0x6354 | open | candidate: starts after ret, not called |  |  |  |
| c_637e |  | 0x637e | open | candidate: starts after ret, not called |  |  |  |
| c_63a4 |  | 0x63a4 | open | candidate: starts after ret, not called |  |  |  |
| c_63c8 |  | 0x63c8 | open | candidate: starts after ret, not called |  |  |  |
| c_63f0 |  | 0x63f0 | open | candidate: starts after ret, not called |  |  |  |
| c_64fa |  | 0x64fa | open | candidate: starts after ret, not called |  |  |  |
| c_6510 |  | 0x6510 | open | candidate: starts after ret, not called |  |  |  |
| c_6522 |  | 0x6522 | open | candidate: starts after ret, not called |  |  |  |
| c_6598 |  | 0x6598 | open | candidate: starts after ret, not called |  |  |  |
| c_65b6 |  | 0x65b6 | open | candidate: starts after ret, not called |  |  |  |
| c_65ec |  | 0x65ec | open | candidate: starts after ret, not called |  |  |  |
| c_6620 |  | 0x6620 | open | candidate: starts after ret, not called |  |  |  |
| c_6684 |  | 0x6684 | open | candidate: starts after ret, not called |  |  |  |
| c_6866 |  | 0x6866 | open | candidate: starts after ret, not called |  |  |  |
| c_68a2 |  | 0x68a2 | open | candidate: starts after ret, not called |  |  |  |
| c_68e4 |  | 0x68e4 | open | candidate: starts after ret, not called |  |  |  |
| c_68ee |  | 0x68ee | open | candidate: starts after ret, not called |  |  |  |
| c_6920 |  | 0x6920 | open | candidate: starts after ret, not called |  |  |  |
| c_6942 |  | 0x6942 | open | candidate: starts after ret, not called |  |  |  |
| c_695c |  | 0x695c | open | candidate: starts after ret, not called |  |  |  |
| c_6964 |  | 0x6964 | open | candidate: starts after ret, not called |  |  |  |
| c_6984 |  | 0x6984 | open | candidate: starts after ret, not called |  |  |  |
| c_6a08 |  | 0x6a08 | open | candidate: starts after ret, not called |  |  |  |
| c_6a36 |  | 0x6a36 | open | candidate: starts after ret, not called |  |  |  |
| c_6b5a |  | 0x6b5a | open | candidate: starts after ret, not called |  |  |  |
| c_6bae |  | 0x6bae | open | candidate: starts after ret, not called |  |  |  |
| c_6c7a |  | 0x6c7a | open | candidate: starts after ret, not called |  |  |  |
| c_6da8 |  | 0x6da8 | open | candidate: starts after ret, not called |  |  |  |
| c_6e96 |  | 0x6e96 | open | candidate: starts after ret, not called |  |  |  |
| c_6ed4 |  | 0x6ed4 | open | candidate: starts after ret, not called |  |  |  |
| c_6efe |  | 0x6efe | open | candidate: starts after ret, not called |  |  |  |
| c_6f46 |  | 0x6f46 | open | candidate: starts after ret, not called |  |  |  |
| c_6fbc |  | 0x6fbc | open | candidate: starts after ret, not called |  |  |  |
| c_6ffe |  | 0x6ffe | open | candidate: starts after ret, not called |  |  |  |
| c_702a |  | 0x702a | open | candidate: starts after ret, not called |  |  |  |
| c_7042 |  | 0x7042 | open | candidate: starts after ret, not called |  |  |  |
| c_70ce |  | 0x70ce | open | candidate: starts after ret, not called |  |  |  |
| c_7112 |  | 0x7112 | open | candidate: starts after ret, not called |  |  |  |
| c_7148 |  | 0x7148 | open | candidate: starts after ret, not called |  |  |  |
| c_714e |  | 0x714e | open | candidate: starts after ret, not called |  |  |  |
| c_7160 |  | 0x7160 | open | candidate: starts after ret, not called |  |  |  |
| c_7182 |  | 0x7182 | open | candidate: starts after ret, not called |  |  |  |
| c_7186 |  | 0x7186 | open | candidate: starts after ret, not called |  |  |  |
| c_719c |  | 0x719c | open | candidate: starts after ret, not called |  |  |  |
| c_719e |  | 0x719e | open | candidate: starts after ret, not called |  |  |  |
| c_71ca |  | 0x71ca | open | candidate: starts after ret, not called |  |  |  |
| c_7212 |  | 0x7212 | open | candidate: starts after ret, not called |  |  |  |
| c_7248 |  | 0x7248 | open | candidate: starts after ret, not called |  |  |  |
| c_724e |  | 0x724e | open | candidate: starts after ret, not called |  |  |  |
| c_7280 |  | 0x7280 | open | candidate: starts after ret, not called |  |  |  |
| c_7286 |  | 0x7286 | open | candidate: starts after ret, not called |  |  |  |
| c_72f2 |  | 0x72f2 | open | candidate: starts after ret, not called |  |  |  |
| c_7300 |  | 0x7300 | open | candidate: starts after ret, not called |  |  |  |
| c_7326 |  | 0x7326 | open | candidate: starts after ret, not called |  |  |  |
| c_7348 |  | 0x7348 | open | candidate: starts after ret, not called |  |  |  |
| c_73a2 |  | 0x73a2 | open | candidate: starts after ret, not called |  |  |  |
| c_73f0 |  | 0x73f0 | open | candidate: starts after ret, not called |  |  |  |
| c_7414 |  | 0x7414 | open | candidate: starts after ret, not called |  |  |  |
| c_74ca |  | 0x74ca | open | candidate: starts after ret, not called |  |  |  |
| c_7580 |  | 0x7580 | open | candidate: starts after ret, not called |  |  |  |
| c_75b8 |  | 0x75b8 | open | candidate: starts after ret, not called |  |  |  |
| c_7684 |  | 0x7684 | open | candidate: starts after ret, not called |  |  |  |
| c_76a0 |  | 0x76a0 | open | candidate: starts after ret, not called |  |  |  |
| c_76f2 |  | 0x76f2 | open | candidate: starts after ret, not called |  |  |  |
| c_77d2 |  | 0x77d2 | open | candidate: starts after ret, not called |  |  |  |
| c_79ca |  | 0x79ca | open | candidate: starts after ret, not called |  |  |  |
| c_7a52 |  | 0x7a52 | open | candidate: starts after ret, not called |  |  |  |
| c_7a5e |  | 0x7a5e | open | candidate: starts after ret, not called |  |  |  |
| c_7ad6 |  | 0x7ad6 | open | candidate: starts after ret, not called |  |  |  |
| c_7ae2 |  | 0x7ae2 | open | candidate: starts after ret, not called |  |  |  |
| c_7b7e |  | 0x7b7e | open | candidate: starts after ret, not called |  |  |  |
| c_7c26 |  | 0x7c26 | open | candidate: starts after ret, not called |  |  |  |
| c_7d3a |  | 0x7d3a | open | candidate: starts after ret, not called |  |  |  |
| c_7e64 |  | 0x7e64 | open | candidate: starts after ret, not called |  |  |  |
| c_8394 |  | 0x8394 | open | candidate: starts after ret, not called |  |  |  |
| c_84b0 |  | 0x84b0 | open | candidate: starts after ret, not called |  |  |  |
| c_84b6 |  | 0x84b6 | open | candidate: starts after ret, not called |  |  |  |
| c_8572 |  | 0x8572 | open | candidate: starts after ret, not called |  |  |  |
| c_8582 |  | 0x8582 | open | candidate: starts after ret, not called |  |  |  |
| c_8682 |  | 0x8682 | open | candidate: starts after ret, not called |  |  |  |
| c_8684 |  | 0x8684 | open | candidate: starts after ret, not called |  |  |  |
| c_8686 |  | 0x8686 | open | candidate: starts after ret, not called |  |  |  |
| c_8688 |  | 0x8688 | open | candidate: starts after ret, not called |  |  |  |
| c_86fa |  | 0x86fa | open | candidate: starts after ret, not called |  |  |  |
| c_87a2 |  | 0x87a2 | open | candidate: starts after ret, not called |  |  |  |
| c_8806 |  | 0x8806 | open | candidate: starts after ret, not called |  |  |  |
| c_880e |  | 0x880e | open | candidate: starts after ret, not called |  |  |  |
| c_8860 |  | 0x8860 | open | candidate: starts after ret, not called |  |  |  |
| c_8882 |  | 0x8882 | open | candidate: starts after ret, not called |  |  |  |
| c_88ee |  | 0x88ee | open | candidate: starts after ret, not called |  |  |  |
| c_8970 |  | 0x8970 | open | candidate: starts after ret, not called |  |  |  |
| c_898e |  | 0x898e | open | candidate: starts after ret, not called |  |  |  |
| c_89ac |  | 0x89ac | open | candidate: starts after ret, not called |  |  |  |
| c_8a14 |  | 0x8a14 | open | candidate: starts after ret, not called |  |  |  |
| c_8a48 |  | 0x8a48 | open | candidate: starts after ret, not called |  |  |  |
| c_8a50 |  | 0x8a50 | open | candidate: starts after ret, not called |  |  |  |
| c_8aac |  | 0x8aac | open | candidate: starts after ret, not called |  |  |  |
| c_8ad0 |  | 0x8ad0 | open | candidate: starts after ret, not called |  |  |  |
| c_8be8 |  | 0x8be8 | open | candidate: starts after ret, not called |  |  |  |
| c_8c86 |  | 0x8c86 | open | candidate: starts after ret, not called |  |  |  |
| c_8e58 |  | 0x8e58 | open | candidate: starts after ret, not called |  |  |  |
| c_8e5e |  | 0x8e5e | open | candidate: starts after ret, not called |  |  |  |
| c_8ec0 |  | 0x8ec0 | open | candidate: starts after ret, not called |  |  |  |
| c_9072 |  | 0x9072 | open | candidate: starts after ret, not called |  |  |  |
| c_9184 |  | 0x9184 | open | candidate: starts after ret, not called |  |  |  |
| c_91da |  | 0x91da | open | candidate: starts after ret, not called |  |  |  |
| c_91ea |  | 0x91ea | open | candidate: starts after ret, not called |  |  |  |
| c_9210 |  | 0x9210 | open | candidate: starts after ret, not called |  |  |  |
| c_9296 |  | 0x9296 | open | candidate: starts after ret, not called |  |  |  |
| c_92ae |  | 0x92ae | open | candidate: starts after ret, not called |  |  |  |
| c_92c6 |  | 0x92c6 | open | candidate: starts after ret, not called |  |  |  |
| c_92f8 |  | 0x92f8 | open | candidate: starts after ret, not called |  |  |  |
| c_932e |  | 0x932e | open | candidate: starts after ret, not called |  |  |  |
| c_9376 |  | 0x9376 | open | candidate: starts after ret, not called |  |  |  |
| c_93ac |  | 0x93ac | open | candidate: starts after ret, not called |  |  |  |
| c_93e8 |  | 0x93e8 | open | candidate: starts after ret, not called |  |  |  |
| c_943c |  | 0x943c | open | candidate: starts after ret, not called |  |  |  |
| c_9494 |  | 0x9494 | open | candidate: starts after ret, not called |  |  |  |
| c_94c8 |  | 0x94c8 | open | candidate: starts after ret, not called |  |  |  |
| c_952c |  | 0x952c | open | candidate: starts after ret, not called |  |  |  |
| c_9538 |  | 0x9538 | open | candidate: starts after ret, not called |  |  |  |
| c_9564 |  | 0x9564 | open | candidate: starts after ret, not called |  |  |  |
| c_9570 |  | 0x9570 | open | candidate: starts after ret, not called |  |  |  |
| c_9580 |  | 0x9580 | open | candidate: starts after ret, not called |  |  |  |
| c_9590 |  | 0x9590 | open | candidate: starts after ret, not called |  |  |  |
| c_95a4 |  | 0x95a4 | open | candidate: starts after ret, not called |  |  |  |
| c_95b0 |  | 0x95b0 | open | candidate: starts after ret, not called |  |  |  |
| c_95c2 |  | 0x95c2 | open | candidate: starts after ret, not called |  |  |  |
| c_95d4 |  | 0x95d4 | open | candidate: starts after ret, not called |  |  |  |
| c_95e8 |  | 0x95e8 | open | candidate: starts after ret, not called |  |  |  |
| c_95fe |  | 0x95fe | open | candidate: starts after ret, not called |  |  |  |
| c_960a |  | 0x960a | open | candidate: starts after ret, not called |  |  |  |
| c_961c |  | 0x961c | open | candidate: starts after ret, not called |  |  |  |
| c_9638 |  | 0x9638 | open | candidate: starts after ret, not called |  |  |  |
| c_9678 |  | 0x9678 | open | candidate: starts after ret, not called |  |  |  |
| c_9690 |  | 0x9690 | open | candidate: starts after ret, not called |  |  |  |
| c_96fa |  | 0x96fa | open | candidate: starts after ret, not called |  |  |  |
| c_9704 |  | 0x9704 | open | candidate: starts after ret, not called |  |  |  |
| c_9716 |  | 0x9716 | open | candidate: starts after ret, not called |  |  |  |
| c_9730 |  | 0x9730 | open | candidate: starts after ret, not called |  |  |  |
| copy_leafname | 0x0624-0x06c6 | 0x4444 | confirmed | exact | D only (pattern-verified) |  |  |
| f0018 | f0018..0056 | 0x0018 | open | open |  |  |  |
| f00ee | f00ee..0178 | 0x00ee | open | open |  |  |  |
| f0178 | 0x05c6-0x0624 | 0x0178 | confirmed | differs | D only (pattern-verified) | D1, D10 |  |
| f01d2 |  | 0x01d2 | open | open |  |  |  |
| f0220 | f0220..06c6 | 0x0220 | open | open |  |  |  |
| f06c6 | f06c6..0762 | 0x06c6 | open | open |  |  |  |
| f0762 | 0x0094-0x00be | 0x0762 | confirmed | exact | D only (pattern-verified) |  |  |
| f078c | f078c..0a5c | 0x078c | open | open |  |  |  |
| f0b04 | f0b04..0f26 | 0x0b04 | open | open |  |  |  |
| f0f26 | f0f26..1070 | 0x0f26 | open | open |  |  |  |
| f1070 | f1070..11ea | 0x1070 | open | open |  |  |  |
| f11ea | f11ea..1364 | 0x11ea | open | open |  |  |  |
| f1364 | f1364..14a4 | 0x1364 | open | open |  |  |  |
| f1560 | f1560..1664 | 0x1560 | open | open |  |  |  |
| f1696 | f1696..18f2 | 0x1696 | open | open |  |  |  |
| f18f2 | f18f2..1f96 | 0x18f2 | open | open |  |  |  |
| f1f96 | f1f96..2056 | 0x1f96 | open | open |  |  |  |
| f2056 | f2056..22a4 | 0x2056 | open | open |  |  |  |
| f22a4 | f22a4..2346 | 0x22a4 | open | open |  |  |  |
| f23fe | f23fe..2492 | 0x23fe | open | open |  |  |  |
| f2492 | f2492..2624 | 0x2492 | open | open |  |  |  |
| f2624 | f2624..27d8 | 0x2624 | open | open |  |  |  |
| f27d8 | f27d8..2cbc | 0x27d8 | open | open |  |  |  |
| f2d4c | 0x0d8c-0x0e1a | 0x2d4c | confirmed | differs | D only (pattern-verified) | D10, D2, D6, D7 |  |
| f2dde | 0x0558-0x05c6 | 0x2dde | confirmed | exact | D only (pattern-verified) |  |  |
| f2e76 | 0x0e1a-0x0eae | 0x2e76 | confirmed | differs | D only (manual reading) | D2 | arithmetic shift of the count with another register (r5 here, r9 in the original); D2 |
| f2f04 | 0x0526-0x0558 | 0x2f04 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f2f3a | f2f3a..3090 | 0x2f3a | open | open |  |  |  |
| f3090 | 0x0194-0x01a8 | 0x3090 | confirmed | exact | D only (pattern-verified) |  |  |
| f30a4 | f30a4..3144 | 0x30a4 | open | open |  |  |  |
| f3144 | f3144..3408 | 0x3144 | open | open |  |  |  |
| f3408 | f3408..34b8 | 0x3408 | open | open |  |  |  |
| f3516 | f3516..3760 | 0x3516 | open | open |  |  |  |
| f3760 | f3760..3a30 | 0x3760 | open | open |  |  |  |
| f3a88 | 0x0262-0x02a0 | 0x3a88 | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| f3b1e |  | 0x3b1e | open | open |  |  |  |
| f3b58 | 0x0c64-0x0d04 | 0x3b58 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7, D8, D8/D2 |  |
| f3c16 | f3c16..410c | 0x3c16 | open | open |  |  |  |
| f410c | f410c..42f8 | 0x410c | open | open |  |  |  |
| f42f8 | f42f8..4444 | 0x42f8 | open | open |  |  |  |
| f44e6 | f44e6..4968 | 0x44e6 | open | open |  |  |  |
| f4968 | f4968..4a16 | 0x4968 | open | open |  |  |  |
| f4a16 | f4a16..5a2c | 0x4a16 | open | open |  |  |  |
| f5a2c | f5a2c..5eec | 0x5a2c | open | open |  |  |  |
| f5eec | f5eec..60f0 | 0x5eec | open | open |  |  |  |
| f60f0 | 0x02a0-0x02d8 | 0x60f0 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D7, D8/D2 |  |
| f6134 | 0x07ae-0x0890 | 0x6134 | confirmed | exact | D only (pattern-verified) | D3, D6, D7 |  |
| f620e | f620e..642e | 0x620e | open | open |  |  |  |
| f64d0 | f64d0..6522 | 0x64d0 | open | open |  |  |  |
| f6522 | f6522..65c6 | 0x6522 | open | open |  |  |  |
| f6684 | f6684..6800 | 0x6684 | open | open |  |  |  |
| f6800 | 0x0bfe-0x0c64 | 0x6800 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f6866 | f6866..68a2 | 0x6866 | open | open |  |  |  |
| f68a2 | f68a2..6a08 | 0x68a2 | open | open |  |  |  |
| f6a08 | 0x030a-0x035e | 0x6a08 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6a58 | f6a58..6b5a | 0x6a58 | open | open |  |  |  |
| f6b5a | 0x035e-0x038c | 0x6b5a | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6b84 | 0x038c-0x03b6 | 0x6b84 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6bae | 0x094c-0x0a04 | 0x6bae | confirmed | differs | D only (pattern-verified) | D7 |  |
| f6c6a | f6c6a..6da8 | 0x6c6a | open | open |  |  |  |
| f6da8 | 0x0a04-0x0a76 | 0x6da8 | confirmed | differs | D only (pattern-verified) | D10, D13, D7, D8, D8/D2 |  |
| f6e1a | f6e1a..6efe | 0x6e1a | open | open |  |  |  |
| f6efe | 0x03b6-0x0406 | 0x6efe | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6f46 | f6f46..6fd0 | 0x6f46 | open | open |  |  |  |
| f6ffe | 0x0434-0x0460 | 0x6ffe | confirmed | exact | D only (pattern-verified) |  |  |
| f702a | 0x0460-0x0494 | 0x702a | confirmed | exact | D only (pattern-verified) |  |  |
| f705e | f705e..70ce | 0x705e | open | open |  |  |  |
| f7112 | f7112..719e | 0x7112 | open | open |  |  |  |
| f71ca | f71ca..7286 | 0x71ca | open | open |  |  |  |
| f7286 | f7286..7300 | 0x7286 | open | open |  |  |  |
| f7300 | f7300..7526 | 0x7300 | open | open |  |  |  |
| f7526 | f7526..7808 | 0x7526 | open | open |  |  |  |
| f7808 | f7808..7956 | 0x7808 | open | open |  |  |  |
| f79ca | f79ca..7bbc | 0x79ca | open | open |  |  |  |
| f7bbc | 0x0b8e-0x0bfe | 0x7bbc | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| f7c26 | f7c26..85f8 | 0x7c26 | open | open |  |  |  |
| f85f8 | 0x0890-0x0922 | 0x85f8 | confirmed | differs | D only (manual reading) | D2 | indirect byte store through another register pair (rr2 here, rr4 in the original); D2 |
| f_8000 |  | 0x8000 | open | proven: target of a call in the code |  |  |  |
| f_8924 |  | 0x8924 | open | proven: target of a call in the code |  |  |  |
| f_89ba |  | 0x89ba | open | proven: target of a call in the code |  |  |  |
| f_8cb4 |  | 0x8cb4 | open | proven: target of a call in the code |  |  |  |
| f_8f7c |  | 0x8f7c | open | proven: target of a call in the code |  |  |  |
| f_903a |  | 0x903a | open | proven: target of a call in the code |  |  |  |
| f_946e |  | 0x946e | open | proven: target of a call in the code |  |  |  |
| f_96be |  | 0x96be | open | proven: target of a call in the code |  |  |  |
| finish_output | 0x0d04-0x0d8c | 0x2cbc | confirmed | exact | D only (pattern-verified) | D10, D7 |  |
| flush_buffer | 0x06c6-0x0726 | 0x3ac6 | confirmed | differs | D only (pattern-verified) | D1, D3, D7 |  |
| high_byte | 0x0166-0x0194 | 0x2e4c | confirmed | exact | D only (pattern-verified) | D7 |  |
| init_entry | 0x0eae-0x0f66 | 0x2346 | confirmed | exact | D only (pattern-verified) |  |  |
| init_signals | 0x011c-0x0166 | 0x0aba | confirmed | exact | D only (pattern-verified) |  |  |
| list_move_after | 0x0a76-0x0b18 | 0x642e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| load_header | 0x0f66-0x101e | 0x14a4 | confirmed | exact | D only (pattern-verified) | D7 |  |
| make_ccl | 0x0b18-0x0b8e | 0x7956 | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D8/D2 |  |
| sig_handler | 0x00be-0x011c | 0x0a5c | confirmed | exact | D only (pattern-verified) |  |  |
| str_compare | 0x020a-0x0262 | 0x3a30 | confirmed | differs | D only (pattern-verified) | D13 |  |
| swap_bytes | 0x0922-0x094c | 0x65c6 | confirmed | differs | D only (manual reading) | D2 | indirect word load through another register pair (rr4 here, rr2 in the original); D2 |
| swap_pair | 0x02d8-0x030a | 0x65ec | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| swap_quad | 0x04c0-0x0526 | 0x6620 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| table16_entry | 0x0406-0x0434 | 0x6fd0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| table16_index | 0x0726-0x0780 | 0x70ce | confirmed | differs | D only (pattern-verified) | D1, D10, D16, D2, D3, D7 |  |
| table32_entry | 0x0494-0x04c0 | 0x719e | confirmed | exact | D only (pattern-verified) |  |  |
| table_lookup | 0x01a8-0x020a | 0x34b8 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7 |  |
| unpack_run | 0x0000-0x0094 | 0x0056 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D8 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
