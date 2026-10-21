# libc members inside the programs

Made by `host/tools/libcver.py --catalog` from the original file tree. "identical" = the member of
the shipped `lib/libc.a` is in the program instruction by instruction (addresses and relocated values
ignored). "other" = functions behind the own code that no shipped member explains; the nearest
member is a hint only (ratio below 0.6: do not trust the name).

## Versions

| Version | Programs | Functions that differ from the shipped libc |
|---|---|---|
| shipped | ar, getud, putud, tar, dump, dumpdir, restor, copydisk, format, verify, sync | 0 |
| cc | cc, scc | 9 |
| size | size | 13 |
| strip | strip | 14 |
| ranlib | ranlib | 10 |

## Functions that occur in more than one version

| fingerprint | instructions | nearest shipped member | versions |
|---|---|---|---|
| 8c43ca44 | 78 | cnvt | cc, size, strip |
| e405d4a4 | 14 | length | cc, size, strip |
| 25a4f550 | 44 | ? (rew 0.20) | size, strip |
| 77b78fc7 | 56 | ? (ttyname 0.22) | size, strip |
| 0f843545 | 69 | ? (swap 0.16) | size, strip |
| 889e63ad | 57 | swap | size, strip |

## bin/ar (version shipped)

3438 instructions, libc part from 0d7c, 33 identical members.

SCCS ids in the file: ar.c 2.2 11/18/83; malloc.c 3.3 09/12/83

Identical members:

printf doprnt stfl ffltioc flsbuf cuexit ctime getenv perror cnvt decimal length malloc isatty mktemp stty strcmp strcpy strlen access brk close creat fstat getpid ioctl lseek open read signal unlink write cerror

Same libc part: getud, putud, tar, dump, dumpdir, restor, copydisk, format, verify, sync.

## bin/cc (version cc)

3711 instructions, libc part from 15f2, 31 identical members.

SCCS ids in the file: none

Identical members:

freopen fopen ffltioc flsbuf endopen findiop cuexit abort decimal isatty stty strcmp access brk chmod close creat execl execv fork getpid ioctl lseek open signal stat umask unlink wait write cerror

| address | instructions | fingerprint | nearest shipped member |
|---|---|---|---|
| 1638..17ca | 93 | 470a9d98 | ? (printf 0.19) |
| 17cc..20de | 714 | c85aab55 | ? (doprnt 0.58) |
| 20e0..2154 | 43 | 66709a12 | ? (atoi 0.37) |
| 2506..25f8 | 78 | 8c43ca44 | cnvt |
| 2686..26a2 | 14 | e405d4a4 | length |
| 26a4..2832 | 140 | 82fa7ec3 | ? (getopt 0.10) |
| 2834..292c | 97 | 48cabf2a | ? (rdwr 0.13) |
| 2978..29a6 | 22 | c5b2d1ff | strcat |
| 29d6..2a12 | 29 | ab8c4f1e | ? (strcpy 0.57) |

Same libc part: scc.

## bin/size (version size)

1863 instructions, libc part from 031e, 18 identical members.

SCCS ids in the file: size.c 2.1 01/14/83

Identical members:

fopen ffltioc cuexit decimal isatty stty swab brk chmod close creat ioctl lseek open read umask write cerror

| address | instructions | fingerprint | nearest shipped member |
|---|---|---|---|
| 033c..04ea | 106 | e9cd24f5 | ? (flsbuf 0.10) |
| 04ec..0acc | 467 | f4407d47 | ? (doprnt 0.35) |
| 0ace..0b48 | 45 | 6c7ccb23 | ? (atoi 0.33) |
| 0b52..0d1e | 155 | c9623877 | ? (flsbuf 0.45) |
| 0d20..0d9c | 44 | 25a4f550 | ? (rew 0.20) |
| 0d9e..0f66 | 147 | 0663f888 | ? (endopen 0.51) |
| 0f68..1002 | 56 | 77b78fc7 | ? (ttyname 0.22) |
| 100e..1100 | 78 | 8c43ca44 | cnvt |
| 118e..11aa | 14 | e405d4a4 | length |
| 11ac..130a | 122 | a2e8c8c2 | ? (getopt 0.11) |
| 130c..13c8 | 69 | 0f843545 | ? (swap 0.16) |
| 1452..14a2 | 29 | 206a8659 | goodmagic |
| 14a4..1544 | 57 | 889e63ad | swap |

## usr/bin/strip (version strip)

2668 instructions, libc part from 08c2, 23 identical members.

SCCS ids in the file: strip.c 1.2 11/19/82

Identical members:

fopen ffltioc cuexit decimal isatty stty swab strcmp access brk chmod close creat getpid ioctl lseek open read signal umask unlink write cerror

| address | instructions | fingerprint | nearest shipped member |
|---|---|---|---|
| 08e0..0a80 | 102 | de24504a | ? (printf 0.15) |
| 0a82..1354 | 693 | 4ac45421 | ? (doprnt 0.41) |
| 1356..13ca | 43 | b2cda47b | ? (atoi 0.37) |
| 13d4..159c | 154 | 5f38a5b8 | ? (flsbuf 0.45) |
| 159e..161a | 44 | 25a4f550 | ? (rew 0.20) |
| 161c..17b4 | 134 | 38d8d9d3 | endopen |
| 17b6..1850 | 56 | 77b78fc7 | ? (ttyname 0.22) |
| 185c..194e | 78 | 8c43ca44 | cnvt |
| 19dc..19f8 | 14 | e405d4a4 | length |
| 19fa..1b54 | 121 | 2e441bde | ? (system 0.12) |
| 1b56..1c12 | 69 | 0f843545 | ? (swap 0.16) |
| 1c32..1caa | 44 | f5425dcb | mktemp |
| 1d44..1dba | 46 | 5c14631d | goodmagic |
| 1dbc..1e5c | 57 | 889e63ad | swap |

## usr/bin/ranlib (version ranlib)

2408 instructions, libc part from 101a, 18 identical members.

SCCS ids in the file: ranlib.c 1.1 07/24/82

Identical members:

ffltioc cuexit decimal brk close creat execl exit fork ioctl lseek read signal time unlink wait write cerror

| address | instructions | fingerprint | nearest shipped member |
|---|---|---|---|
| 1020..11dc | 152 | 2cf69cbf | ? (filbuf 0.59) |
| 11de..125a | 44 | 9d864275 | ? (setbuf 0.25) |
| 125c..13e0 | 133 | abbfe20b | ? (endopen 0.48) |
| 13e2..1454 | 40 | 8a918e66 | ? (cuserid 0.21) |
| 1460..1562 | 83 | 745c2a69 | ? (cnvt 0.53) |
| 15f0..160c | 14 | d8a2ce55 | ? (strlen 0.46) |
| 160e..176a | 122 | ec7c4083 | ? (getopt 0.12) |
| 176c..1992 | 210 | 98f1b448 | ? (execvp 0.11) |
| 1994..1a38 | 58 | 801487d0 | swap |
| 1aea..1af4 | 6 | 6328fe36 | open |

