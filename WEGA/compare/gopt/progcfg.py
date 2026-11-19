"""Configuration of the recon toolkit (host/recon/progcfg.py) for gopt (segmented E611, two code segments 4 and 5, no source known).

The original has the code in segment 4 (65534 bytes) and segment 5 (50432 bytes); the tools of host/recon know one code segment,
so an address of the findings is written SEG:OFFSET (4:0042, 5:0114), see compare/gopt/README.md.
"""
NAME = 'gopt'
ORIG = 'lib/gopt'
OWN_LIM = 0x1c4fe   # all of the code (65534 + 50432 bytes): the program has no library part below the stripped start; libc is decided later
ORIG_LIM = OWN_LIM
SEG = True
CC = '/bin/scc'
CODESEGS = {4: (0, 65534), 5: (65534, 50432)}    # segment -> (image offset, size)
# module order = link order (SCCS lines of the original); common is the pcc file of System III
MODULES = ['reader2', 'quads2', 'debug2', 'rdr2', 'comm2', 'common', 'wrtil2', 'alias2', 'flow2', 'dom2', 'init2', 'live2',
           'ebbo2', 'lpinv2', 'lpind2', 'beta2', 'reg2', 'alias2b']
NOT_MODULE = ('mfile2.h', 'common', 'rdr')   # SCCS lines of included files: comm2.c includes common, rdr2.c includes rdr
COMMONS = (0x4d70, 0x4da0)                  # words of segment 7 that functions of every module read (not an anchor)
# the libc members in the original (names from their code: odisc.py shows them in the listings)
LIBNAMES = {(5, 0xa94e): 'printf', (5, 0xa9b0): 'fprintf', (5, 0xb81e): '_flsbuf', (5, 0xb712): '_filbuf', (4, 0xfc3a): 'fseek',
            (4, 0xfd9a): 'ftell', (4, 0xfe50): 'fread', (4, 0xff78): 'getw', (5, 0xb932): 'fflush', (5, 0xbc32): 'exit',
            (5, 0xc332): 'mkseg', (5, 0xc392): 'sgbrk', (5, 0xc3a2): 'ssgbrk', (5, 0xb2ba): 'ungetc', (5, 0xa1aa): 'fscanf',
            (5, 0xbc2a): '__callex'}
