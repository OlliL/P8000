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
