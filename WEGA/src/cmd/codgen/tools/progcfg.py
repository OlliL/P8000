"""Configuration of the recon toolkit (host/recon/progcfg.py) for codgen."""
NAME = 'codgen'
ORIG = 'lib/codgen'
OWN_LIM = 0xbbc4   # the program part of the original ends here (the C library follows)
OWN_OBJ = (r'^(crt0|allo3|local3|match3|order3|out3|regallo3|regfile3|tmp3|zzz3|reader3|gen3|optim3|'
           r'comm3|rdr3|main3|efcode3|lcl3|tr3|regs3|table3_c|ops_codg|hop_codg)')
